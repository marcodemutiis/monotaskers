/*
 ADSR
 
 info:          http://monotaskers.com
 Author	 	Marco De Mutiis
 
 Date	        24/7/14
 Version	0
 
 This is a sample code to be uploaded for the ADSR, a Monotaskers module made of 4 analog sliders+leds.
 
 This sketch requires FScale library - Floating Point Autoscale Function V0.1 Paul Badger 2007 (Modified from code by Greg Shakar) http://playground.arduino.cc/Main/Fscale
 
 
  useful teaching applications:
- Analog sensors
- Re-mapping values
- Smoothing
- Fading (PWM)
- Serial communication ( sending multiple values )
 
 
 HARDWARE SETUP:
 SLID1 1 > ARD GND
 SLID1 3 > ARD 5v
 SLID1 2 > ARD pin A1
 SLID1 B > ARD pin 6
 SLID1 E > ARD GND
 
 SLID2 1 > ARD GND
 SLID2 3 > ARD 5v
 SLID2 2 > ARD pin 5
 SLID2 B > ARD pin A3
 SLID2 E > ARD GND
 
 SLID3 1 > ARD GND
 SLID3 3 > ARD 5v
 SLID3 2 > ARD pin A0
 SLID3 B > ARD pin 9
 SLID3 E > ARD GND
 
 SLID4 1 > ARD GND
 SLID4 3 > ARD 5v
 SLID4 2 > ARD pin A2
 SLID4 B > ARD pin 10
 SLID4 E > ARD GND
 */

#include <math.h>

//slider input pins
const int slideIn[4] = {
  A1, A3, A0, A2};
//slider values for mapping
int slideInv[4];
//leds outputs
const int leds[4] = {
  6, 5, 9, 10};
//led values
int ledVal[4];

//smoothing
const int numReadings = 5;

int readings[4][numReadings];      // the readings from the analog input
int index[4] = {
  0, 0, 0, 0};                // the index of the current reading
int total[4] = {
  0, 0, 0, 0};                  // the running total
int average[4] = {
  0, 0, 0, 0};                // the average
//values for comparison
int averageOld[4];
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
  for(int i = 0; i<4; i++){
    pinMode(leds[i], OUTPUT);
    pinMode(slideIn[i], INPUT);
    digitalWrite(leds[i], LOW);
    for (int thisReading = 0; thisReading < numReadings; thisReading++)    readings[i][thisReading] = 0; //init the readings at 0
  }

  Serial.begin(9600);        //init Serial communication


}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop(){
  for(int i = 0; i < 4; i++){
    // subtract the last reading:
    total[i] = total[i] - readings[i][index[i]]; 
    //remap the slider value to digital range
    slideInv[i] = map(analogRead(slideIn[i]), 0, 1023, 255, 0);
    // store the remapped reading from the sensor:  
    readings[i][index[i]] = slideInv[i]; 
    // add the reading to the total:
    total[i]= total[i] + readings[i][index[i]];   
    // advance to the next position in the array:  
    index[i] = index[i] + 1;   
    // if we're at the end of the array wrap around to the beginning: 
    if (index[i] >= numReadings)    index[i] = 0;    
    // calculate the average:
    average[i] = total[i] / numReadings; 
    // send it to the computer as ASCII digits
    //   if( average[i] != averageOld[i]){
    //      Serial.print("slider ");   
   
    Serial.print(average[i]);
    Serial.print(" ");

    //remap slider value to fscale value for led
    ledVal[i] = fscale( 0, 1023, 0, 255, slideInv[i], -1.5);
    analogWrite(leds[i], ledVal[i]);
  }
  Serial.println(); //linefeed break will be used on receiving end to determine that the information is complete.
}

//FSCALE------------------------------------------------------------------------------------------------------------------------

float fscale( float originalMin, float originalMax, float newBegin, float
newEnd, float inputValue, float curve){

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;


  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output 
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  /*
   Serial.println(curve * 100, DEC);   // multply by 100 to preserve resolution  
   Serial.println(); 
   */

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin){ 
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd; 
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float

  /*
  Serial.print(OriginalRange, DEC);  
   Serial.print("   ");  
   Serial.print(NewRange, DEC);  
   Serial.print("   ");  
   Serial.println(zeroRefCurVal, DEC);  
   Serial.println();  
   */

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine 
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  {   
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange); 
  }

  return rangedValue;
}




