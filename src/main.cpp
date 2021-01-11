#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// define MFRC522 pins
#define RST_PIN 5
#define SS_PIN 4
// define led and button pin
#define green_led 16
#define button 15

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
	Serial.begin(9600);		// Initialize serial communications with the PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	pinMode(button,INPUT);
	pinMode(green_led, OUTPUT);
}

void loop() {
	digitalWrite(green_led, LOW);
// Revisamos si hay nuevas tarjetas  presentes
	if ( mfrc522.PICC_IsNewCardPresent()){
  		//Seleccionamos una tarjeta
            if ( mfrc522.PICC_ReadCardSerial()){
                  // Enviamos serialemente su UID
                	for (byte i = 0; i < mfrc522.uid.size; i++) {
                    	Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                        Serial.print(mfrc522.uid.uidByte[i], HEX);
                }
                Serial.print('\n');
                mfrc522.PICC_HaltA();

            }
	}
	while (digitalRead(button) == HIGH){
    	if (mfrc522.PICC_IsNewCardPresent()){
    		//Seleccionamos una tarjeta
            if ( mfrc522.PICC_ReadCardSerial()){
                  // Enviamos serialemente su UID
                Serial.print('r');
                for (byte i = 0; i < mfrc522.uid.size; i++) {
                    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                    Serial.print(mfrc522.uid.uidByte[i], HEX);
                }
                Serial.print('\n');
            	mfrc522.PICC_HaltA();
                break;
            }
    	}
	}
	if (Serial.available() > 0){
    char comando = Serial.read();
    if (comando == 'o'){
    	digitalWrite(green_led, HIGH);
    	delay(2000);
	}
	}
}