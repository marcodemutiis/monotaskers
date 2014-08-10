/*
 Sequitur

 info:          http://monotaskers.com
 Author	 	Marco De Mutiis

 Date	        24/7/14
 Version	0

 This is a sample code to be uploaded for Sequitur, a Monotaskers module made of 8 push-buttons+leds, inspired by the legendary Roland 808 drum machine.  
 In this example code the 8 buttons are turned into a drum-machine-like interface. You can add/delete an instrument step by clicking on the button when the led is blinking at the right time.
 You can reset the drum machine by long-pressing the most left button on the bottom row. 

 This sketc requires the MIDI library (optional) / use with Hairless Midi software on receiving computer.
 
 useful teaching applications:
- Push button single press and long press
- MIDI
- Multi dimensional arrays
- Serial communication ( int )
*/

//#define MIDION
#define SERIALON

#ifdef MIDION
#include <MIDI.h>
#endif

int numStep = 8;    // number of steps
int numInst = 8;    // number of instruments

//drum machine arrays - 8 steps / 8 instruments / on/off value
boolean drummachineSteps[8][8];

//8 instruments (random MIDI note numbers for now)
int drummachineInstr[8]   =   { 
  36, 37, 48, 39, 40, 41, 42, 43};

//on-off value
boolean drummachineBool = 0;

//button and led pins definition
#define btns 8
int pushBtn[btns] = { 
  8, 6, 4, 2, A0, A2, 12, 10 };
int ledPin[btns] = { 
  9, 7, 5, 3, 13, A1, A3, 11};

int btnVal[btns];     //store button value
int btnValOld[btns];  //store previous button value for comparison
int ledPinCount=-1;      //counter to move led sequence across the buttons
int b[btns];       //for checking button click/dbl clk/hold/long hold


//timer stuff
long previousMillis = 0;     
long interval = 300; 


//setup::::::::::::::::::::::::::::::::::::::::::::::::
void setup(){

#ifdef MIDION
  MIDI.begin();                    //initialize MIDI communication
  Serial.begin(115200);
#endif

  Serial.begin(9600);            //initialize Serial communication

  for(int i = 0; i < btns; i++){
    pinMode(pushBtn[i], INPUT);    //initialize buttons as inputs
    pinMode(ledPin[i], OUTPUT);    //initialize leds as outputs
  }


  //fill our array with values
  for(int i = 0; i < numStep; i++){
    for(int j = 0; j< numInst; j++){
      drummachineSteps[i][j] = false; //all instruments at every steps are turned to 0
    }
  }

  for(int loops = 0; loops<5; loops++){
    for(int led = 0; led<btns;led++){
      digitalWrite(ledPin[led], HIGH);
    }
    delay(50);
    for(int led = 0; led<btns;led++){
      digitalWrite(ledPin[led], LOW);
    }
    delay(50);
  }

#ifdef SERIALON
  //checkArrayContent();             //just for printing out values
#endif

}


//loop::::::::::::::::::::::::::::::::::::::::::::::::
void loop(){
//check for button clicks
  for(int i =0; i<btns; i++){
    b[i]=0;
    b[i] = checkButton(i);
   
    if (b[i] == 1) clickEvent(i);
    if (b[i] == 3) holdEvent(i);
  }
  
 //timer - gets executed every interval (has be converted from millis to bpm later) 

  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis; 
    LedPinOn();    //move Led to next step
  }
 
}


void LedPinOn(){
  //stop MIDI note from previous step
#ifdef MIDION
  for(int i = 0; i < btns; i++){
    if (drummachineSteps[ledPinCount][i] == true)
    {
      MIDI.sendNoteOff(drummachineInstr[i],0,1);
    }
  }
#endif

  digitalWrite( ledPin[ledPinCount], LOW);      //turn off led from previous step
  if(ledPinCount<7)ledPinCount++;               //next step
  else ledPinCount = 0;
  digitalWrite( ledPin[ledPinCount], HIGH );    //turn on led for current step
  #ifdef SERIALON
  char ledChar[8] = { 'a', 'b', 'c', 'd', 'e', 'f','g', 'h'};
  Serial.write(ledChar[ledPinCount]);          //send a char everytime we turn an led on 
  #endif

  //play MIDI note for current step
  for(int i = 0; i < btns; i++){
    if (drummachineSteps[ledPinCount][i] == true)
    {
#ifdef MIDION
      MIDI.sendNoteOn(drummachineInstr[i],127,1);
#endif

#ifdef SERIALON
      Serial.println(i, DEC);
#endif
    }
  }
}

void checkArrayContent(){

  Serial.println( "::::: SEQUENCER ( x ) / SOUND MATRIX ( y ) ::::::::::::::::" );
  Serial.println( "        | 0 |  | 1 |  | 2 |  | 3 |  | 4 |  | 5 |  | 6 |  | 7 |" );

  // for each instrument 
  for (int i = 0; i < numInst; i++) {
    Serial.print("MIDI "); 
    Serial.print( drummachineInstr[ i ]); 
    Serial.print(" | ");


    // for each sequencer step
    for (int j = 0; j< numStep; j++) {

      if(  drummachineSteps[j][i] ) Serial.print("X      ");
      else Serial.print("0      ");    

    }
    Serial.println();
  }
  Serial.println("*********************************************************************************************");
}


//btnFunctions/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////btnFunctions
// Button timing variables
int debounce = 1; // ms debounce period to prevent flickering when pressing or releasing the button
int DCgap = 250; // max ms between clicks for a double click event
int holdTime = 1500; // ms hold period: how long to wait for press+hold event
int longHoldTime = 5000; // ms long hold period: how long to wait for press+hold event

// Other button variables
boolean buttonVal = HIGH; // value read from button
boolean buttonLast = HIGH; // buffered value of the button's previous state
boolean DCwaiting = false; // whether we're waiting for a double click (down)
boolean DConUp = false; // whether to register a double click on next release, or whether to wait and click
boolean singleOK = true; // whether it's OK to do a single click
long downTime = -1; // time the button was pressed down
long upTime = -1; // time the button was released
boolean ignoreUp = false; // whether to ignore the button release because the click+hold was triggered
boolean waitForUp = false; // when held, whether to wait for the up event
boolean holdEventPast = false; // whether or not the hold event happened already
boolean longHoldEventPast = false;// whether or not the long hold event happened already


int checkButton(int i)
{ 
  int event = 0;
  // Read the state of the button
  btnVal[i] = digitalRead(pushBtn[i]);

  // Button pressed down
  if (btnVal[i] == HIGH && btnValOld[i] == LOW && (millis() - upTime) > debounce) {

    digitalWrite(ledPin[i],HIGH);

    downTime = millis();
    ignoreUp = false;
    waitForUp = false;
    singleOK = true;
    holdEventPast = false;
    longHoldEventPast = false;
    if ((millis()-upTime) < DCgap && DConUp == false && DCwaiting == true) DConUp = true;
    else DConUp = false;
    DCwaiting = false;

  }

  // Button released
  else if (btnVal[i] == LOW && btnValOld[i] == HIGH && (millis() - downTime) > debounce) { 

    digitalWrite(ledPin[i],LOW);

    if (not ignoreUp) {
      upTime = millis();
      if (DConUp == false) DCwaiting = true;
      else {
        event = 2;
        DConUp = false;
        DCwaiting = false;
        singleOK = false;
      }
    }
  }

  // Test for normal click event: DCgap expired
  if ( btnVal[i] == LOW && (millis()-upTime) < DCgap && DCwaiting == true && DConUp == false && singleOK == true) {//instead of >=DCgap
    event = 1;
    DCwaiting = false;
  }

  // Test for hold
  if (btnVal[i] == HIGH && (millis() - downTime) >= holdTime) {
    // Trigger "normal" hold
    if (not holdEventPast) {
      event = 3;
      waitForUp = true;
      ignoreUp = true;
      DConUp = false;
      DCwaiting = false;
      //downTime = millis();
      holdEventPast = true;
    }
    // Trigger "long" hold
    if ((millis() - downTime) >= longHoldTime) {
      if (not longHoldEventPast) {
        event = 4;
        longHoldEventPast = true;
      }
    }
  }
  btnValOld[i] = btnVal[i];
  return event;

}



void clickEvent(int i) {
#ifdef SERIALON//write to Serial
  //Serial.print("button #"); 
 // Serial.print(i); 
  //Serial.println(" was clicked");
#endif

  //assign instrument+step to the drummachine array////////////////////////////////////////////////////
  //drummachineSteps[ledPinCount][i] = true;

  //suggestion by philip>>
  // or, if you just want to change the state on/off every time you press: ON -> OFF, OFF -> ON
  drummachineSteps[ledPinCount][i] = !drummachineSteps[ledPinCount][i];

  //assign//end///////////////////////////////////////////////////////////////////////////////////////  

}


void holdEvent(int i) {
  //reset drummachine if last button is hold for >2secs
  if (i= 7){
#ifdef SERIALON//write to Serial
    //Serial.print("drummachine reset"); 
    Serial.write('R'); //send an R if we reset
#endif
    for(int i = 0; i < numStep; i++){
      
      #ifdef MIDION//turn off every note that could be left on
      MIDI.sendNoteOff(drummachineInstr[i],0,1); 
      #endif
      
      for(int j = 0; j< numInst; j++){
        drummachineSteps[i][j] = false; //all instruments at every steps are turned to 0
      }
    }
  }

  for(int loops = 0; loops<3; loops++){
    for(int led = 0; led<btns;led++){
      digitalWrite(ledPin[led], HIGH);
    }
    delay(50);
    for(int led = 0; led<btns;led++){
      digitalWrite(ledPin[led], LOW);
    }
    delay(50);
  }

}










