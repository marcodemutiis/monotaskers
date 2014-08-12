/*
  *
 * This sketch demonstrates how read values from a binome 
 * monotasker in Processing and trigger samples with minim 
 * library.
 * for more info visit monotaskers.com.
 *
 */

import processing.serial.*;
import controlP5.*;
import ddf.minim.*;
import ddf.minim.*;
import ddf.minim.ugens.*;

Minim minim;
AudioSample[] sample;

ControlP5 cp5;
DropdownList d1;

PFont f;

char val;

Serial myPort;
String portName = Serial.list()[0];
boolean myPortIsOpen = false;
boolean [] pushed = {
  false, false, false, false
};

char [] pushBtn = {
  '2', '1', '4', '3'
};
char [] releaseBtn = {
  '6', '5', '8', '7'
};


//--setup-----------------------------------------------------------------------------------------------------setup()
void setup() {
  size(260, 750, P3D);
  smooth();
  //println(Serial.list());

  //MINIM///////////////////////////////////////////////////////
  minim = new Minim(this);

  //trigger a sample if button is PUSHED
  //load samples
  sample = new AudioSample[8];
  sample[0] = minim.loadSample("ukulele0.mp3", 512);
  sample[1] = minim.loadSample("ukulele1.mp3", 512);
  sample[2] = minim.loadSample("ukulele2.mp3", 512);
  sample[3] = minim.loadSample("ukulele3.mp3", 512); 
  //trigger a sample if button is RELEASED
  //load samples
  sample[4] = minim.loadSample("ukulele4.mp3", 512);
  sample[5] = minim.loadSample("ukulele5.mp3", 512);
  sample[6] = minim.loadSample("ukulele6.mp3", 512);
  sample[7] = minim.loadSample("ukulele7.mp3", 512);

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
  //background(100);//(128, 0, 54);
  noStroke();
  if (!myPortIsOpen) fill(100);
  else fill(100, 100, 100, 75); 
  rect(0, 300, width, height);
  fill(100);
  rect(0, 0, width, 300);

  //logos//
  fill(255);
  textAlign(CENTER);
  for (int i =0; i<2; i++) {   
    text("m", (width * 0.5), 40);
    text("monotaskers", (width * 0.5), 80); //we write it twice for better visibility
    text("binome", (width * 0.5), 130);
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
      fill(100);
      rect(35+50*i, 200, 40, 40, 7);

      if (val == ' ') {      
        if (!pushed[i])fill(100, 100, 100, 50); //draw rectangles grey if buttons are NOT PUSHED
        else fill(255); //draw rectangle white if PUSHED
        rect(35+50*i, 200, 40, 40, 7);
      }

      //BUTTON PUSHED state ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
      else if (pushBtn[i] == val) {

        //trigger the sample
        sample[i].trigger();

        stroke(255);
        noFill();
        // use the mix buffer to draw the waveforms.
        for (int j = 0; j < sample[i].bufferSize() - 1; j++)
        {
          float x1 = map(j, 0, sample[i].bufferSize(), 0, width);
          float x2 = map(j+1, 0, sample[i].bufferSize(), 0, width);
          line(x1, 400 - sample[i].mix.get(j)*200, x2, 400 - sample[i].mix.get(j+1)*200);
        }
        
        val = ' ';
        pushed[i]=true;
      }


      //BUTTON RELEASED state ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
      else if (releaseBtn[i] == val) {
  
        //trigger the sample
        sample[i+4].trigger();

        // use the mix buffer to draw the waveforms.
        stroke(255);
        noFill();
        for (int j = 0; j < sample[i+4].bufferSize() - 1; j++)
        {
          float x1 = map(j, 0, sample[i+4].bufferSize(), 0, width);
          float x2 = map(j+1, 0, sample[i+4].bufferSize(), 0, width);
          line(x1, 400 - sample[i+4].mix.get(j)*200, x2, 400 - sample[i+4].mix.get(j+1)*200);
        }
        val = ' ';
        pushed[i]=false;
      }
    }
  }
}

