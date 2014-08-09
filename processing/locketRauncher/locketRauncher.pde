 /*
  *
  * This sketch demonstrates how read values from a locket  
  * rauncher monotasker in Processing and trigger play/pause
  * of 4 audio files, and 4 different waves (1 triangle wave
  * and 3 sine waves) with the minim minimlibrary.
  * for more info visit monotaskers.com.
  *
  */
  
import processing.serial.*;
import controlP5.*;
import ddf.minim.*;
import ddf.minim.*;
import ddf.minim.ugens.*;

Minim minim;
AudioPlayer [] player;
AudioOutput out, out1, out2, out3;
Oscil fm, sineWave, sineWave2, sineWave3;

ControlP5 cp5;
DropdownList d1;

PFont f;

char val;

Serial myPort;
String portName = Serial.list()[0];
boolean myPortIsOpen = false;

char [] toggleON = {
  '1', '2', '3', '4'
};
char [] toggleOFF = {
  'w', 'x', 'y', 'z'
};
char [] toggleTempON = {
  'a', 'b', 'c', 'd'
};
boolean [] turnON = { 
  false, false, false, false
};
boolean [] turnONTEMP = { 
  false, false, false, false
};
int [] wavePos = {350, 450, 550, 650};
int wavePos0 = 350;
int wavePos1 = 450;
int wavePos2 = 550;
int wavePos3 = 650;


//--setup-----------------------------------------------------------------------------------------------------setup()
void setup() {
  size(260, 750, P3D);
  smooth();

  //println(Serial.list());

  //MINIM///////////////////////////////////////////////////////
  minim = new Minim(this);

  //play music files if toggles ON
  //load music files
  player = new AudioPlayer[4];
  player[0] = minim.loadFile("sound0.wav");
  player[1] = minim.loadFile("sound1.wav");
  player[2] = minim.loadFile("sound2.wav");
  player[3] = minim.loadFile("sound3.wav");

  //play waves if toggles (ON)
  //square wave and sine waves 
  out   = minim.getLineOut();
  out1   = minim.getLineOut();
  out2   = minim.getLineOut();
  out3   = minim.getLineOut();

  Oscil wave = new Oscil( 200, 0.2, Waves.TRIANGLE );
  fm   = new Oscil( 10, 2, Waves.SINE ); 
  fm.offset.setLastValue( 0 );
  fm.patch( wave.frequency );
  wave.patch( out );

  sineWave = new Oscil( 0, 0.3f, Waves.SINE );
  sineWave.patch( out1 );

  sineWave2 = new Oscil( 0, 0.3f, Waves.SINE );
  sineWave2.patch( out2 );

  sineWave3 = new Oscil( 0, 0.3f, Waves.SINE );
  sineWave3.patch( out3 );

  //SERIAL PORT DROPDOWN MENU//////////////////////////////////
  cp5 = new ControlP5(this);

  d1 = cp5.addDropdownList("select SERIAL PORT") //add dropdown menu element 
      .setPosition(30, 180)                        //and set position and size
      .setSize(200, Serial.list().length*35)
        ;
  customize(d1); // customize the list

  //FONT/////////////////////////////////////////////////////////
  f = createFont("Infinity", 24);
  textFont(f);
}

//--draw-----------------------------------------------------------------------------------------------------draw()
void draw() {
  background(100);//(128, 0, 54);

  //logos//
  fill(255);
  textAlign(CENTER);
  for (int i =0; i<2; i++) {   
    text("m", (width * 0.5), 40);
    text("monotaskers", (width * 0.5), 80); //we write it twice for better visibility
    text("locket rauncher", (width * 0.5), 130);
  }
  noFill();
  stroke(255);
  rect(width * 0.5-15, 40-22, 30, 30);
  rect(30, 102, 200, 40);

  noStroke();


  if (myPortIsOpen) {                    //only execute the rest of the loop if I have selected a port and established a connection
    if (myPort.available() > 0) {        // If data is available,
      int valInt = myPort.read();        // read it and store it in val
      val = char(valInt);                //convert to char
      println(val);
    }

    for (int i = 0; i< 4; i++) {
      stroke(255);
      ellipse(55+50*i, 230, 40, 40);

      //TOGGLES ON state ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
      if (val == toggleON[i]) {
        turnON[i] = true;
      }

      //TOGGLES OFF state ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
      if (val == toggleOFF[i]) {
        turnON[i] = false;
        turnONTEMP[i] = false;
      }

      //TOGGLES (ON) state ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
      if (val == toggleTempON[i]) {
        //turnON[i] = !turnON[i];
        turnONTEMP[i] = true;
      }
    }
    ctrlON(); //visualization of toggle switch states
    play();   //controls the sound playback >> play a file if toggle is ON / silent if OFF / create sound wave if (ON)
  }
}

//--ctrlON-----------------------------------------------------------------------------------------------------ctrlON()
void ctrlON() {
  for (int i =0; i<4; i++) {                 //ON
    if (turnON[i]) {
      //draw rectangle white if ON
      fill(255, 255, 255);
      ellipse(55+50*i, 230, 30, 30);
      stroke(255); noFill();
      ellipse(55+50*i, 230, 40, 40);
      //draw black text "ON" on rectangle
      //      fill(60);
      //      text("ON", 35+50*i+20, 228);
    }
    else if (!turnON[i] && !turnONTEMP[i]) { //OFF 
      //draw rectangle dark grey if OFF
      fill(100);
      ellipse(55+50*i, 230, 30, 30);
      stroke(255); noFill();
      ellipse(55+50*i, 230, 40, 40);
    }
    else if (turnONTEMP[i]) {                //(ON)
      //draw rectangle light grey if (ON)
      fill(128);
      ellipse(55+50*i, 230, 30, 30);
      stroke(255); noFill();
      ellipse(55+50*i, 230, 40, 40);
    }
  }
}

//--play-----------------------------------------------------------------------------------------------------play()
void play() {
  for (int j =0; j<4; j++) {
    ///////ON///////////////////////////////
    if(turnON[j]){
       // loop if file has reached end
      if (player[j].position() >= player[j].length()) player[j].cue(0);
      // play the file
      player[j].play();
      //draw the waveform
      stroke(255);
      for (int i = 0; i < player[j].bufferSize() - 1; i++)
      {
        float x1 = map( i, 0, player[j].bufferSize(), 0, width );
        float x2 = map( i+1, 0, player[j].bufferSize(), 0, width );
        line( x1, wavePos[j] + player[j].left.get(i)*50, x2, wavePos[j] + player[j].left.get(i+1)*50 );
        line( x1, wavePos[j] + player[j].right.get(i)*50, x2, wavePos[j] + player[j].right.get(i+1)*50 );
      }
    }
    

    ///////OFF///////////////////////////////
    
    if (!turnON[j]) { 
      //pause the player
      player[j].pause();
      
    }
    if (!turnONTEMP[0]) { 
      //kill the wave
      fm.offset.setLastValue( 0 );
      float modulateAmount = 0;
      float modulateFrequency = 0;

      fm.frequency.setLastValue( modulateFrequency );
      fm.amplitude.setLastValue( modulateAmount );
    }
    if (!turnONTEMP[1]) { 
      sineWave.frequency.setLastValue(0);
    }
    if (!turnONTEMP[2]) { 
      sineWave2.frequency.setLastValue(0);
    }
    if (!turnONTEMP[3]) { 
      sineWave3.frequency.setLastValue(0);
    }
    ///////////////(ON)////////////////////////
    if (turnONTEMP[0]) { 
      //set wave frequency to whatever audible Hz
      fm.offset.setLastValue( 200 );
      float modulateAmount = 110;
      float modulateFrequency = 50;

      fm.frequency.setLastValue( modulateFrequency );
      fm.amplitude.setLastValue( modulateAmount );
      // draw the waveforms
      stroke( 255 );
      for ( int i = 0; i < out.bufferSize() - 1; i++ )
      {
        // find the x position of each buffer value
        float x1  =  map( i, 0, out.bufferSize(), 0, width );
        float x2  =  map( i+1, 0, out.bufferSize(), 0, width );
        // draw a line from one buffer position to the next for both channels
        line( x1, wavePos0 + out.left.get(i)*50, x2, wavePos0 + out.left.get(i+1)*50);
        line( x1, wavePos0 + out.right.get(i)*50, x2, wavePos0 + out.right.get(i+1)*50);
      }
    }
    if (turnONTEMP[1]) { 
      sineWave.frequency.setLastValue(440);

      stroke( 255 );
      for ( int i = 0; i < out1.bufferSize() - 1; i++ )
      {
        float x1  =  map( i, 0, out1.bufferSize(), 0, width );
        float x2  =  map( i+1, 0, out1.bufferSize(), 0, width );
        line( x1, wavePos1 + out1.left.get(i)*50, x2, wavePos1 + out1.left.get(i+1)*50);
        line( x1, wavePos1 + out1.right.get(i)*50, x2, wavePos1 + out1.right.get(i+1)*50);
      }
    }
    if (turnONTEMP[2]) {
      sineWave2.frequency.setLastValue(600);

      stroke( 255 );
      for ( int i = 0; i < out2.bufferSize() - 1; i++ )
      {
        float x1  =  map( i, 0, out2.bufferSize(), 0, width );
        float x2  =  map( i+1, 0, out2.bufferSize(), 0, width );
        line( x1, wavePos2 + out2.left.get(i)*50, x2, wavePos2 + out2.left.get(i+1)*50);
        line( x1, wavePos2 + out2.right.get(i)*50, x2, wavePos2 + out2.right.get(i+1)*50);
      }
    }
    if (turnONTEMP[3]) {
      sineWave3.frequency.setLastValue(330);

      stroke( 255 );
      for ( int i = 0; i < out3.bufferSize() - 1; i++ )
      {
        float x1  =  map( i, 0, out3.bufferSize(), 0, width );
        float x2  =  map( i+1, 0, out3.bufferSize(), 0, width );
        line( x1, wavePos3 + out3.left.get(i)*50, x2, wavePos3 + out3.left.get(i+1)*50);
        line( x1, wavePos3 + out3.right.get(i)*50, x2, wavePos3 + out3.right.get(i+1)*50);
      }
    }
  }
}

