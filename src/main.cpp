#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoJson.h>

// define MFRC522 pins
#define RST_PIN 5
#define SS_PIN 4
// define led and button pin
#define green_led 16
#define button 15

ESP8266WiFiMulti wifimulti;
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

WiFiClient client;
HTTPClient http;

void handle_actions(String uid, String send_action){
				String json_data = "{\"action\":\""+ send_action + "\",\"uid\":\"" + uid +"\"}";
				Serial.print(json_data);
				Serial.print("\n");
    			http.begin(client,"http://192.168.31.204:3000/action");
				http.addHeader("Content-Type", "application/json");
				delay(1000);
				if (http.POST(json_data) == 200){
					String response = http.getString();
					StaticJsonDocument<210> doc;
					deserializeJson(doc, response);
					const char* action = doc["action"];
					const char* uid = doc["uid"];
					const char* name = doc["name"];
					int result = doc["result"];
					int open = doc["open"];

					if (open == 1){
						if (result == 1){
							Serial.print("\nAcceso Autorizado: ");
							Serial.print(name);
							digitalWrite(green_led,HIGH);
							Serial.print("\nOpen");
							delay(2000);
							Serial.print("\nClose");
							digitalWrite(green_led,LOW);
							Serial.print("\n");
						}else{
							Serial.print("\nAcceso no autorizado: ");
							Serial.print(name);
							Serial.print("\n");
						}
					}else if (open == 0){
						if (result == 1){
							Serial.print("\nRegistro Completo: ");
							Serial.print(name);
							digitalWrite(green_led,HIGH);
							Serial.print("\n");
						}else{
							Serial.print("\nError en registro");
							Serial.print("\n");
						}
					}
				}else{
					Serial.print("error");
				}
    			http.end();
    			delay(1000);
}

void setup() {
	Serial.begin(9600);		// Initialize serial communications with the PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	pinMode(button,INPUT);
	pinMode(green_led, OUTPUT);
	delay(1000);
	WiFi.mode(WIFI_STA);
	wifimulti.addAP("INPIMACA", "E151098.A261201");
}

void loop() {
	digitalWrite(green_led, LOW);
// Revisamos si hay nuevas tarjetas  presentes
	if((wifimulti.run() == WL_CONNECTED)) {
		if ( mfrc522.PICC_IsNewCardPresent()){
		String uid = "";
            if ( mfrc522.PICC_ReadCardSerial()){
				for (byte i = 0; i < mfrc522.uid.size; i++) {
					if (mfrc522.uid.uidByte[i] < 0x10){
						uid = uid + " 0";
						} else {
							uid = uid + " ";
						}
					String str = String(mfrc522.uid.uidByte[i],HEX);
					str.toUpperCase();
					uid = uid + str;
                }
			mfrc522.PICC_HaltA();
			handle_actions(uid, "open");
            }
	}
		while (digitalRead(button) == HIGH){
    	if (mfrc522.PICC_IsNewCardPresent()){
			String uid = "";
    		//Seleccionamos una tarjeta
            if ( mfrc522.PICC_ReadCardSerial()){
                for (byte i = 0; i < mfrc522.uid.size; i++) {
					if (mfrc522.uid.uidByte[i] < 0x10){
						uid = uid + " 0";
					} else {
						uid = uid + " ";
					}
					String str = String(mfrc522.uid.uidByte[i],HEX);
					str.toUpperCase();
					uid = uid + str;
                }
				mfrc522.PICC_HaltA();
				handle_actions(uid,"register");
                break;
            }
    	}
	}
	}
}
