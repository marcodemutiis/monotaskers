 /*
  *
  * This sketch demonstrates how read values from an o 
  * monotasker in Processing and trigger play/pause of an
  * audio file, and control playing position with the 
  * minimlibrary.
  * for more info visit monotaskers.com.
  *
  */
  
import processing.serial.*;
import controlP5.*;
import ddf.minim.*;
import ddf.minim.*;
import ddf.minim.ugens.*;

Minim minim;
AudioPlayer player;

ControlP5 cp5;
DropdownList d1;

PFont f;
char val;

Serial myPort;
String portName = Serial.list()[0];
boolean myPortIsOpen = false;
boolean play = false;



float angle = 30;
//--setup-----------------------------------------------------------------------------------------------------setup()
void setup() {
  size(260, 750, P3D);
  smooth();

  //println(Serial.list());

  //MINIM///////////////////////////////////////////////////////
  minim = new Minim(this);
  player = minim.loadFile("one_last_lemon_tea.mp3");


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
    text("o", (width * 0.5), 129); //insert monotaskers' name here
  }
  noFill();
  stroke(255);
  rect(width * 0.5-15, 40-22, 30, 30);
  rect(30, 102, 200, 40);


  if (myPortIsOpen) {                    //only execute the rest of the loop if I have selected a port and established a connection
    if (myPort.available() > 0) {        // If data is available,
      int valInt = myPort.read();        // read it and store it in val
      val = char(valInt);                //convert to char
      println(val);
    }



    pushMatrix();
    translate(width*0.5, 300);
    if (val != 'P' && val!= ' ') {
      angle = map(val, 0, 23, 0, 360);
      float newPos = map(val, 0, 23, -12, 12);
      player.cue(player.position()+int(newPos));
      val = ' ';
    }
    else if (val == 'P') push();
    rotate(radians(45));
    rotate(radians(angle));

    stroke(255);
    ellipse(0, 0, 180, 180);
    line(0-55/2, 0, 0-55/2, 0-55/2);
    line(0, 0-55/2, 0-55/2, 0-55/2);
    fill(255);
    triangle(0-55/2, 0, 0, 0-55/2, 0-55/2, 0-55/2);
    noStroke();
    ellipse(0, 0, 55, 55);
    popMatrix();
    
    
    if(player.isPlaying()){
      stroke(255);
    for(int i = 0; i < player.bufferSize() - 1; i++)
  {
    float x1 = map( i, 0, player.bufferSize(), 0, width );
    float x2 = map( i+1, 0, player.bufferSize(), 0, width );
    line( x1, height*0.65 + player.left.get(i)*50, x2, height*0.65 + player.left.get(i+1)*50 );
    line( x1, height*0.65 +100 + player.right.get(i)*50, x2, height*0.65+100 + player.right.get(i+1)*50 );
  }};
  }

}


void push() {
  background(255);
  val = ' ';
  play = !play;

  if (play) player.play();
  else player.pause();
}

