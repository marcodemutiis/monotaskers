/*
 Sequitur

 info:          http://monotaskers.com
 Author	 	Marco De Mutiis

 Date	        24/7/14
 Version	0

 This is a sample code to be uploaded for Sequitur, a Monotaskers module made of 8 push-buttons+leds, inspired by the legendary Roland 808 drum machine.  
 In this example code the 8 buttons are turned into a drum-machine-like interface. You can add/delete an instrument step by clicking on the button when the led is blinking at the right time.
 You can reset the drum machine by long-pressing the most left button on the bottom row. 

*/

//#define MIDION
#define SERIALON

#ifdef MIDION
#include <MIDI.h>
#endif

//TODO
/*
-add top left btn double click to increase the tempo
 -add bottom right btn double click to decrease the tempo
bottom right btn long press to reset the drum machine +blink all LEDS
 */

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











