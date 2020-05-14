void mqttPublish(PubSubClient cl, String top, String user, String action, String room) {
  String msg = "{\"name\":\"" + user + "\",\"action\":\"" + action + "\",\"room\":" + room + "}";
  cl.publish(top, msg);
  Serial.println("Published");
}

void readMqttCredentials() {
  // read eeprom for mqtt credentials
  Serial.println("Reading EEPROM mqtt address");
  for (int i = 97; i < 161; ++i)
    {
      maddress += char(EEPROM.read(i));
    }
  Serial.print("Mqtt address: ");
  Serial.println(maddress);
  Serial.println("Reading EEPROM mqtt topic");
  for (int i = 161; i < 177; ++i)
    {
      mtopic += char(EEPROM.read(i));
    }
  Serial.print("Topic: ");
  Serial.println(mtopic);  
  // read eeprom for ssid and pass
  Serial.println("Reading EEPROM mqtt port");
  for (int i = 177; i < 209; ++i)
    {
      strport += char(EEPROM.read(i));
    }
  Serial.print("Port: ");
  mport = strport.toInt();
  Serial.println(mport);
  Serial.println("Reading EEPROM mqtt user");
  for (int i = 209; i < 241; ++i)
    {
      muser += char(EEPROM.read(i));
    }
  Serial.print("User: ");
  Serial.println(muser);
  Serial.println("Reading EEPROM mqtt pass");
  for (int i = 241; i < 305; ++i)
    {
      mpass += char(EEPROM.read(i));
    }
  Serial.print("Pass: ");
  Serial.println(mpass);

  Serial.println("Access point credentials.");
  Serial.println("Reading EEPROM AP ssid");
  for (int i = 305; i < 337; ++i)
    {
      ssid += char(EEPROM.read(i));
    }
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.println("Reading EEPROM AP passphrase");
  for (int i = 337; i < 401; ++i)
    {
      passphrase += char(EEPROM.read(i));
    }
  Serial.print("SSID: ");
  Serial.println(passphrase);
}


void mqttConnect()
{
  if (WiFi.status() == WL_CONNECTED) 
    {
    if (!client.connected()) {
      Serial.println("Connecting to MQTT server");
      if (counter > 5)
      {
        ESP.restart();
      }
      counter++;
      if (client.connect(MQTT::Connect("arduinoClient2")
      .set_auth("euveaslz", "EeAR0N6BgvpM"))) {
        Serial.println("Connected to MQTT server");
        client.set_callback(callback);
        client.subscribe("server"); // підписуємось на топік з даними світлодіода
      } else {
        Serial.println("Could not connect to MQTT server");
      }
    }
  }
}



// Функція отримування даних від сервера
void callback(const MQTT::Publish& pub)
{
  Serial.print(pub.topic()); // вивід топіка в серіал порт
  Serial.print(" => ");
  Serial.println(pub.payload_string()); // вивід даних
  
  String payload = pub.payload_string();
  
  if(String(pub.topic()) == "server") // перевірка топіка
  {
    isAllowed = pub.payload_string();
  }
}
