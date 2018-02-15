#include <Wire.h>
#define SLVAE_ADDRESS 0x04
void receiveData(int byteCount);
void sendData();
int number = 0;
void setup(){    
    Serial.begin(9600);
    Wire.begin(SLVAE_ADDRESS);   
    //attach function to be called when data need to be received 
    Wire.onReceive(receiveData); 
    //attach function to be called when data need to be send
    Wire.onRequest(sendData);    
}
void loop(){
    delay(100);
}
void receiveData(int byteCount){
    number = Wire.read();      
    Serial.print("data received: ");
    Serial.print(number);                   
}
void sendData(){
    Wire.write(number);
    Serial.print("sendData() was called \n");
}














//commented for testing other scenario
/*
#define NUMBER_OF_BYTES 17
int state = 0;
volatile int numberOfBytes = 0;
char str[NUMBER_OF_BYTES+1] = {'\0'};
*/

 //commented for testing other scenario
    //digitalWrite(SDA,HIGH); //this is done in the init() function in twi.c via Wire.c, 
    //digitalWrite(SCL,HIGH); //so if I don't use Wire or twi.c I need to pull the lines up
    
  //commented for testing other scenario
    /*
    numberOfBytes = byteCount;
    
    int i = 0;
    while(Wire.available() && (i < byteCount) ){
          str[i++] = Wire.read();      
    }
    Serial.print("data received: ");
    Serial.print(str);                 //print what was received  
    */  
        //Wire.write(str);    
