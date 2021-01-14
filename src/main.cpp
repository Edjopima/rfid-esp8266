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
    	WiFiClient client;
    	HTTPClient http;
		if ( mfrc522.PICC_IsNewCardPresent()){
		String uid = "";
  		//Seleccionamos una tarjeta
            if ( mfrc522.PICC_ReadCardSerial()){
                  // Enviamos serialemente su UID
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
				Serial.print('\n');
				String json_data = "{\"action\":\"open\",\"uid\":\"" + uid +"\"}";
				Serial.print(json_data);
                Serial.print('\n');
                mfrc522.PICC_HaltA();
    			http.begin(client,"http://192.168.31.204:3000/action");
				http.addHeader("Content-Type", "application/json");
				int httpCode = http.POST(json_data);
    			String respuesta = http.getString();
				Serial.print(httpCode);
    			Serial.print(respuesta);
    			http.end();
    			delay(1000);

            }
	}
	while (digitalRead(button) == HIGH){
    	if (mfrc522.PICC_IsNewCardPresent()){
			String uid = "";
    		//Seleccionamos una tarjeta
            if ( mfrc522.PICC_ReadCardSerial()){
                  // Enviamos serialemente su UID
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
				String json_data = "{\"action\":\"register\",\"uid\":\"" + uid +"\"}";
				Serial.print(json_data);
                Serial.print('\n');
            	mfrc522.PICC_HaltA();
				http.begin(client,"http://192.168.31.204:3000/action");
				http.addHeader("Content-Type", "application/json");
				int httpCode = http.POST(json_data);
    			String respuesta = http.getString();
				Serial.print(httpCode);
    			Serial.print(respuesta);
                break;
            }
    	}
	}
	}
}