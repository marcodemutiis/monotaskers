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







