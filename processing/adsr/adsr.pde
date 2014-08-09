 /*
  *
  * This sketch demonstrates how read values from a adsr 
  * monotasker in Processing and adjust volume of 4 files  
  * playing through the minim library.
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

ControlP5 cp5;
DropdownList d1;

PFont f;

Serial myPort;
String portName = Serial.list()[0];
boolean myPortIsOpen = false;
int LINE_FEED=10; 

int[] vals;

int [] wavePos = {500, 550, 600, 650};

boolean startPlaying = false;
//--setup-----------------------------------------------------------------------------------------------------setup()
void setup() {
  size(260, 750, P3D);
  smooth();

  //println(Serial.list());

  //MINIM///////////////////////////////////////////////////////
  minim = new Minim(this);

  //load music files
  player = new AudioPlayer[4];
  player[0] = minim.loadFile("sound0.wav");
  player[1] = minim.loadFile("sound1.wav");
  player[2] = minim.loadFile("sound2.wav");
  player[3] = minim.loadFile("sound3.wav");



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
    text("adsr", (width * 0.5), 129); //insert monotaskers' name here
  }
  noFill();
  stroke(255);
  rect(width * 0.5-15, 40-22, 30, 30);
  rect(30, 102, 200, 40);

  noStroke();


  if (myPortIsOpen) {                    //only execute the rest of the loop if I have selected a port and established a connection
    //visualization
    for (int i =0; i < 4; i++) {
      stroke(255);
      line(45 + i*56, 200, 45 + i*56, 370);
      //fill(255);
      //rect(40 + i*56, 285-35/2, 10, 35);
    }

    if (startPlaying) {
      for (int i =0; i<4; i++) {
        player[i].play();
        player[i].loop();
        player[i].mute();
      }
      startPlaying = false;
    }
    if (myPort.available() > 0) {        // If data is available,

      //visulization
      for (int i =0; i < 4; i++) {
        //        println(vals[i]);  
        stroke(255);
        line(45 + i*56, 200, 45 + i*56, 370);
        fill(255);
        rect(40 + i*56, int(map(vals[i], 0, 255, 335, 200)), 10, 35);
        player[i].setGain(map(vals[i], 0, 255, -40., 0.)); //set the gain according to the mapped values of our sliders
        if(player[i].getGain() <= -39.) player[i].setGain(-2000.);


        //draw the waveform
        stroke(255, 255, 255, vals[i]);
        for (int j = 0; j < player[i].bufferSize() - 1; j++)
        {
          float x1 = map( j, 0, player[0].bufferSize(), 0, width );
          float x2 = map( j+1, 0, player[0].bufferSize(), 0, width );
          line( x1, wavePos[i] + player[i].left.get(j)*50 - vals[i]*0.025, x2, wavePos[i] + player[i].left.get(j+1)*50 + vals[i]*0.025);
          line( x1, wavePos[i] + player[i].right.get(j)*50 - vals[i]*0.025, x2, wavePos[i] + player[i].right.get(j+1)*50 + vals[i]*0.025);
        }
      }
    }
  }
}


// handle serial data-----------------------------------------------------------------------------------------------------serialEvent(Serial p)
void serialEvent(Serial p) {
  String data = p.readStringUntil(LINE_FEED);
  if (data != null) {
    //println(data);
    vals = int(split(data, ' '));
  }
}

