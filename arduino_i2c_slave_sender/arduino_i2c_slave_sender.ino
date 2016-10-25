#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9           
#define SS_PIN 10       

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
bool ifread=false;

void setup() {
  Serial.begin(9600);
  SPI.begin();                  // Init SPI bus
  while (!Serial);
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event

  for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
  }
}

void requestEvent() {
  if(ifread) 
    Wire.write(1); // if read the ID card respond with message 1
  else
    Wire.write(0); // if read the ID card respond with message 0
}

void readRFID(){
  MFRC522::StatusCode status;
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak); //get the RFID card type
  Serial.println(mfrc522.PICC_GetTypeName(piccType));
  byte trailerBlock   = 0;

  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status == MFRC522::STATUS_OK) {
      ifread=true;
  }else if(status != MFRC522::STATUS_OK){
      Serial.print(F("PCD_Authenticate() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
  }
  mfrc522.PICC_HaltA(); 
}

void loop() {
  mfrc522.PCD_Init();
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) // Look for new cards
      return;

  if ( ! mfrc522.PICC_ReadCardSerial()) // Select one of the cards
      return;

  readRFID();

  if(ifread){
    delay(5000);
    ifread=false;
  }
  delay(100);
}
