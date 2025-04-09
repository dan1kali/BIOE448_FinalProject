#include <Wire.h> // Necessary for I2C communication
#include <LiquidCrystal.h>

int accel = 0x53; // I2C address for this sensor (from data sheet)
float x, y, z, accvector;
int threshold = 40000;   // Threshold initialized to 40000
int stepcount = 0;       // Step count initialized to 0

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2); //Initiate the LCD in a 16x2 configuration
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
  // x = (x/256)*9.8;
  y = (Wire.read() | Wire.read() << 8); // Parse y values
  // y = (y/256)*9.8;
  z = (Wire.read() | Wire.read() << 8); // Parse z values
  // z = (z/256)*9.8;
  accvector = sqrt(x*x + y*y+ z*z);

  // Serial.print("x = "); // Print values
  // Serial.print(x);
  // Serial.print(", y = ");
  // Serial.print(y);
  // Serial.print(", z = ");
  // Serial.print(z);
  
  
  //Serial.println(accvector);

  if (accvector > threshold)
    stepcount++;
    Serial.println(stepcount);

  lcd.print("Step Count:")
  lcd.setCursor(0, 1);
  lcd.print(stepcount);

  delay(200);
}