/*
 O
 
 info:          http://monotaskers.com
 Author	 	Marco De Mutiis
 
 Date	        24/7/14
 Version	0
 
 This is a sample code to be uploaded for the o, a Monotaskers module made of a rotary encoder (push-button enabled) with a 24 led ring around it, 
 inspired by similar controller knob interfaces, like the arc and the duet.  
 In this example the lights will turn on according to the position of the rotary encoder, and will blink all at once if the encoder is pushed. If the monotasker
 is not touched for longer than 10 minutes it will start a light animation unti it is touched again.
 
 Requires ShiftOutX library by Juan Hernandez and the QuandEncoder library by Pedro Rodrigues.
 
 useful teaching applications:
 - Rotary encoder
 - Controlling leds
 - Pin extension with shift registers IC 74HC595
 - Debounce
 - Serial communication ( int )
 */


//#define ABSOLUTE //prints absolute position to serial
#define LIGHTPOS //prints light position (0-23) to serial

//#define PROCESSING //for Processing we will use Serial.write
#define OPENFRAMEWORKS //for openFrameworks we will use Serial.print

#include <QuadEncoder.h>
#include <SPI.h>
#include <ShiftOutX.h>
#include <ShiftPinNo.h>


////LED_stuff////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//_latchPin connected to ST_CP of 74HC595
//_dataPin connected to DS of 74HC595
//_clockPin connected to SH_CP of 74HC595
shiftOutX regOne(A2, A1, A3, MSBFIRST, 3);
uint64_t ledArray[24] = {
  shPin1, shPin2, shPin3, shPin4, shPin5, shPin6, shPin7, shPin8,
  shPin9, shPin10, shPin11, shPin12, shPin13, shPin14, shPin15, shPin16,
  shPin17, shPin18, shPin19, shPin20, shPin21, shPin22, shPin23, shPin24};

//Rotary Encoder stuff////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int qe1Move=0;
QuadEncoder qe(12,10);
int lightPos=0; //between 0 and 23
int lastReportedPos;
int absolutePos = 0;
int encoderSwitchPin = 11; //encoder push btn
int push, oldPush;

//general variables//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int frameCounter;
int ledCounter;
int indexLED;
bool reverse;

//nobody has touched the controller for more than 1 minute/////////////////////////////////////////////////////////////////////////////////////////////////
boolean noTouch = true;
long previousMillisBUTTON = 0;        // will store last time button was updated
long intervalBUTTON = 3500;           // interval at which to change random light

long previousMillisNOTOUCH = 0;        // will store last time timer was updated
long intervalNOTOUCH = 600000;          // interval at which to trigger the auto blinking [10 min = 10 x 60000 millis]


//SENDTOLED:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void sendToLed(int index){

  if(reverse == true){
    for (int i = 23; i > -1; i--){
      if (i >= index)regOne.pinOn(ledArray[i]);
      else regOne.pinOff(ledArray[i]);
    }
  }
  else{
    for (int i = 0; i < 24; i++){
      if (i <= index) regOne.pinOn(ledArray[i]);
      else  regOne.pinOff(ledArray[i]);
    }
  }
}

//LIGHTANIMATION:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void oneRing(int frameRate){
  if(frameCounter%frameRate == 0){
    ledCounter++;
    if(ledCounter>=24)  ledCounter=0;
    regOne.pinOn(ledArray[ledCounter]);

    if(ledCounter>2) regOne.pinOff(ledArray[ledCounter-3]);
    else regOne.pinOff(ledArray[ledCounter+21]);

  }
}

//DEBOUNCE  :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// debounce returns true if the switch in the given pin is closed and stable
bool debounce(int pin)
{
  boolean state;
  boolean previousState;
  previousState = digitalRead(pin); // store switch state
  for(int counter=0; counter < 1; counter++)
  {
    delay(1); // wait for 1 millisecond
    state = digitalRead(pin); // read the pin
    if( state != previousState)
    {
      counter = 0; // reset the counter if the state changes
      previousState = state; // and save the current state
    }
  }
  // here when the switch state has been stable longer than the debounce period
  return state;
}

//SETUP:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void setup(){

  regOne.allOff();                        //switch off all leds

  Serial.begin(9600);                      //init Serial communication
  digitalWrite(encoderSwitchPin, HIGH);   //internal pull up resistors for pushbutton reading


  frameCounter    =   0;
  ledCounter      =   -1;
  reverse         =   false;

  for(int i=0; i<24; i++){
    regOne.pinOn(ledArray[i]);
    delay(10);
    if(i>2) regOne.pinOff(ledArray[i-3]);
    else regOne.pinOff(ledArray[i+21]);
    delay(10);
  }

}
//LOOP:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void loop(){
  frameCounter++;

  //NOTOUCH::if the device is plugged in and noone touches it for more than one minute it starts randomly turning on and off buttons.
  if(noTouch){
    oneRing(25);


    unsigned long currentMillisBUTTON = millis();

    if(currentMillisBUTTON - previousMillisBUTTON > intervalBUTTON) {
      previousMillisBUTTON = currentMillisBUTTON; 
    }

  }
  unsigned long currentMillisNOTOUCH = millis();
  //NOTOUCH ends///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  frameCounter++;

  //BTN///////////////////////////////////////PUSHHHHH/////////////////////////////////////////////////////////////////////////////////
  push = digitalRead(encoderSwitchPin);
  if(debounce(encoderSwitchPin)==true){

    if(oldPush!=push){
      if(push == 0) {
        noTouch = false;
        lastReportedPos = lightPos-1;

#ifdef OPENFRAMEWORKS
        Serial.println("P");
#endif

#ifdef PROCESSING
        Serial.write("P");
#endif
        //light animation for push/////

        for(int looping=0; looping<2; looping++){

          for(int i=0; i<24; i++){
            regOne.pinOn(ledArray[i]);
            //delay(5);
          }
          delay(10);
          for(int i=0; i<24; i++){
            regOne.pinOff(ledArray[i]);
            //delay(5);
          }
          delay(10);
        }
        //light animation end///////////
      }
    }

    oldPush = push;

  }


  //ROTATION////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  qe1Move=qe.tick();

  if (qe1Move=='>'){
    lightPos++;
    if(lightPos>23) {
      lightPos=0;
      reverse = false;
    }
    absolutePos++;
#ifdef ABSOLUTE
#ifdef OPENFRAMEWORKS
    Serial.println(absolutePos);
#endif
#ifdef PROCESSING
    Serial.write(absolutePos);
#endif
#endif

#ifdef LIGHTPOS
#ifdef OPENFRAMEWORKS
    Serial.println(lightPos);
#endif
#ifdef PROCESSING
    Serial.write(lightPos);
#endif

#endif
  }

  else if (qe1Move=='<'){
    lightPos--;
    if(lightPos<0) {
      lightPos=23;
      reverse = true;
    }
    absolutePos--;
#ifdef ABSOLUTE
    Serial.write(absolutePos);
#endif
#ifdef LIGHTPOS
    Serial.write(lightPos);
#endif
  }


  //TO//LEDS////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (lastReportedPos != lightPos) {



    indexLED = lightPos;
    lastReportedPos = lightPos;
    sendToLed(indexLED);
    noTouch = false;
  }

  else{
    if(!noTouch){
      //Serial.println(currentMillisNOTOUCH - previousMillisNOTOUCH);
      if(currentMillisNOTOUCH - previousMillisNOTOUCH > intervalNOTOUCH) {
        previousMillisNOTOUCH = currentMillisNOTOUCH;
        noTouch=true; 
      }
    } 
  }
}

void oneRingIndexLED(int index){
  //  index = 12;
  if(frameCounter%50 == 0){
    ledCounter++;
    if(ledCounter+index>=24)  ledCounter=0;
    regOne.pinOn(ledArray[ledCounter+index]);
    //        
    if(ledCounter>2) regOne.pinOff(ledArray[ledCounter+index-3]);
    else regOne.pinOff(ledArray[ledCounter+21]);

  }

}





