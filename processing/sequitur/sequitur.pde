 /*
  *
  * This sketch demonstrates how read values from a sequitur 
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
AudioSample [] sample;

ControlP5 cp5;
DropdownList d1;

PFont f;
char val;

Serial myPort;
String portName = Serial.list()[0];
boolean myPortIsOpen = false;


//--setup-----------------------------------------------------------------------------------------------------setup()
void setup() {
  size(260, 750, P3D);
  background(100);
  smooth();

  //println(Serial.list());

  //MINIM///////////////////////////////////////////////////////
  minim = new Minim(this);

  //load music samples
  sample = new AudioSample[8];
  sample[0] = minim.loadSample("drum0.mp3", 512);
  sample[1] = minim.loadSample("drum1.mp3", 512);
  sample[2] = minim.loadSample("drum2.mp3", 512);
  sample[3] = minim.loadSample("drum3.mp3", 512);
  sample[4] = minim.loadSample("drum4.mp3", 512);
  sample[5] = minim.loadSample("drum5.mp3", 512);
  sample[6] = minim.loadSample("drum6.mp3", 512);
  sample[7] = minim.loadSample("drum7.mp3", 512);

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
  if (!myPortIsOpen)  background(100);
  else {
    fill(100);
    noStroke();
    rect(0, 0, width, 400);              //draw the upper half of the background
  }
  
  //logos//
  fill(255);
  textAlign(CENTER);
  for (int i =0; i<2; i++) {   
    text("m", (width * 0.5), 40);
    text("monotaskers", (width * 0.5), 80); //we write it twice for better visibility
    text("sequitur", (width * 0.5), 129);
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

    //draw the buttons to visualize inputs
    noFill();
    stroke(255);
    for (int j=0; j<2; j++) {
      for (int i = 0; i< 4; i++) {

        rect(35+50*i, 200 + j*68, 40, 57, 5);
        line(35+50*i, 219 + j*68, 35+50*i+40, 219 + j*68);
        line(35+50*i, 222 + j*68, 35+50*i+40, 222 + j*68);
        ellipse(35+50*i+20, 209 + j*68, 10, 10);
      }
    }


    //trigger samples 
    char [] triggerChar = {
      '0', '1', '2', '3', '4', '5', '6', '7'
    }; 


    for (int i = 0; i < 8; i++) {
      if (triggerChar[i] == val) {
        sample[i].trigger();

        stroke(255);
        // use the mix buffer to draw the waveforms.
        for (int j = 0; j < sample[i].bufferSize() - 1; j++)
        {
          float x1 = map(j, 0, sample[i].bufferSize(), 0, width);
          float x2 = map(j+1, 0, sample[i].bufferSize(), 0, width);
          line(x1, 400 - sample[i].mix.get(j)*50, x2, 405 + (i*50) - sample[i].mix.get(j+1)*50);
        }
      }
    }

    //visualize steps 
    char [] ledChar = { 
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'
    };
    for (int i = 0; i < 8; i++) {
      if (ledChar[i] == val) {
        fill(255);
        if (i <= 3 ) ellipse(35+50*i+20, 209, 10, 10);
        else  ellipse(35+(50*(i-4))+20, 209 + 68, 10, 10);
      }
    }

    fill(100, 100, 100, 50);
    noStroke();
    rect(0, 400, width, height-400);
  }
}

