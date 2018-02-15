#include <Wire.h>
#include <SD.h>

#define SIGNAL_PIN 22

//SD card
#define SD_CARD_PIN 53
File scoreRecordsFile;
int numberOfRecords = 0;
#define SLAVE_ADDRESS 0x04
#define NUMBER_OF_BYTES 17

char str[NUMBER_OF_BYTES+1] = {'\0'};
//volatile char record[] = "356 Mads2";
volatile char * record;
volatile int index = 0;
volatile int recordIndex = 0;
char * arr[5] = {"356 Mads2","032 Mark ","100 Peter","003 Mads ","501 Nick "};
char arr2[5][10]={'\0'};

void receiveData(int byteCount);
void sendData();

volatile int index2 = 0;

void setup(){
    
    Serial.begin(9600);
    pinMode(SIGNAL_PIN, OUTPUT);
    
    if(!SD.begin(SD_CARD_PIN)){
      Serial.println("SD card init failed!");
      while(true)
        ;
    }else{
      Serial.println("SD card init OK!");
    }
        
    //open file for reading 
    scoreRecordsFile = SD.open("SCOR.txt");
    if(scoreRecordsFile){
      Serial.println("SCOR.txt opened OK!");
      //read the file
      int i = 0;
      while(scoreRecordsFile.available()){
          Serial.print("record: ");
          String str = scoreRecordsFile.readStringUntil('\n'); 
          int n = str.length();
          //char chars[n+1] = {'\0'};
          char chars[10] = {'\0'};
          strcpy(chars, str.c_str());
          
          if((i < 5) && (i >= 0)){    
            for(int k = 0; k < n; k++){
              arr2[i][k] = chars[k]; 
            }     
            Serial.println(arr2[i]);  
            //arr2[i++] = chars;            
            //Serial.println(arr2[i-1]);
          }
          i++;
      }
      
      scoreRecordsFile.close();
    }else{
      Serial.println("Error opening SCOR.txt !");
    }
    
    Wire.begin(SLAVE_ADDRESS);    
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
    record = arr2[recordIndex++];
}


void loop(){  
    digitalWrite(SIGNAL_PIN,HIGH);  //START signal to RPi
    delay(2);
    digitalWrite(SIGNAL_PIN,LOW);  //STOP signal to RPi
    delay(2);
}
void sendData(void){
  Serial.print("sendData() was called \n");
  char myname[6] = "MARK ";                 //hard-coded for testing
  Wire.write(myname[index2++]); 
  if(index2 == 5) index2 = 0;
}

void receiveData(int byteCount){
    while(Wire.available()){
      index = (int)Wire.read();      
    Serial.print("index received: ");
    Serial.println(index);
    }
}

  /*
  //Wire.write((byte)record[index]); //uncomment after test
  Serial.print("char was sent: ");
  Serial.println(record[index]);
 
  //reset the index in the end
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
