// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

// Note:Ademi: I made some changes to the code in Order to accomodate our needs

#include <Wire.h>
#define SLAVE_ADDRESS 0x0c    // for somereason arduino master code shifts the PIC slave address one bit to the right "not sure if wether the bug in PIC's protocol or Arduino's
void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  delay(15);
}

void loop() {
  Wire.beginTransmission(SLAVE_ADDRESS);
  
  Wire.requestFrom(SLAVE_ADDRESS, 11);    // request 11 bytes from slave device for the Message "Hello World"
  char counter =0;
  while (Wire.available()) { // as long as characters are sent
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }
    Serial.println(""); // print a new line
    delay(500);         // wait before initiating the next request
}
