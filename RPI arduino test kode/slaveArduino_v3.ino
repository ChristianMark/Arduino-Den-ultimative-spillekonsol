#include <Wire.h>
#include <SD.h>
//SD card
#define NUMBER_RECORDS 5
#define SD_CARD_PIN 53
#define SLAVE_ADDRESS 0x04
#define NUMBER_OF_BYTES 17

File scoreRecordsFile;
int numberOfRecords = 0;
volatile char * record;
char str[NUMBER_OF_BYTES+1] = {'\0'};
char arr2[NUMBER_RECORDS][10]={'\0'};
                                            //volatile char record[] = "356 Mads2";
volatile int index = 0;
volatile int recordIndex = 0;
                                            //char * arr[NUMBER_RECORDS] = {"356 Mads2","032 Mark ","100 Peter","003 Mads ","501 Nick "};
void receiveData(int byteCount);
void sendData();

void setup(){
    Serial.begin(9600);    
    //start of records extraction
    if(!SD.begin(SD_CARD_PIN)){
      Serial.println("SD card init failed!");
      while(true) //stay here forever 
        ;
    }else{
      Serial.println("SD card init OK!");
    }        
    //open file for reading 
    scoreRecordsFile = SD.open("SCOR.txt");
    if(scoreRecordsFile){ //chek if it was opened
      Serial.println("SCOR.txt opened OK!");
      //read the file
      int i = 0;
      while(scoreRecordsFile.available()){
          Serial.print("record: ");
          String str = scoreRecordsFile.readStringUntil('\n'); //returns a String 
          int n = str.length();
          char chars[10] = {'\0'};    //fill with end of string character
          strcpy(chars, str.c_str()); //copy the string read from file to char array         
          if((i < NUMBER_RECORDS) && (i >= 0)){    //read only the first 5 records, its top 5
            for(int k = 0; k < n; k++){
              arr2[i][k] = chars[k];  //copy local char to global char array
                                      //so it can be used later to send data 
            }     
            Serial.println(arr2[i]);  //print the record
          }
          i++; //increase the count for records
      }      
      scoreRecordsFile.close();
    }else{
      Serial.println("Error opening SCOR.txt !");
    }
    //end of records extraction        
    Wire.begin(SLAVE_ADDRESS);    
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
    record = arr2[recordIndex++]; //get pointer to the first record
}
void loop(){
    delay(0);
}
void receiveData(int byteCount){
    while(Wire.available()){
      index = (int)Wire.read(); //read the next index      
      Serial.print("index received: ");
      Serial.println(index);
    }
}//end of receiveData
void sendData(void){
  Serial.print("sendData() was called \n");    
  Wire.write((byte)record[index]); //send current char  
  Serial.print("char was sent: ");
  Serial.println(record[index]);  
  if(index+1 > 8){ //RPi will request 8 chars only
    record = arr2[recordIndex++]; //get pointer to new record
    if(recordIndex > 4){ //there are only 5 records to send
      recordIndex = 0;   //start again 
    }//start again    
  }
}//end of sendData

/*
  char temp[NUMBER_RECORDS];
  int count = sizeof(temp);
  memset(temp,'\0', count);
  strncpy(temp, (const char *)&record[index], count );  
  //sprintf(temp, "%5c", &record[index]); //this is just another option
  unsigned char toSend[count] = {'\0'};
  for(int i = 0; i < count; i++){
    toSend[i] = (unsigned char)temp[i];
  }
  
  Wire.write( &toSend[0], sizeof(toSend));
  
  if(index+1 > 8){ 
    record = arr2[recordIndex++];
    if(recordIndex > 4){ recordIndex = 0; }//start again    
  }
  */
    
    /*
//get number of records
    scoreRecordsFile = SD.open("SCOR.txt");
    if(scoreRecordsFile){
      Serial.println("SCOR.txt opened OK for counting !");
      while(scoreRecordsFile.available()){
          scoreRecordsFile.readStringUntil('\n'); 
          numberOfRecords++;
      }
      scoreRecordsFile.close();
    }else{
      Serial.println("Error opening SCOR.txt for counting !");
    }
    */
