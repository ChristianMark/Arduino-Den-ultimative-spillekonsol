/* Change name variables with the analog sticks. Only uses
 *  big letters and space.
 *  Call with a turn variabel, false for p1, and true for p2
 *  
 */
void inputName(bool chnmPlayer){
  char temp = 0;
  char useOldName_index = 0;
  
  if(chnmPlayer){//Player 2
    temp = player2Name[useOldName_index];
  }else{//Player 1
    temp = player1Name[useOldName_index];
  }
  
  bool button_detect;
 
  bool old_turn;            //hold old global turn value
  old_turn = turn;        //save global turn
  turn = chnmPlayer;            //change global turn value
 
  chnmPlayer ? button_detect = button2_pressed_ISR : button_detect = button1_pressed_ISR;
 
  lcd.clear();
  if (chnmPlayer){
    lcd.print("Player 2 name:");
  } else {
    lcd.print("Player 1 name:");
  }
 
  lcd.setCursor(0,1);
  lcd.cursor();
  for (char i = 0; i < (NAME_LENGTH); ++i){
    while (!button_detect){
      lcd.setCursor(i,1);
      lcd.write(temp);
      detectinput(chnmPlayer); //detect input for player 1
      delay(250);         //delay for responsive ness feeling
     
      //increment temp
      if (Move[0] > 0){
        temp++;
      } else if(Move[0] < 0){
        temp--;
      }//increment temp
 
     
      //made for looping char choise around
      switch (temp){
        case 91 :
            temp = 32;
            break;
        case 64 :
            temp = 32;
            break;
        case 31 :
            temp = 90;
            break;
        case 33 :
            temp = 65;
            break;
        default :
            break;
      }//end switch
 
      //poll for button press
      chnmPlayer ? button_detect = button2_pressed_ISR : button_detect = button1_pressed_ISR;
     
    }//end while - not pressed
   
    //save char
    chnmPlayer ? player2Name[i] = temp : player1Name[i] = temp;

    useOldName_index++;
    if(chnmPlayer){//Player 2
      temp = player2Name[useOldName_index];
    }else{//Player 1
      temp = player1Name[useOldName_index];
    }    
   
    //reset press ISR
    button1_pressed_ISR = false;
    button2_pressed_ISR = false;
    button1_detected = false;
    button2_detected = false;
    button_detect = false;
  }//end for - name
 lcd.noCursor();
  turn = old_turn; //restore global turn to previous value
 
}//end inputName
 

 
////////////////////////////////////////////// DEBUG FUNCTION //////////////////////////////////
void printNamesSerial(){
  for (char i = 0; i < NAME_LENGTH; ++i){
    Serial.print(player1Name[i]);
  }
  Serial.println("");
  for (char i = 0; i < NAME_LENGTH; ++i){
    Serial.print(player2Name[i]);
  }
}
