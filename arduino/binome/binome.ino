/*
 Binome

 info:          http://monotaskers.com
 Author	 	Marco De Mutiis

 Date	        24/7/14
 Version	0

 This is a sample code to be uploaded for Binome, a Monotaskers module made of 4 silicon pusbutton, inspired by the Monome.
*/

#include <MIDI.h>
//#define MIDION
#define SERIALOUT

// Button Grounds
const int buttonMatrix1 = 13;
const int buttonMatrix2 = 4;
const int buttonMatrix3 = 8;
const int buttonMatrix4 = 6;
// LED Grounds
const int ledGnd1 = 12;
const int ledGnd2 = 2;//D3 assigned to 1 bit DAC cause we needed PWM (fritzing sketch shows conenction to D3, that's wrong)
const int ledGnd3 = 7;
const int ledGnd4 = 5;


int white[]  =   { 
  255, 255, 255 };
int grounds[4]={
  ledGnd1, ledGnd2, ledGnd3, ledGnd4};

int fourColors[4];
int blackOrWhite[2]={
  HIGH, LOW};

char serialOutMsgs[4]={
  '1', '2', '3', '4'};
char serialOutMsgsUp[4]={
  '5', '6', '7', '8'};
/*
//GARAGE BAND HIP HOP DRUM KIT or KONTAKT 5 + OldTapeDrum Library
 int MIDIOutMsgs[4]={ 
 42, 36, 40, 39 };
 */

/*
//REASON 
 int MIDIOutMsgs[4]={ 
 50, 60, 70, 80 };
 */


//KONTAKT ukulele Strum
int MIDIOutMsgs[4]={ 
  67, 60, 81, 65 };

int MIDIOutMsgsUP[4]={ 
  91, 88, 91, 89 };


//button array
int button[]             =       {  
  buttonMatrix1, buttonMatrix2, buttonMatrix3, buttonMatrix4 };
int btnPressed[]         =       {  
  LOW, LOW, LOW, LOW };

//debounce
int debounceDelay  =  20;

//nobody has touched the controller for more than 1 minute
boolean noTouch = true;
long previousMillisBUTTON = 0;        // will store last time button was updated
long intervalBUTTON = 3500;           // interval at which to change random light

long previousMillisNOTOUCH = 0;        // will store last time timer was updated
long intervalNOTOUCH = 600000;          // interval at which to trigger the auto blinking [10 min = 10 x 60000 millis]



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::SETUP
void setup() 
{ 
  // define button inputs
  pinMode(buttonMatrix1, INPUT);
  pinMode(buttonMatrix2, INPUT);
  pinMode(buttonMatrix3, INPUT);
  pinMode(buttonMatrix4, INPUT);

  // define Led outputs
  pinMode(ledGnd1, OUTPUT);
  pinMode(ledGnd2, OUTPUT);
  pinMode(ledGnd3, OUTPUT);
  pinMode(ledGnd4, OUTPUT);

  //blink leds 5 times
  for(int i=0; i<5;i++){
    for(int j =0; j<4;j++){
      digitalWrite(grounds[j], HIGH);
    } 
    delay(50);
    for(int j =0; j<4;j++){
      digitalWrite(grounds[j], LOW);
    }  
    delay(50);
  }

#ifdef MIDION
  // Launch MIDI with default options
  // (input channel is set to 4)
  MIDI.begin(4); 
  Serial.begin(115200);
#endif

#ifdef SERIALOUT
  Serial.begin(9600);
//  establishContact();
#endif

}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::LOOP
void loop() {

  buttonPress();

}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::BUTTONPRESS
void buttonPress(){
  //NOTOUCH::if the device is plugged in and noone touches it for more than one minute it starts randomly turning on and off buttons.
  if(noTouch){
    unsigned long currentMillisBUTTON = millis();

    if(currentMillisBUTTON - previousMillisBUTTON > intervalBUTTON) {
      previousMillisBUTTON = currentMillisBUTTON; 
      for(int j =0; j<4; j++){
        int rand = random(0,2);
        fourColors[j]=blackOrWhite[rand];
        #ifdef SERIALOUT
        Serial.write(serialOutMsgs[rand]);
        #endif
      }
      for(int j =0; j<4;j++){
        digitalWrite(grounds[j], fourColors[j]);
      }  
    }

  }

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::NOTOUCH
  unsigned long currentMillisNOTOUCH = millis();

  for(int i=0; i< sizeof(button)/sizeof(int);i++){

    if (digitalRead(button[i]) == 0 && btnPressed[i]==LOW){                   //if no button is pushed and was not previously pushed, 
      if(!noTouch){
        //Serial.println(currentMillisNOTOUCH - previousMillisNOTOUCH);
        if(currentMillisNOTOUCH - previousMillisNOTOUCH > intervalNOTOUCH) {
          previousMillisNOTOUCH = currentMillisNOTOUCH;
          noTouch=true; 
        }
        for(int j =0; j<4;j++){
          digitalWrite(grounds[j], LOW);
        }    
      }

    }

    else if(digitalRead(button[i]) == 0 && btnPressed[i]==HIGH){              //if a button is released after being pushed, we detect the button being pushed and released and we change the btn status and turn off the led

      btnPressed[i]=LOW;
#ifdef MIDION
      MIDI.sendNoteOff(MIDIOutMsgs[i],0,1);   // Stop the note 
      ////////ukulele up stroke on
      delay(1);
      MIDI.sendNoteOn(MIDIOutMsgsUP[i],127,1);  // Send a Note (pitch 42, velo 127 on channel 1)
      delay(1);
      MIDI.sendNoteOff(MIDIOutMsgsUP[i],0,1);   // Stop the note
      //////////
#endif
#ifdef SERIALOUT
      Serial.write(serialOutMsgsUp[i]);
#endif
    }

    else if(digitalRead(button[i]) == 1 && btnPressed[i]==LOW){              //if a button which was not previously pressed is pushed, the PressedDown state is recorded in the bool array and we trigger the note

      if (debounce(button[i])==true)
      {
        noTouch = false;
        previousMillisNOTOUCH = currentMillisNOTOUCH;
        //set buttonPressed color to white and the other three to dark       
        for(int j =0; j<4;j++){
          digitalWrite(grounds[j], LOW);
        }  
        digitalWrite(grounds[i], HIGH);

        btnPressed[i]=HIGH;

        //send serial or MIDI message
#ifdef MIDION

        MIDI.sendNoteOn(MIDIOutMsgs[i],127,1);  // Send a Note (pitch 42, velo 127 on channel 1)
#endif

#ifdef SERIALOUT
        Serial.write(serialOutMsgs[i]);
#endif


      }
    }

    else if(digitalRead(button[i]) == 1 && btnPressed[i]==HIGH){             //if a button which was previously pressed is still pushed, we leave the lights on

      for(int j =0; j<4;j++){
        digitalWrite(grounds[j], LOW);
      }  
      digitalWrite(grounds[i], HIGH);



    }
  }
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::DEBOUNCE
// debounce returns true if the switch in the given pin is closed and stable 
boolean debounce(int pin)
{
  boolean state;
  boolean previousState;
  previousState = digitalRead(pin); // store switch state 
  for(int counter=0; counter < debounceDelay; counter++)
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


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::DEBOUNCE
void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("1");   // send an initial string
    delay(300);
  }
}









