#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <MQTT.h>
#include <SPI.h>
#include <MFRC522.h>


#define RST_PIN         0          // Configurable, see typical pin layout above
#define SS_PIN          15         // Configurable, see typical pin layout above
#define GRN_LED         16
#define RED_LED         5

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void readMqttCredentials();
bool testWifi(void);
void launchWeb(int);
void setupAP(void);


ESP8266WebServer server(80);


WiFiClient wclient;
PubSubClient client(wclient, "tailor.cloudmqtt.com", 11847);

String ssid = "";
String passphrase = "";
String st;
String content;
int statusCode;
int counter = 0;


String maddress = "";
String mtopic = "";
int mport = 0;
String strport = "";
String muser = "";
String mpass = "";


String isAllowed = "";


void setup() {
  pinMode(GRN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GRN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  Serial.begin(115200);
  EEPROM.begin(512);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  //Start Wi-Fi client for MQTT client
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);       // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  readMqttCredentials();
  // read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");
  String esid;
  for (int i = 0; i < 32; ++i)
    {
      esid += char(EEPROM.read(i));
    }
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 96; ++i)
    {
      epass += char(EEPROM.read(i));
    }
  Serial.print("PASS: ");
  Serial.println(epass); 
  if ( esid.length() > 1 ) {
      WiFi.begin(esid.c_str(), epass.c_str());
      if (testWifi()) {
        launchWeb(0);
        return;
      } else {
        setupAP();
      }
  }
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent() and mfrc522.PICC_ReadCardSerial()) {
    Serial.println("MQTT"); 
    mqttConnect();
    String id = "";
    for (int i=0; i<4; i++) {
      id+=mfrc522.uid.uidByte[i];
    }
    Serial.println(id); 
    mqttPublish(client, "test", id, "short", "0");
    unsigned long time = millis();
    while ((millis()-time < 5000) && (isAllowed == "")) {
      client.loop();
      delay(100);
    }
    if (isAllowed == "allow") {
      Serial.println("Allowed");
      digitalWrite(GRN_LED, HIGH);
      delay(3000);
      digitalWrite(GRN_LED, LOW);
    } else {
      digitalWrite(RED_LED, HIGH);
      delay(3000);
      digitalWrite(RED_LED, LOW);
    }
    
    isAllowed = "";
  }
  server.handleClient();
}
