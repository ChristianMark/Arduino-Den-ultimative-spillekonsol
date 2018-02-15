void writeWinnerToSD(boolean winner) {
  File writeFile;                 //for write file
  char score[SCORE_LENGTH];       //to contain score as string
  int finalPoints;                //to contain score as int
  bool foundName = false;         //used for searching name

  if (sendingDataToRPI) {
          //if sending data, write date to buffer file
    if (writeFile = SD.open("buffer.txt", O_RDWR)) {
#ifdef DEBUG_writeWinnerToSD
      Serial.println("Write file 'buffer.txt' opened");
#endif
    } else {
#ifdef DEBUG_writeWinnerToSD
      Serial.println("Error, could not open");
#endif
      return;
    }//end if - open SD
  } else {  //else open score file
    
    if (writeFile = SD.open("score.txt", O_RDWR)) {
#ifdef DEBUG_writeWinnerToSD
      Serial.println("Write file 'score.txt' opened");
#endif
    } else {

#ifdef DEBUG_writeWinnerToSD
      Serial.println("Error, could not open");
#endif
      return;
    }//end if - open SD
  }//end if - sendingDataToRPI



      //search for player 1 or 2, depending on who wins.
  if (winner) {
    foundName = writeFile.find(player2Name, NAME_LENGTH);
  } else {
    foundName = writeFile.find(player1Name, NAME_LENGTH);
  }//end if - winner find

#ifdef DEBUG_writeWinnerToSD
  Serial.print("The winner is: ");
  serialPrintArr(winner ? player2Name : player1Name, NAME_LENGTH);
#endif


      //if the name is found, 1 is added to players score
      //if not,  a new name has to be added to the list
  if (foundName) {

#ifdef DEBUG_writeWinnerToSD
    Serial.println("player found in highscore");
    Serial.print("This is pos after find: ");
    Serial.println(writeFile.position());
#endif

    //jump from end of line to begining of line to read score
    writeFile.seek(writeFile.position() - LINE_LENGTH);

#ifdef DEBUG_writeWinnerToSD
    Serial.print("This is pos after seek: ");
    Serial.println(writeFile.position());
#endif

        //read the SCORE_LENGTH char score into char arr
    for (char i = 0; i < SCORE_LENGTH; i++) {
      score[i] = writeFile.read();
    }//end for

#ifdef DEBUG_writeWinnerToSD
    Serial.print("This is pos after read: ");
    Serial.println(writeFile.position());
    serialPrintArr(score, SCORE_LENGTH);
#endif

        //jump cursor back to before read
    writeFile.seek(writeFile.position() - SCORE_LENGTH);

        //make the score into an int and add one
    finalPoints = atoi(score);
    finalPoints++;
    
    if(finalPoints > MAX_SCORE){ // The score can only be < MAX_SCORE
      finalPoints =  MAX_SCORE;
      Serial.println("The score has been set to MAX_SCORE");
    }

        //write finalPoints formatted to score again
    if(SCORE_LENGTH == 3){
      sprintf(score, "%03d", finalPoints);
    }else{
      Serial.println("The 'SCORE_LENGTH' MACRO has been changed from 3, go change code in SD_FUNC");
    }
    

#ifdef DEBUG_writeWinnerToSD
    Serial.println(finalPoints);
    Serial.print("New score: ");
    serialPrintArr(score, SCORE_LENGTH);
#endif

    //write the score to the place in file
    writeFile.write(score);


  } else {
        //the playername was not found in highscore list

#ifdef DEBUG_writeWinnerToSD
    Serial.println("player not found in highscore");
    Serial.print("This is pos after find:");
    Serial.println(writeFile.position());
#endif
        //position is now at the end of the file, since we could not find the player


        //now write the first point for player
    for(char i = 0; i < SCORE_LENGTH - 1; i++){
      writeFile.write("0");
    }
    writeFile.write("1 ");
    
    //writeFile.write("001 ");

        //now write playerName to file
    if (winner) {
      for (char i = 0; i < NAME_LENGTH; i++) {
        writeFile.write(player2Name[i]);
      }
    } else {
      for (char i = 0; i < NAME_LENGTH; i++) {
        writeFile.write(player1Name[i]);
      }
    }//end if - winner find

    writeFile.write("\n");
#ifdef DEBUG_writeWinnerToSD
    Serial.print("This is pos after write:");
    Serial.println(writeFile.position());
#endif
  }//if else - foundname

      //close the file as the file is only to be used in function
  writeFile.close();
}//end function - writeWinnerToSD



//////////////////////////////// DEBUG FUNCTION //////////////////////////////////

void serialPrintArr(char* arr, int len) {
  for (char i = 0; i < len; i++) {
    Serial.print(arr[i]);
    Serial.print(", ");
  }
  Serial.println("");
}
