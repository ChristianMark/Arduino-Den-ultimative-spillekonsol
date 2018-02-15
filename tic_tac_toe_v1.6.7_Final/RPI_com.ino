void sendOneRecord(){
  
  lcd.setCursor(0 , 1);
  lcd.print("Sending data... ");
  char i = 0;
  digitalWrite(RPI_cp, HIGH);
  while(digitalRead(RPI_cp)){
    //Serial.print("This is value of done sending:");
    //Serial.println(doneSending);
    delay(200);
    if (++i > 15){
      digitalWrite(RPI_cp, LOW);
      i=0;
    }
  }
  digitalWrite(RPI_cp, LOW);
  doneSending = false;
}


void sendData(void) {

  static int index = 0;

#ifdef DEBUG_sendData
  Serial.print("sendData() was called \n");
#endif

  if (turn) {
    Wire.write((byte)player1Name[index]);
  } else {
    Wire.write((byte)player2Name[index]);
  }
  index++;

  //reset index and done sending
  if (index >= NAME_LENGTH) {
    index = 0;
    doneSending = true;
    digitalWrite(RPI_cp, LOW);
  }


#ifdef DEBUG_sendData
  Serial.print("char was sent: ");
  if (turn) {
    Serial.println(player1Name[index]);
  } else {
    Serial.println(player2Name[index]);
  }//end if
#endif
}

/*
  void receiveData(int byteCount){
    while(Wire.available()){
      index = (int)Wire.read();
    Serial.print("index received: ");
    Serial.println(index);
    }
  }
*/
