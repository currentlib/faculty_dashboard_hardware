bool testWifi(void) {
  int c = 0;
  Serial.println("Waiting for Wifi to connect");  
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED) { return true; } 
    delay(500);
    Serial.print(WiFi.status());    
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
} 

void launchWeb(int webtype) {
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer(webtype);
  Serial.println("Server created"); 
  server.begin();
  Serial.println("Server started"); 
}

void setupAP(void) {
  isAP = true;
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  delay(100);
  WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
     }
  }
  Serial.println(""); 
  st = "<ol>";
  for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      st += "<li>";
      st += WiFi.SSID(i);
      st += " (";
      st += WiFi.RSSI(i);
      st += ")";
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
      st += "</li>";
    }
  st += "</ol>";
  delay(100);
  Serial.println("SSSSSSSIIIID: " + ssid + passphrase);
  WiFi.softAP(ssid, passphrase);
  Serial.println("softap");
  launchWeb(0);
  Serial.println("over");
}

void createWebServer(int webtype)
{
//  if ( webtype == 1 ) {
    server.on("/", []() {
        IPAddress ip = WiFi.localIP();
        String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        content = "<!DOCTYPE HTML>\r\n<html>ESP8266 Settings at ";
        content += ipStr;
        content += "<p>";
        content += st;
        content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><br><label>Pass: </label><input name='pass' length=64><br><input type='submit'></form>";
        content += "<br>Mqtt settings<br>";
        content += "</p><form method='get' action='mqtt'><label>Address: </label><input name='address' length=64><br>";
        content += "<label>Port: </label><input name='port' length=16><br>";
        content += "<label>Topic: </label><input name='topic' length=32><br>";
        content += "<label>Username: </label><input name='user' length=32><br>";
        content += "<label>Password: </label><input name='pass' length=64><br><input type='submit'></form>";
        content += "<br>Access Point settings<br>";
        content += "<form method='get' action='ap'><label>SSID: </label><input name='apssid' length=32><br><label>Pass: </label><input name='appass' length=64><br><input type='submit'></form>";
        content += "<br>Mqtt settings<br>";
        content += "</html>";
        server.send(200, "text/html", content);  
    });
    server.on("/ap", []() {
        String apssid = server.arg("apssid");
        String appass = server.arg("appass");
        if (apssid.length() > 0 && appass.length() > 0) {
          Serial.println("clearing eeprom");
          for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
          Serial.println(apssid);
          Serial.println("");
          Serial.println(appass);
          Serial.println("");
            
          Serial.println("writing eeprom ssid:");
          for (int i = 0; i < apssid.length(); ++i)
            {
              EEPROM.write(305+i, apssid[i]);
              Serial.print("Wrote: ");
              Serial.println(apssid[i]); 
            }
          Serial.println("writing eeprom pass:"); 
          for (int i = 0; i < appass.length(); ++i)
            {
              EEPROM.write(337+i, appass[i]);
              Serial.print("Wrote: ");
              Serial.println(appass[i]); 
            }    
          EEPROM.commit();
          content = "{\"Success\":\"wifi access point credentials saved to eeprom... reset to boot into new wifi\"}";
          statusCode = 200;
        } else {
          content = "{\"Error\":\"404 not found\"}";
          statusCode = 404;
          Serial.println("Sending 404");
        }
        server.send(statusCode, "application/json", content);
    });
    server.on("/mqtt", []() {
        String qaddress = server.arg("address");
        String qtopic = server.arg("topic");
        String qport = server.arg("port");
        String quser = server.arg("user");
        String qpass = server.arg("pass");
        if (qaddress.length() > 0 && 
            qtopic.length() > 0 && 
            qport.length() > 0 &&
            quser.length() > 0 &&
            qpass.length() > 0) {
          Serial.println("clearing eeprom");
          for (int i = 97; i < 305; ++i) { EEPROM.write(i, 0); }
          Serial.println(qaddress);
          Serial.println("");
          Serial.println(qtopic);
          Serial.println("");
          Serial.println(qport);
          Serial.println("");
          Serial.println(quser);
          Serial.println("");
          Serial.println(qpass);
          Serial.println("");
            
          Serial.println("writing eeprom address:");
          for (int i = 0; i < qaddress.length(); ++i)
            {
              EEPROM.write(97+i, qaddress[i]);
              Serial.print("Wrote: ");
              Serial.println(qaddress[i]); 
            }
          Serial.println("writing eeprom topic:"); 
          for (int i = 0; i < qtopic.length(); ++i)
            {
              EEPROM.write(161+i, qtopic[i]);
              Serial.print("Wrote: ");
              Serial.println(qtopic[i]); 
            }    
          Serial.println("writing eeprom port:");
          for (int i = 0; i < qport.length(); ++i)
            {
              EEPROM.write(177+i, qport[i]);
              Serial.print("Wrote: ");
              Serial.println(qport[i]); 
            }  
          Serial.println("writing eeprom user:");
          for (int i = 0; i < quser.length(); ++i)
            {
              EEPROM.write(209+i, quser[i]);
              Serial.print("Wrote: ");
              Serial.println(quser[i]); 
            }  
          Serial.println("writing eeprom password:");
          for (int i = 0; i < qpass.length(); ++i)
            {
              EEPROM.write(241+i, qpass[i]);
              Serial.print("Wrote: ");
              Serial.println(qpass[i]); 
            }  
          EEPROM.commit();
          content = "{\"Success\":\"mqtt credentials saved to eeprom... reset to boot with new settings\"}";
          statusCode = 200;
        } else {
          content = "{\"Error\":\"404 not found\"}";
          statusCode = 404;
          Serial.println("Sending 404");
        }
        server.send(statusCode, "application/json", content);
    });
    server.on("/setting", []() {
        String qsid = server.arg("ssid");
        String qpass = server.arg("pass");
        if (qsid.length() > 0 && qpass.length() > 0) {
          Serial.println("clearing eeprom");
          for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
          Serial.println(qsid);
          Serial.println("");
          Serial.println(qpass);
          Serial.println("");
            
          Serial.println("writing eeprom ssid:");
          for (int i = 0; i < qsid.length(); ++i)
            {
              EEPROM.write(i, qsid[i]);
              Serial.print("Wrote: ");
              Serial.println(qsid[i]); 
            }
          Serial.println("writing eeprom pass:"); 
          for (int i = 0; i < qpass.length(); ++i)
            {
              EEPROM.write(32+i, qpass[i]);
              Serial.print("Wrote: ");
              Serial.println(qpass[i]); 
            }    
          EEPROM.commit();
          content = "{\"Success\":\"wifi credentials saved to eeprom... reset to boot into new wifi\"}";
          statusCode = 200;
        } else {
          content = "{\"Error\":\"404 not found\"}";
          statusCode = 404;
          Serial.println("Sending 404");
        }
        server.send(statusCode, "application/json", content);
    });
}
