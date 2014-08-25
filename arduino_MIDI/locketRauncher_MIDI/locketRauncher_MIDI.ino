/*
 Locket Rauncher
 
 info:          http://monotaskers.com
 Author	 	Marco De Mutiis
 
 Date	        24/7/14
 Version	0
 
 This is a sample code to be uploaded for the Locket Rauncher, a Monotaskers module made of 4 ON-OFF-(ON) toggles and 4 leds, inspired by rocket launching interfaces and world control panels! ;]  
 The nice thing about this simple interface is the possibility to perform a momentary (ON) state by pushing down the toggle, and a static ON if the toggle is pushed upward once. 
 This way 4 toggles provide 8 different inputs and can be mapped to different led output behaviours. In this example code the corresponding led will blink at a 100 ms rate when pressing the (ON) toggle, 
 and will turn on steadily if the toggle is switched ON.
 
 for toggle switch info see datasheet: http://www.jprelec.co.uk/pdffiles/800-247-245.pdf
 
 useful teaching applications:
- PWM modulation (led fading)
- Debounce without delay
- Blinking without delay
- Serial communication ( chars )
- ON-OFF-(ON) toggles
 */

void noteOn(byte channel, byte pitch, byte velocity) {
  MIDIEvent noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MIDIUSB.write(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  MIDIEvent noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MIDIUSB.write(noteOff);
}
int channel = 0; 


#define tot 4

int togglesSTATIC[tot] = { //pins connected to the ON toggle connectors
  4, A1, 11, A2  };
int togglesTEMP[tot] = { //pins connected to the (ON) toggle connectors
  7, A0, 12, 8  };
int leds[tot] = { //leds
  5, 6, 9, 10 };


int midiNoteA[4]   =   {
  64, 65, 66, 67
};
int midiNoteB[4]   =   {
  36, 37, 38, 39
};


char serialMsgsSTATIC[tot] = { //serial messages sent when ON toggle is pushed
  '1', '2', '3', '4'};
char serialMsgsTEMP[tot] = { //serial messages sent when (ON) toggle is pushed
  'a', 'b', 'c', 'd'};  

char serialMsgsOFF[tot] = { //serial messages sent when (ON) toggle is pushed
  'w', 'x', 'y', 'z'};  

//variable to store the readings of our toggle states
int toggleTEMPVal[tot];
int toggleTEMPOldVal[tot];
int toggleSTATICVal[tot];
int toggleSTATICOldVal[tot];

//blinking without delay
boolean isBlinking[tot];
long previousMillis = 0;        // will store last time LED was updated
long interval = 100;           // interval at which to blink (milliseconds)
int ledState = LOW;

//if nobody has touched the controller for more than 10 minute start an led fading animation
boolean noTouch = true;
long previousMillisBUTTON = 0;        // will store last time button was updated
long intervalBUTTON = 3500;           // interval at which to change random light
long previousMillisNOTOUCH = 0;        // will store last time timer was updated
long intervalNOTOUCH = 600000;          // interval at which to trigger the auto blinking [10 min = 10 x 60000 millis]

//fading leds (all our input are PWM enabled, so we can fade if we want)
int brightness[4];    // how bright the LED is
int fadeAmount = 1;    // how many points to fade the LED by
int fadingLed = 0;

//debounce
int lastButtonState[tot];// the previous reading from the input pin
long lastDebounceTime[tot];  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
int buttonState[tot];             // the current reading from the input pin



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::SETUP
void setup(){
  //initialize inputs and outputs
  for(int i =0; i < tot; i++){

    pinMode(togglesTEMP[i], INPUT);
    pinMode(togglesSTATIC[i], INPUT);
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);

    toggleSTATICVal[i] = 1; 
    toggleSTATICOldVal[i] = 1;
    toggleTEMPVal[i] = 1; 
    toggleTEMPOldVal[i] = 1;
  }
  //initialize Serial at 9600 baud rate
  Serial.begin(9600); 

}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::LOOP
void loop(){

  //NOTOUCH if no one has touched the controller yet, or noone has touched it for more than 10 minutes start a pulse animation
  if(noTouch){
    for(int i =0; i < 4; i++){
      analogWrite(leds[i], brightness[i]);
      // change the brightness for next time through the loop:
      brightness[i] = brightness[i] + fadeAmount;
      // reverse the direction of the fading at the ends of the fade: 
      if (brightness[i] == 0 || brightness[i] == 255) {
        fadeAmount = -fadeAmount ; 
      }  
      // wait for 1 millisecond to see the dimming effect    
      delay(1); 
    }
  }

  //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::NOTOUCH
  unsigned long currentMillisNOTOUCH = millis();


  for(int i =0; i < tot; i++){

    toggleSTATICVal[i] = digitalRead(togglesSTATIC[i]);       //read toggle ON states

    //toggle ON state - static toggle mode//////////////////////////////////////////////////////////////////////////////////////////////////////////     
   
    if(debounce(i)){ //debounce to cancel noise in the readings. ( based on  Limor Fried's debounce without delay example http://arduino.cc/en/Tutorial/Debounce )
      
      if(toggleSTATICVal[i] != toggleSTATICOldVal[i]){       //if the toggle was switched

        if(toggleSTATICVal[i] == 0){                         //if it was switched ON (0 = ON because we use pull-up resistors)
          if(noTouch) {
            noTouch = false;
            previousMillisNOTOUCH = currentMillisNOTOUCH;
            for(int j =0; j < 4; j++){
              digitalWrite(leds[j], LOW);
              brightness[j]=0;
            }
          }

          digitalWrite(leds[i], HIGH);                       //turn according led on
          noteOn(channel, midiNoteA[i], 127);
         // Serial.println(serialMsgsSTATIC[i]);               //write according message to Serial
        }

        else  {                                              //if it was switched OFF
          if(noTouch) {
            noTouch = false;
            previousMillisNOTOUCH = currentMillisNOTOUCH;
            for(int j =0; j < 4; j++){
              digitalWrite(leds[j], LOW);
              brightness[j]=0;
            }
          }
          digitalWrite(leds[i], LOW);                        //turn according led off               
          //Serial.println(serialMsgsOFF[i]);               //write according message to Serial
          
          noteOff(channel, midiNoteA[i], 0);

        }   

        /*      Serial.print("static toggle ");
         Serial.print(i);
         Serial.print(" = ");
         Serial.println(toggleSTATICVal[i]);*/
      }
      else{

        if(!noTouch){ //if more than 10 mins pass than activate the pulse animation
          if(currentMillisNOTOUCH - previousMillisNOTOUCH > intervalNOTOUCH) {
            previousMillisNOTOUCH = currentMillisNOTOUCH;
            noTouch=true; 
          } 
        }
      }

      toggleSTATICOldVal[i] = toggleSTATICVal[i];            //update old val to allow comparison on the next loop iteration

    } //debounce ends here


    //toggle (ON) state - momentary switch mode///////////////////////////////////////////////////////////////////////////////////////////////////////
    toggleTEMPVal[i] = digitalRead(togglesTEMP[i]);          //read the toggle (ON) states
    if(toggleTEMPVal[i] != toggleTEMPOldVal[i]){       //if the toggle was switched
      if(toggleTEMPVal[i] == 0){                               //if it was switched ON
        if(isBlinking[i] == false) isBlinking[i] = true;       //change boolean for the button to true (it means we are pushing the button down)

        if(noTouch) {
          noTouch = false;
          previousMillisNOTOUCH = currentMillisNOTOUCH;
          for(int j =0; j < 4; j++){
            digitalWrite(leds[j], LOW);
            brightness[j]=0;
          }
        }
      }
      else if(toggleTEMPVal[i] == 1 && toggleSTATICVal[i] == 1){//if it was switched OFF
        isBlinking[i] = false;
        digitalWrite(leds[i], LOW);                              //turn according led off 
        //Serial.println(serialMsgsOFF[i]);               //write according message to Serial
        noteOff(channel, midiNoteB[i], 0);
      }
    }
    toggleTEMPOldVal[i] = toggleTEMPVal[i];            //update old val to allow comparison on the next loop iteration
  }

  blinking();                                                  //call blinking function


}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::BLINKING
void blinking(){
  unsigned long currentMillis = millis();



  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    for(int i =0; i<tot; i++){
      // set the LED with the ledState of the variable:
      if(isBlinking[i] == true)  {
        digitalWrite(leds[i], ledState);                       //turn on the blinking lights for all the toggles which are momentarily pressed - (ON) states
        //Serial.println(serialMsgsTEMP[i]);                     //write according message to Serial
        noteOn(channel, midiNoteB[i], 127);
      }
    }
  }

}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::DEBOUNCE
boolean debounce(int i){
  int reading = digitalRead(togglesSTATIC[i]);

  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState[i]) {
    // reset the debouncing timer
    lastDebounceTime[i] = millis();
  } 

  if ((millis() - lastDebounceTime[i]) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState[i]) {
      buttonState[i] = reading;

      // only toggle the LED if the new button state is HIGH
      //  if (buttonState == HIGH) {
      return true;
        //}
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState[i] = reading;


}








