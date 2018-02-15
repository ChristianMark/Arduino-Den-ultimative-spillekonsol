void rungame(){
  switch(gamestate){
        case start:    
                  resetgame(); // Restart the game variabels and print
                  gamestate = going;
                  break;
        case going:
                //Print to the LCD 
                playerTurnLCD(turn);
                
                //Get input from player           
                detectinput(turn);
             
                //Move cursor in array
                MoveInArray();
             
                //Check for button pressed
                detectput(turn);
             
                //Detect if the game has been won or if its a draw
                detectGameCondition();
             
                // Decide if the turn should change
                Turndecide();
               
                // Send out the array to the LED Matrix
                #ifndef NM
                animate();
                #else
                animateNM();
                #endif
                
                //delay(100);            
                break;
        case wonP1...wonP2:
                  delay(1000);
                  matrix.fillScreen(0);
                  matrix.show();
                  
                  // When someone has won, the SM has already shifted the turn to the other player with the Turndecide() func, so we need to invert the turn.
                  playerTurnLCD(!turn); 
                 
                  if(gamestate == wonP1){
                    boolean toggle = true;
                   
                    for(int j=0;j<3;j++){
                      if(toggle){
                        for(int i=0;i<3;i++){
                          #ifndef NM
                          drawPiece(i,j,PLAYER_1_COLOR);
                          #else
                          drawPieceNM(i,j,PLAYER_1_COLOR);
                          #endif
                          matrix.show();
                          delay(300);
                        }
                        toggle = !toggle;
                      }else{
                        for(int i=2;i>-1;i--){
                          #ifndef NM 
                           drawPiece(i,j,PLAYER_1_COLOR);
                          #else
                           drawPieceNM(i,j,PLAYER_1_COLOR);
                          #endif
                           matrix.show();
                           delay(300);
                        }
                        toggle = !toggle;
                      }
                    }
                  }else if(gamestate == wonP2) {
                    boolean toggle = true;
                   
                    for(int j=0;j<3;j++){
                      if(toggle){
                        for(int i=0;i<3;i++){
                          #ifndef NM
                          drawPiece(i,j,PLAYER_2_COLOR);
                          #else
                          drawPieceNM(i,j,PLAYER_2_COLOR);
                          #endif
                          matrix.show();
                          delay(300);
                        }
                        toggle = !toggle;
                      }else{
                        for(int i=2;i>-1;i--){
                          #ifndef NM
                          drawPiece(i,j,PLAYER_2_COLOR);
                          #else
                          drawPieceNM(i,j,PLAYER_2_COLOR);
                          #endif
                          matrix.show();
                          delay(300);
                        }
                        toggle = !toggle; 
                      }
                    }                
                  }
                  // When someone has won, the SM has already shifted the turn to the other player with the Turndecide() func, so we need to invert the turn to get the winner.
                  writeWinnerToSD(!turn);
                  //send til RPI
                  sendOneRecord();
                  gamestate = start;
                  menustate = start_menu;
                  break;
    
           //The game has ended in a draw
           case draw:
                    delay(1000);
                    matrix.fillScreen(0);
                    matrix.show();
                    lcd.setCursor(0,1);
                    lcd.print("It's a draw!");
                 
                    boolean toggle = true;
                   
                    for(int j=0;j<3;j++){
                      if(toggle){
                          for(int i=0;i<3;i++){
                            #ifndef NM
                            drawPiece(i,j,PLAYER_3_COLOR);
                            #else
                            drawPieceNM(i,j,PLAYER_3_COLOR);
                            #endif
                            matrix.show();
                            delay(300);
                          }
                          toggle = !toggle;
                      }else{
                          for(int i=2;i>-1;i--){
                            #ifndef NM
                             drawPiece(i,j,PLAYER_3_COLOR);
                             #else
                             drawPieceNM(i,j,PLAYER_3_COLOR);
                             #endif
                             matrix.show();
                             delay(300);
                          }
                          toggle = !toggle;
                      }
                    }
                    gamestate = start;
                    menustate = start_menu;
                    break;
              } // Switch/ Game State Machine
}//end run-game

/*
 * Decides whos turn it is
 */
void Turndecide(){   
    if(turn){// Player 2
        if(button2_detected ){
          #ifdef DEBUG_Turndecide
              Serial.println("Its now player 1's turn");
          #endif
          turn = PLAYER1;
          button2_detected = false;
        }
    }else{// Player 1
        if(button1_detected ){ 
          #ifdef DEBUG_Turndecide
              Serial.println("Its now player 2's turn");
          #endif
          turn = PLAYER2;
          button1_detected = false;
        }      
    }
}
 
/*
 *  Function takes the move, sees if it is possible and if so, moves the cursor
 */
void MoveInArray(){
  char newCursorPos[2] = {0,0}; // for checking
  newCursorPos[0] = cursorPos[0] + Move[0];
  newCursorPos[1] = cursorPos[1] + Move[1];
 
  if (outOfBounds(newCursorPos[0],newCursorPos[1])){
    #ifdef DEBUG_MoveInArray
        Serial.println("First bound check failed");
    #endif
    
    return;
  }
   
  cursorPos[0] = newCursorPos[0];
  cursorPos[1] = newCursorPos[1];
  return;
}
 
/*
 * returns true if the input position is out of the array
 */
bool outOfBounds(char xPos, char yPos){
  if (xPos > 2 || xPos < 0 || yPos > 2 || yPos < 0){
    return true;
  } else {
    return false;
  }
}
 
/*
 * returns true if the input position is filled already
 */
bool gameAreaFiled(char xPos, char yPos){
  if ((gameArea[yPos] [xPos]) != 0){
    return true;
  } else {
    return false;
  }
}
 
/* NEW LED MATRIX FUNC
 * Runs the animation funcktion, animates the board and cursor on the new led matrix
 */
void animateNM(){
  matrix.fillScreen(0);
  
  drawGridNM(); //draw grid
  
  //run through game area and print it in players colors
  for(size_t i = 0; i < 3; i++){
    for (size_t j = 0; j < 3; j++){
      switch (gameArea[j][i]){
        case 1:
          #ifdef DEBUG_animate
              Serial.println("Player 1's tile is being printed");
          #endif
          drawPieceNM(i, j,PLAYER_1_COLOR);
          break;
        case 2:
          #ifdef DEBUG_animate
              Serial.println("Player 2's tile is being printed");
          #endif
          drawPieceNM(i, j,PLAYER_2_COLOR);
          break;
        default:
          break;
      }//end switch
    }//end for
  }//end for
 
  //animate cursor position
  if(gamestate == going){
      #ifdef DEBUG_animate
            Serial.println("The Cursor is being printed");
      #endif
      drawPieceNM(cursorPos[0], cursorPos[1], (turn ? CURSOR_2_COLOR : CURSOR_1_COLOR));
  }
  matrix.show();
}
 
/* NEW LED MATRIX FUNC
 * Function draws a piece on the NEW LED matrix from a gameArea coordinate
 */
void drawPieceNM(char posX, char posY, int color){
  matrix.drawPixel((posX * 3),(posY * 3),color);
  matrix.drawPixel((posX * 3)+1,(posY * 3),color);
  matrix.drawPixel((posX * 3),(posY * 3)+1,color);
  matrix.drawPixel((posX * 3)+1,(posY * 3)+1,color);
}
extern void drawGridNM(){
  for (char y = 2; y < 6; y += 3){
    for (char x = 0; x < 8; x++){
      matrix.drawPixel(x, y, GRID_COLOR);
    }
  }
  for (char x = 2; x < 6; x += 3){
    for (char y = 0; y < 8; y++){
      matrix.drawPixel(x, y, GRID_COLOR);
    }
  }
  
}
 /*Runs the animation funcktion, animates the board and cursor
  */
void animate(){
  matrix.fillScreen(0);
  //run through game area and print it in players colors
  for(size_t i = 0; i < 3; i++){
    for (size_t j = 0; j < 3; j++){
      switch (gameArea[j][i]){
        case 1:
          #ifdef DEBUG_animate
              Serial.println("Player 1's tile is being printed");
          #endif
          drawPiece(i, j,PLAYER_1_COLOR);
          break;
        case 2:
          #ifdef DEBUG_animate
              Serial.println("Player 2's tile is being printed");
          #endif
          drawPiece(i, j,PLAYER_2_COLOR);
          break;
        default:
          break;
      }//end switch
    }//end for
  }//end for
 
  //animate cursor position
  if(gamestate == going){
      #ifdef DEBUG_animate
            Serial.println("The Cursor is being printed");
      #endif
      drawPiece(cursorPos[0], cursorPos[1], (turn ? CURSOR_2_COLOR : CURSOR_1_COLOR));
  }
  matrix.show();
}
 
/*Function draws a piece on the LED matrix from a gameArea coordinate
 */
void drawPiece(char posX, char posY, int color){
  matrix.drawPixel((posX * 2),(posY*2),color);
  matrix.drawPixel((posX * 2)+1,(posY*2),color);
  matrix.drawPixel((posX * 2),(posY*2)+1,color);
  matrix.drawPixel((posX * 2)+1,(posY*2)+1,color);
}
 
/* DetectInput() recieves a boolean "turn" which indicades who's turn it is. The func will change the Move[] array and return */
void detectinput(bool turn){
  int x,y = 0;
  if(turn){ // Chooses which analog stick that needs to be read from
      // Player 2
      x = analogRead(VRX2); // Read from the x-axis
      y = analogRead(VRY2); // Read from the y-axis
      
      #ifdef DEBUG_detectinput
          Serial.print("VRX2:");
          Serial.print(x);
          Serial.print("\tVRY2:");
          Serial.println(y);
      #endif
       
  }else{
      // Player 1
      x = analogRead(VRX1); // Read from the x-axis
      y = analogRead(VRY1); // Read from the y-axis
       
      #ifdef DEBUG_detectinput
          Serial.print("VRX1:");
          Serial.print(x);
          Serial.print("\tVRY1:");
          Serial.println(y);
      #endif          
  }
 
  //Decides which direction the analog stick is facing on the x-axis
  if( x > 800){
      #ifdef DEBUG_detectinput
          Serial.println("The analog stick is facing left");
      #endif
     
      Move[0] = 1;
  }else if(x < 300){
      #ifdef DEBUG_detectinput
          Serial.println("The analog stick is facing right");
      #endif     
      Move[0] = -1;
  }else{
      Move[0] = 0;
  }
 
    //Decides which direction the analog stick is facing on the y-axis
  if( y > 800){
      #ifdef DEBUG_detectinput
          Serial.println("The analog stick is facing up");
      #endif
      Move[1] = 1;
  }else if(y < 300){
      #ifdef DEBUG_detectinput
          Serial.println("The analog stick is facing down");
      #endif
      Move[1] = -1;
 
  }else{
      Move[1] = 0;
  }
  return;
}
 
/*The function finds the first "legal" position in the gameArea to place the cursor*/
boolean find_new_pos(){
        for(char i = 0;i<3;i++){
              for(char j = 0;j<3;j++){
                  if(gameAreaFiled(j, i) == 0){
                      cursorPos[0] = j;
                      cursorPos[1] = i;
                            #ifdef DEBUG_find_new_pos
                                Serial.println("A new position has been found");
                            #endif
                      
                      return 1;
                  }
              }      
      }
      return 0; // No new position available
}
 
/*The function detects a button press, inserts a '1' or '2'
 *on the position of the cursor in the gameArea Array and
 *finds a new legal position for the cursor */
void detectput(bool turn){
 
  if(turn){
        if(button2_pressed_ISR){
              #ifdef DEBUG_detectput
                   Serial.println("Button 2 has been pressed");
              #endif
              
              button2_pressed_ISR = false; // These var must be reset, because they can be toggled anytime by the ISR
              button1_pressed_ISR = false;
             
              if (!gameAreaFiled(cursorPos[0],cursorPos[1] )){
                  PUT_SOUND
                  //Serial.println("second filld check failed");
                  button2_detected = true; // This var must be set, because we need to know that the game has registered a button press
                  turns++; // Increment the amount of turns
                  CURSOR_POS_IN_AREA = 2; // Indsætter player 2's træk på banen
                  //Saves the cursorposition where a 1 or 2 has been placed. This is used in the func "detectGameCondition()"
                  latestPut[0] = cursorPos[0];
                  latestPut[1] = cursorPos[1];
                  #ifdef DEBUG_detectput    // Prints out the array on the Serial monitor
                    updateD();
                  #endif
                  find_new_pos(); //Find a new "legal" position for the cursor
                  return;
              }  else {
                ERR_SOUND
              }
 
        } // end if
  }else{
         if(button1_pressed_ISR){
              #ifdef DEBUG_detectput
                   Serial.println("Button 1 has been pressed");
              #endif
              button1_pressed_ISR = false;// These var must be reset, because they can be toggled anytime by the ISR
              button2_pressed_ISR = false;
 
              if (!gameAreaFiled(cursorPos[0],cursorPos[1] )){
                PUT_SOUND
                button1_detected = true;  //This var must be set, because we need to know that the game has registered a button press
                turns++; // Increment the amount of turns
                CURSOR_POS_IN_AREA = 1; // Indsætter player 1's træk på banen
                //Saves the cursorposition where a 1 or 2 has been placed. This is used in the func "detectGameCondition()"
                latestPut[0] = cursorPos[0];
                latestPut[1] = cursorPos[1];
                #ifdef DEBUG_detectput
                  updateD();
                #endif
                find_new_pos(); //Find a new "legal" position for the cursor
              } else {
                ERR_SOUND
              }// end if gameAreaField()
          } // end if
  } // end else
} // end func
 
/*
 * Make life great again
 * Detect the winner/draw and change game state.
 */
void detectGameCondition(){
  char xCheck = 0;
  char yCheck = 0;
  if (button1_detected || button2_detected){

    #ifdef DEBUG_detectGameCondition
        Serial.println("Check win begun");
    #endif 
/////////////////// Check column win ///////////////////
    //first make x coordinate to check
    xCheck = latestPut[0] + 1;
    if (xCheck > 2){
      xCheck = 0;
    }//end if - xCheck overflow

    #ifdef DEBUG_detectGameCondition
        Serial.print("First X coordinate generatred:\t");
        Serial.println((int)xCheck);
    #endif

    if (gameArea[latestPut[1]][xCheck] == gameArea[latestPut[1]][latestPut[0]]){

      #ifdef DEBUG_detectGameCondition
         Serial.println("First test coordnate matches latest put");
      #endif
      
      //second make x coordinate to check
      xCheck++;
      if (xCheck > 2){
        xCheck = 0;
      }//end if - xCheck overflow

      #ifdef DEBUG_detectGameCondition
          Serial.print("Second X coordinate generatred:\t");
          Serial.println((int)xCheck);
      #endif

      if (gameArea[latestPut[1]][xCheck] == gameArea[latestPut[1]][latestPut[0]]){
        
        #ifdef DEBUG_detectGameCondition
           Serial.println("Someone won");
        #endif
        if (gameArea[latestPut[1]][latestPut[0]] == 1){
          p1win();
          return;
         
        } else if (gameArea[latestPut[1]][latestPut[0]] == 2){
          p2win();
          return;
        }//end if - change gamestate
      }//end if - second check "win check"
    }//end if - first check
 
/////////////////// Check row win ///////////////////
    //first make y coordinate to check
    yCheck = latestPut[1] + 1;
    if (yCheck > 2){
      yCheck = 0;
    }//end if - yCheck overflow
    
    #ifdef DEBUG_detectGameCondition
        Serial.print("First Y coordinate generatred:\t");
        Serial.println((int)yCheck);
    #endif
    
    if (gameArea[yCheck][latestPut[0]] == gameArea[latestPut[1]][latestPut[0]]){

      #ifdef DEBUG_detectGameCondition
          Serial.println("First test coordnate matches latest put");
      #endif

      //second make x coordinate to check
      yCheck++;
      if (yCheck > 2){
        yCheck = 0;
      }//end if - yCheck overflow
      
      #ifdef DEBUG_detectGameCondition
          Serial.print("Second Y coordinate generatred:\t");
          Serial.println((int)yCheck);
      #endif

      if (gameArea[yCheck][latestPut[0]] == gameArea[latestPut[1]][latestPut[0]]){
        
        #ifdef DEBUG_detectGameCondition
           Serial.println("Someone won");
        #endif
        
        if (gameArea[latestPut[1]][latestPut[0]] == 1){
          p1win();
          return;
         
        } else if (gameArea[latestPut[1]][latestPut[0]] == 2){
          p2win();
          return;
        }//end if - change gamestate
      }//end if - second check "win check"
    }//end if - first check
 
/////////////////// Check diagonal win ///////////////////
    if ((latestPut[0] == latestPut[1]) || ((latestPut[0] - latestPut[1]) % 2 == 0)){
      //check if latest put is inside diagonals
      //might be more work to check if i should check, please reasearch that! -Mads
     
      if ((gameArea[0][0] == gameArea[1][1] && gameArea[1][1] == gameArea[2][2]) || (gameArea[0][2] == gameArea[2][0] && gameArea[1][1] == gameArea[2][0])){

        #ifdef DEBUG_detectGameCondition
            Serial.println("Diagonal win detected");
        #endif
        //winner clearly has the middel
        if (gameArea[1][1] == 1){
          p1win();
          return;
         
        } else if (gameArea[1][1] == 2){
          p2win();
          return;
        }//end if - change gamestate
      }//end if - diagonal win
    }//end if - last put on diagonal
      if(turns == 9){
        drawWin();
        return;
      }
  }//end if buttons detected
}//end function detect win state



/* Printning out the Array on the Serial monitor (DEBUG)*/
void updateD(){
  for(size_t i = 0; i < 3; i++){
    for (size_t j = 0; j < 3; j++){
      Serial.print((int)gameArea[i][j]);
    }
    Serial.println();
  }Serial.println();

}//end updateD

void p1win(){
  gamestate = wonP1;
  #ifdef DEBUG_winner
       Serial.println("Player 1 won");
  #endif
  
}//end p1win

void p2win(){
  gamestate = wonP2;
  #ifdef DEBUG_winner
       Serial.println("Player 2 won");
  #endif
}//end p2win

void drawWin(){
  gamestate = draw;
  #ifdef DEBUG_winner
         Serial.println("draw detected, 9 moves used");
  #endif

}//end drawWin


void playerTurnLCD(boolean playerTurn){

   if(playerTurn){ // Player 2
      if(gamestate == going){
          lcd.setCursor(0,1);
          for(char i = 0;i<NAME_LENGTH;i++){
              lcd.print(player2Name[i]);
          }
      }else if(gamestate == wonP2){
          lcd.setCursor(0,1);
          for(char i = 0;i<NAME_LENGTH;i++){
              lcd.print(player2Name[i]);
          }
          lcd.print(" wins");
      }
   }else{ // Player 1
      if(gamestate == going){ // Prints the player name for whome has the turn
          lcd.setCursor(0,1);
          for(char i = 0; i < NAME_LENGTH;i++){
              lcd.print(player1Name[i]);
          }
      }else if(gamestate == wonP1){ // Prints the player name for who won.
          lcd.setCursor(0,1);
          for(char i = 0;i<NAME_LENGTH;i++){
              lcd.print(player1Name[i]);
          }
          lcd.print(" wins");
      }  
   }
   lcd.noCursor(); // Remove the cursor
}

void resetgame(){
  
                  button1_pressed_ISR = false;
                  button2_pressed_ISR = false;
                  button1_detected = false;
                  button2_detected = false;
                  cursorPos[0] = 0;
                  cursorPos[1] = 0;
                  latestPut[0]= 0;
                  latestPut[1]= 0;
                  turns = 0;

                  //Write to LCD
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Tic Tac Toe");
                  //Reset the gameArea and the LED Maxtrix
                  for(int i = 0;i<3;i++){
                      for(int j = 0;j<3;j++){
                        gameArea[i][j] = 0;
                      }
                  }
                  matrix.fillScreen(0);
                  matrix.show(); 
}
