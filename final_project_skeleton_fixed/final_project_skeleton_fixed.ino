#include <Wire.h> // Necessary for I2C communication
#include <LiquidCrystal.h>
#include <ArduinoBLE.h>
BLEService newService("180A");

BLEByteCharacteristic readChar("2A58", BLERead);
BLEByteCharacteristic writeChar("2A57", BLEWrite);

int accel = 0x53; // I2C address for this sensor (from data sheet)
float x, y, z, accvector;
int threshold = 40000;   // Threshold initialized to 40000
int stepcount = 0;       // Step count initialized to 0
bool stepdetected = false;
int distance = 0;       // Step count initialized to 0

LiquidCrystal lcd(10, 8, 2, 3, 4, 5);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2); //Initiate the LCD in a 16x2 configuration

  while(!Serial);
  if (!BLE.begin()){
    Serial.println("Waiting for ArduinoBLE");
    while(1);
  }

  BLE.setLocalName("Danika, Marc, and Priyanka");
  BLE.setAdvertisedService(newService);
  newService.addCharacteristic(readChar);
  newService.addCharacteristic(writeChar);
  BLE.addService(newService);
  
  readChar.writeValue(0);
  writeChar.writeValue(0);

  BLE.advertise();
  Serial.println("Bluetooth device active");

  Wire.begin(); // Initialize serial communications
  Wire.beginTransmission(accel); // Start communicating with the device
  Wire.write(0x2D); // Enable measurement
  Wire.write(8); // Get sample measurement
  Wire.endTransmission();
}

void loop() {

  Wire.beginTransmission(accel);
  Wire.write(0x32); // Prepare to get readings for sensor (address from data sheet)
  Wire.endTransmission(false);
  Wire.requestFrom(accel, 6, true); // Get readings (2 readings per direction x 3 directions = 6 values)
  x = (Wire.read() | Wire.read() << 8); // Parse x values
  y = (Wire.read() | Wire.read() << 8); // Parse y values
  z = (Wire.read() | Wire.read() << 8); // Parse z values
  accvector = sqrt(x*x + y*y+ z*z);

  Serial.println(accvector);

  if (accvector > threshold && stepdetected == false) {
    stepdetected = true; 

  }

  if (accvector < threshold && stepdetected == true) {
    stepcount++;
    stepdetected = false;
    distance = distance + 2;
  }

  lcd.setCursor(0, 0);
  lcd.print("Step Count: " + String(stepcount));
  lcd.setCursor(0, 1);
  lcd.print("Distance: " + String(distance));
  delay(200);

  
  BLEDevice central = BLE.central(); // wait for a BLE central

  if (central) {  // if a central is connected to the peripheral
    // Serial.print("Connected to central: ");
    // Serial.println(central.address()); // print the central's BT address
    
    digitalWrite(LED_BUILTIN, HIGH); // turn on the LED to indicate the connection

    if (central.connected()) { // while the central is connected:

      readChar.writeValue(stepcount);
      // Serial.println("Stepcount printed to peripheral");

      /* if (writeChar.written()) {
        if (writeChar.value()) {

        readChar.writeValue(stepcount);
        Serial.println("Stepcount printed to peripheral");
          
        }
      } */

    }
  }
}
