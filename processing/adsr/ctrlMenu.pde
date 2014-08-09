
//--controlEvent-----------------------------------------------------------------------------------------------------controlEvent(ControlEvent theEvent)
void controlEvent(ControlEvent theEvent) {
  // DropdownList is of type ControlGroup.
  // A controlEvent will be triggered from inside the ControlGroup class.
  // therefore you need to check the originator of the Event with
  // if (theEvent.isGroup())
  // to avoid an error message thrown by controlP5.

  if (theEvent.isGroup()) {
    // check if the Event was triggered from a ControlGroup
    println("event from group : "+theEvent.getGroup().getValue()+" from "+theEvent.getGroup());

    int newPort = int(theEvent.getGroup().getValue());

    if (newPort >=0 && newPort <= Serial.list().length) {
      try {
        if (myPortIsOpen) myPort.stop();

        portName = Serial.list()[newPort];
        myPort = new Serial(this, portName, 9600);
        myPortIsOpen = true;
        startPlaying = true;
        println("connected port " + newPort + ";");
        // clear and wait for linefeed
        myPort.clear();
        myPort.bufferUntil(LINE_FEED);
      }
      catch(Exception e) {
        e.printStackTrace();
        myPort.stop();
      }
    }
  }
  else if (theEvent.isController()) {
    println("event from controller : "+theEvent.getController().getValue()+" from "+theEvent.getController());
  }
}

