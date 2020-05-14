void mqttPublish(PubSubClient cl, String top, String user, String action, String room) {
  String msg = "{\"name\":\"" + user + "\",\"action\":\"" + action + "\",\"room\":" + room + "}";
  cl.publish((char*)top.c_str(), (char*)msg.c_str());
  Serial.println("Published");
}

void readMqttCredentials() {
  for (int i = 97; i < 161; ++i)
    {
      maddress += char(EEPROM.read(i));
    }
  for (int i = 161; i < 177; ++i)
    {
      mtopic += char(EEPROM.read(i));
    }
  for (int i = 177; i < 209; ++i)
    {
      strport += char(EEPROM.read(i));
    }
  for (int i = 209; i < 241; ++i)
    {
      muser += char(EEPROM.read(i));
    }
  for (int i = 241; i < 305; ++i)
    {
      mpass += char(EEPROM.read(i));
    }
  for (int i = 305; i < 337; ++i)
    {
      ssid += char(EEPROM.read(i));
    }
  for (int i = 337; i < 401; ++i)
    {
      passphrase += char(EEPROM.read(i));
    }
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
      String clientName = "ESP8266";
      Serial.print("In connecting to MQTTTTTT: ");
      Serial.println((char*)mpass.c_str());
      client.setServer((char*)maddress.c_str(), mport);
      client.setCallback(callback);
      if (client.connect("ESP8266", (char*)muser.c_str(), (char*)mpass.c_str())) {
        Serial.println("Connected to MQTT server");
        client.subscribe("server"); // підписуємось на топік з даними світлодіода
      } else {
        Serial.println("Could not connect to MQTT server");
        Serial.println(client.state());
      }
    }
  }
}

// Функція отримування даних від сервера
void callback(const char* top, byte* payload, unsigned int length)
{
  String msg = String((char*)payload);
  Serial.print(top);
  Serial.print(" => ");
  Serial.println(msg);
  if(String(top) == "server") {
    isAllowed = msg;
  }
}
