
//--customize-----------------------------------------------------------------------------------------------------customize(DropdownList ddl)
void customize(DropdownList ddl) {
  // a convenient function to customize a DropdownList
  ddl.setBackgroundColor(color(190));
  ddl.setItemHeight(20);
  ddl.setBarHeight(15);
  ddl.captionLabel().set("select SERIAL PORT");
  ddl.captionLabel().style().marginTop = 3;
  ddl.captionLabel().style().marginLeft = 3;
  ddl.valueLabel().style().marginTop = 3;
  for (int i= 0; i< Serial.list().length;i++) {
    ddl.addItem(Serial.list()[i], i);
  }
  //ddl.scroll(0);
  ddl.setColorBackground(color(150));
  ddl.setColorActive(color(128));//(255, 128));
}
