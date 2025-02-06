/*
  File: Gimbal_project
  Auther: Sebatian Loe
  Date:  2025-01-30
  Description: Program that stabilizes an objects with the help of servos (Gimbal).
*/

// Include libraries
#include "Wire.h"  
#include <Servo.h> 

//construct servo objects
Servo servo1;
Servo servo2;

//Initialize global variables
int potens = A1;
int potsvaule = 0;
int value;
float X_cords, Y_cords, Z_cords;
const int length = 7;  
int ADXL345 = 0x53;

//Initialize global arrays
float lastreadingsX[length] = { 0 };
float lastreadingsY[length] = { 0 };.


void setup() {
  //Initialize communications
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(ADXL345);
  Wire.write(0x2D);
  Wire.write(8);
  Wire.endTransmission();

  //Sets pins 8 and 9 as output for the servos 
  servo1.attach(8);
  servo2.attach(9);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT); 
}

void loop() {
  //Reads the value of potentiometers and maps the value over to a smaller span
  value = analogRead(potens);
  potsvaule = map(value, 0, 1021, -90, 90);

  //Begins communication with the ADXL345 and requests two bytes for each axis.
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32);
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true);

  //Reads the values for x, y and z cordinates of the accelorometer.  
  X_cords = (Wire.read() | Wire.read() << 8) / 256.0;
  Y_cords = (Wire.read() | Wire.read() << 8) / 256.0;
  Z_cords = (Wire.read() | Wire.read() << 8) / 256.0;

  //Calculates the angle and converts the value from radians to degrees
  float pitch_deg = atan2(Y_cords, Z_cords) * 180 / PI;  
  float roll_deg = atan2(X_cords, Z_cords) * 180 / PI;

  //Uses the updMean function to smooth out the values that get put in the servos 
  float clean_pitch = updMean(pitch_deg, lastreadingsX, length);
  float clean_roll = updMean(roll_deg, lastreadingsY, length) + potsvaule; 

  //Maps the value over to larger area and reverses the values 
  float counter_pitch = map(clean_pitch, -90, 91, 0, 180);
  float counter_roll = map(clean_roll, -90, 91, 180, 0);

  //Writes the value at the servo 
  servo1.write(counter_pitch);
  servo2.write(counter_roll);
}fd

/*
  Function: updMean
  Calucates the latest average of the latest x values.
  It shifts the array to the left and add a new value and returns median.

  Parameters 
    - Value: The input values  
    - Readings[]: Uses the array to store x values
    - Leng: The amount of saved values 

  Returns: The median of the latest x values  
*/
float updMean(float value, float readings[], int leng) { //
  float sum;

  for (int i = 0; i < leng- 1; i++) {
    readings[i] = readings[i + 1];
  }
  readings[leng - 1] = value;

  for (int i = 0; i < leng; i++) {
    sum += readings[i];
  }
  return sum / leng;
}