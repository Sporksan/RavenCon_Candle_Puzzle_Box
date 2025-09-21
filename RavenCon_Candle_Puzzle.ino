/*Down and dirty color theory puzzle made for RavenCon 2026
Modified version from 

*/

#include <MFRC522.h>
#include <MFRC522Extended.h>

// DEFINES
// Provides debugging information over serial connection if defined
#define DEBUG

// LIBRARIES
// Standard SPI library comes bundled with the Arduino IDE
#include <SPI.h>
// Download and install from https://github.com/miguelbalboa/rfid
#include <MFRC522.h>

// CONSTANTS
// The number of RFID readers
const byte numReaders = 2;
// Each reader has a unique Slave Select pin
const byte ssPins[] = {2, 3};
//const byte ssPins[] = {2, 3, 4, 5};
// They'll share the same reset pin
const byte resetPin = 8;
// This pin will be driven LOW to release a lock when puzzle is solved
const byte lockPin = A0;
// These pin will be driven HIGH to light candles
const byte orangeCandle = A1;
const byte purpleCandle = A3;
const byte greenCandle = A2;
// Associate colors to RFID Tags for ease of use.
const String blue = "93eec812";
const String red = "e175e926";
const String yellow = "33386f9a";



// GLOBALS
// Initialise an array of MFRC522 instances representing each reader
MFRC522 mfrc522[numReaders];
// The tag IDs currently detected by each reader
String currentIDs[numReaders];
// Initalize candle states as off
boolean purpleLit = false;
boolean orangeLit = false;
boolean greenLit = false;

/**
 * Initialisation
 */
void setup() {

  #ifdef DEBUG
  // Initialise serial communications channel with the PC
  Serial.begin(9600);
  Serial.println(F("Serial communication started"));
  #endif
  
  // Set the lock pin as output and secure the lock
  pinMode(lockPin, OUTPUT);
  digitalWrite(lockPin, HIGH);

  // Sets the Candle pins as outputs and makes sure they are off
  pinMode(orangeCandle, OUTPUT);
  digitalWrite(orangeCandle, LOW);
  pinMode(purpleCandle, OUTPUT);
  digitalWrite(purpleCandle, LOW);
  pinMode(greenCandle, OUTPUT);
  digitalWrite(greenCandle, LOW);
  
	// We set each reader's select pin as HIGH (i.e. disabled), so 
	// that they don't cause interference on the SPI bus when
	// first initialised
	for (uint8_t i=0; i<numReaders; i++) {
		pinMode(ssPins[i], OUTPUT);
		digitalWrite(ssPins[i], HIGH);
	}
	
  // Initialise the SPI bus
  SPI.begin();

  for (uint8_t i=0; i<numReaders; i++) {
  
    // Initialise the reader
    // Note that SPI pins on the reader must always be connected to certain
    // Arduino pins (on an Uno, MOSI=> pin11, MISO=> pin12, SCK=>pin13)
    // The Slave Select (SS) pin and reset pin can be assigned to any pin
    mfrc522[i].PCD_Init(ssPins[i], resetPin);
    
    // Set the gain to max - not sure this makes any difference...
    // mfrc522[i].PCD_SetAntennaGain(MFRC522::PCD_RxGain::RxGain_max);
    
	#ifdef DEBUG
    // Dump some debug information to the serial monitor
    Serial.print(F("Reader #"));
    Serial.print(i);
    Serial.print(F(" initialised on pin "));
    Serial.print(String(ssPins[i]));
    Serial.print(F(". Antenna strength: "));
    Serial.print(mfrc522[i].PCD_GetAntennaGain());
    Serial.print(F(". Version : "));
    mfrc522[i].PCD_DumpVersionToSerial();
	#endif
    
    // Slight delay before activating next reader
    delay(100);
  }
  
  #ifdef DEBUG
  Serial.println(F("--- END SETUP ---"));
  #endif
}

/**
 * Main loop
 */
void loop() {
  // Assume that the puzzle has been solved
  boolean puzzleSolved = true;

  // Assume that the tags have not changed since last reading
  boolean changedValue = false;

  // Loop through each reader
  for (uint8_t i=0; i<numReaders; i++) {

    // Initialise the sensor
    mfrc522[i].PCD_Init();
    

    // String to hold the ID detected by each sensor
    String readRFID = "";
    
    // If the sensor detects a tag and is able to read it
    if(mfrc522[i].PICC_IsNewCardPresent() && mfrc522[i].PICC_ReadCardSerial()) {
      // Extract the ID from the tag
      readRFID = dump_byte_array(mfrc522[i].uid.uidByte, mfrc522[i].uid.size);
    }
    
    // If the current reading is different from the last known reading
    if(readRFID != currentIDs[i]){
      // Set the flag to show that the puzzle state has changed
      changedValue = true;
      // Update the stored value for this sensor
      currentIDs[i] = readRFID;
    }
    
    //Test is correct tages are present to solve purple
    if ((currentIDs[0] == red && currentIDs[1] == blue)||(currentIDs[1] == red && currentIDs[0] == blue)) {
        Serial.print(F("Purple Candle Lit!"));
        Serial.println();
        purpleLit = true;
        digitalWrite(purpleCandle, HIGH);
    }

    //Test is correct tages are present to solve orange
    if ((currentIDs[1] == red && currentIDs[0] == yellow)||(currentIDs[0] == red && currentIDs[1] == yellow)) {
        Serial.print(F("Orange Candle Lit!"));
        Serial.println();
        orangeLit = true;
        digitalWrite(orangeCandle, HIGH);
    }
    
    //Test is correct tages are present to solve green
    if ((currentIDs[1] == blue && currentIDs[0] == yellow)||(currentIDs[0] == blue && currentIDs[1] == yellow)) {
        Serial.print(F("Green Candle Lit!"));
        Serial.println();
        greenLit = true;
        digitalWrite(greenCandle, HIGH);
    }

    // Halt PICC
    mfrc522[i].PICC_HaltA();
    // Stop encryption on PCD
    mfrc522[i].PCD_StopCrypto1(); 
  }

  #ifdef DEBUG
  // If the changedValue flag has been set, at least one sensor has changed
  if(changedValue){
    // Dump to serial the current state of all sensors
    for (uint8_t i=0; i<numReaders; i++) {
      Serial.print(F("Reader #"));
      Serial.print(String(i));
      Serial.print(F(" on Pin #"));
      Serial.print(String((ssPins[i])));
      Serial.print(F(" detected tag: "));
      Serial.println(currentIDs[i]);
      Serial.println();
    }
    Serial.println(F("---"));
  }
  #endif

  // If the puzzleSolved flag is set, all sensors detected the correct ID
  if(greenLit && purpleLit && orangeLit){
    onSolve();
  }

  // Add a short delay before next polling sensors
  delay(100); 
}

/**
 * Called when correct puzzle solution has been entered
 */
void onSolve(){

  #ifdef DEBUG
  // Print debugging message
  Serial.println(F("Puzzle Solved!"));
  #endif
  
  // Release the lock
  digitalWrite(lockPin, LOW);

  while(true) {
    delay(1000);
  }
  
}

String dump_byte_array(byte *buffer, byte bufferSize) {
  String read_rfid = "";
  for (byte i=0; i<bufferSize; i++) {
    read_rfid = read_rfid + String(buffer[i], HEX);
  }
  return read_rfid;
}