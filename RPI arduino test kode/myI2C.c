#include <avr/io.h>
#include <avr/interrupt.h>
#include "myI2C.h"
#include "util/delay.h"

static void (* onSlaveTransmit)(void);//pointer to function with no params
static void (* onSlaveReceive)(uint8_t * receiveBuffer, int bufferIndexRX);//pointer to function with params: array address and array size

void attachSlaveTxEvent(void(*)(void) function);
void attachSlaveRxEvent(void(*)(uint8_t *, int) function);

void attachSlaveTxEvent(void(*)(void) function){
    onSlaveTransmit = function;	
}
void attachSlaveRxEvent(void(*)(uint8_t *, int) function){
    onSlaveReceive = function;
}

//remember to pull SDA and SCL up f.eks.: digitalWrite(SDA, HIGH); digitalWrite(SCL, HIGH);
void i2c_init(){
    TWI_info.mode = READY;
    TWI_info.errorCode = 0xFF;
    TWI_info.repStart = 0;
    // Set pre-scalers (no pre-scaling)
    TWSR = 0;
    // Set bit rate
    TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
    // Enable TWI and interrupt
    TWCR = (1 << TWIE) | (1 << TWEN);
}

uint8_t isReady(){
return (i2c_info.mode == READY)|(i2c_info.mode == REPEATED_START_SENT) ? 1 : 0;
}
uint8_t transmitData(void * const dataToTransmit, uint8_t size, uint8_t isRepeatedStart){
    if(size <= TX_MAX_BUFFER_LENGHT){
       //wait to be ready
       while(!isReady()){
         ;
       }
      //set repeat status mode
      i2c_info.repStart = isRepeatedStart;
      //copy data in the transmit buffer
      uint8_t * dataPtr =  (uint8_t *)dataToTransmit;
      //make the data global
      for(int i = 0; i < size; i++){
          transmitBuffer[i] =  dataPtr[i];
      }
      bufferIndexTX = 0;
      bufferLenghtTX = size;
      //if a repeated start  was sent ,the i2c devises are already listening for address 
      //no need for another start signal
      if(i2c_info.mode == REPEATED_START_SENT){
         i2c_info.mode = INITIALIZING;
         //load data to the transmit buffer
         TWDR = transmitBuffer[bufferIndexTX++];
         //call macro to sent current byte
         SEND_TRANSMIT();
      }else{ //else send the normal start signal to begin transmission
         i2c_info.mode = INITIALIZING;
         SEND_START();
      }
    }else{
        return 1; //this is error as data size is bigger than the buffer size
    }
    return 0; //transmission of the one byte in TWDR(the data register) was okay
}

//this will be called after each i2c hardware operation
ISR(i2c_vector){
    switch(STATUS){//this is the status register vale(TWSR & 0xF8)	
        //Master is transmiter or writing address
        case MT_SLAW_ACK: //master transmitted -> slave address and write flag, and got ACK
            //set mode to master transmitter
            i2c_info.mode = MASTER_TRANSMITTER;
            //there is no break here so jump to the next case
        case START_SENT: //start signal was transmitted
        case MT_DATA_ACK: //one byte of data has been transmitted, and got ACK
            if(bufferIndexTX < bufferLenghtTX){
	        //load data to the transmit buffer
                TWDR = transmitBuffer[bufferIndexTX++];
                SEND_TRANSMIT(); //macro -> sent current byte signal to the i2c hardware
			}else if(i2c_info.repStart){//this transmission is complete but do not relese the bus yet
				i2c_info.errorCode = 0xFF;
				SEND_START();//macro -> send start signal to the i2c hardware
            }else{ //all transmiffions are done , its time to exit
				i2c_info.mode = READY;
				i2c_info.errorCode = 0xFF;
                SEND_STOP();//macro -> send stop signal to the i2c hardware
            }
	    break;


	//Master is receiver
	case MR_SLAR_ACK: //master receiver -> slave address and read flag, got ACK
	    //switch to master receiver mode
            i2c_info.mode = MASTER_RECEIVER;
	    //if there is more than one byte to read, receive data byte and return ACK
	    if(bufferIndexRX < bufferLenghtTX-1){
	        i2c_info.errorCode = NO_RELEVANT_INFO;
		SEND_ACK();//macro -> send ACK to the i2c hardware so that the next byte can be sent
            }else{ //when the only data byte has been received, return NACK
	        i2c_info.errorCode = NO_RELEVANT_INFO;
		SEND_NACK();//macro -> send NACK to the i2c hardware so that no more bytes need to be received
            }
	    break;
	case MR_DATA_ACK: // data byte was received and ACK was received meaning more bytes waiting to be received
            //insert the byte from the data register to the received buffer array 
            receiveBuffer[bufferIndexRX++] = TWDR;

            //if there is more than one byte to read , receive data byte and return ACK
	    if(bufferIndexRX < bufferLenghtRX - 1){
	        i2c_info.errorCode = NO_RELEVANT_INFO;
	        SEND_ACK();
	    }else{//when the last byte was received, set errorCode and return NACK
	        i2c_info.errorCode = NO_RELEVANT_INFO;
	        SEND_NACK();
	    }
	    break;
	case MR_DATA_NACK:  //the last data byte is about to be received and NACK was transmitted. This is the end of the transmission. 
	    //extract the last byte
	    receiveBuffer[bufferIndex++] = TWDR;
	    //the current transmission is complete, but do not relese the bus yet
	    if(i2c_info.repStart){
	        i2c_info.errorCode = 0xFF;
                SEND_START();
            }else{//all transmissions are completed, send stop signal
	        i2c_info.mode = READY;
		i2c_info.errorCode = 0xFF;
		SEND_STOP(); //macro -> send stop to the i2c hardware
	    }
	    break;

	//Master receiver and Master transmitter general cases
	case MR_SLAR_NACK: //slave address transmitted with read flag, NACK received 
	case MT_SLAW_NACK: //slave address transmitted with write flag, NACK received
	case MT_DATA_NACK: //arbitration has been lost
	    if(i2c_info.repStart){
	        i2c_info.errorCode = STATUS;
		SEND_START(); //macro -> send start to the i2c hardware
	    }else{//all transmitions are complete, its time to exit
	        i2c_info.mode = READY;
		i2c_info.errorCode = STATUS;
		SEND_STOP(); //macro -> send stop to the i2c hardware
            }
	    break;

	//Slave is receiver
	case SR_SCL_ACK://addressed, returned ACK
        case SR_GCALL_ACK: //addressed generally, returned ACK
        case SR_ARB_LOST_SLA_ACK://master lost arbitration, addressed and ACK returned
        case SR_ARB_LOST_GCALL_ACK://general call, master lost arbitration, addressed ACK returned
            //go into slave receiver mode
	    i2c_info.mode = SLAVE_RECEIVER;
	    //indicate that the  reciveBuffer can be overwritten and ACK
	    bufferIndexRX = 0;
	    SEND_ACK();
	    break;
        case SR_DATA_ACK://data received, returned ack
	case SR_GCALL_DATA_ACK: //data received generally, returned ack
	    if(bufferIndexRX < bufferLenghtRX){
	        receiveBuffer[bufferIndexRX++] = TWDR;
		i2c_info.errorCode = NO_RELEVANT_INFO;
		SEND_ACK();
	    }else{
		i2c_info.errorCode = NO_RELEVANT_INFO;
	        SEND_NACK();
            }
	    break;
	case SR_STOP: //stop or repeated start condition received
	    //put end of string char in the end if there is spece
	    if(bufferIndexRX < bufferLenghtRX){
	        receiveBuffer[bufferIndex]='\0';
	    }
            //callback to the user-defined callback function
	    onSlaveReceive(receiveBuffer, bufferIndex);
            SEND_ACK();//ACK future responses
	    i2c_info.mode = READY;
	    break;
	case SR_DATA_NACK: //data received, returned NACK
	case SR_GCALL_DATA_NACK: //data recived generally, returned NACK
	    //NACK  than  back to master
	    SEND_NACK();
	    break;

	//Slave transmitter
        case ST_SLA_ACK://addressed and ACK returned
	case ST_ARB_LOST_SLA_ACK: //master lost arbitration, ACK return
            i2c_info.mode = SLAVE_TRANSMITTER;   //enter slave transmitter mode
            bufferIndexTX = 0; //get the bufferIndex ready for iterations
	    bufferLenghtTX = 0; //let the user change the lenght
            //user nust call i2c_transmit(bytes,lenght); to fill the buffer and the lenght 
	    onSlaveTransmit();
	    //if user didn't change the buffer and lenght give them init values
	    if(bufferLenghtTX==0){
	        bufferLenght = 1;
		transmitBuffer[0] = 0x00;
            }
	//transmit first byte from buffer, fall
        case ST_DATA_ACK://byte was sent and ACK was returned
	    //copy data to output register
	    TWDR = transmitBuffer[bufferIndexTX++];
	    //if more to send give, ACK otherse give NACK
            if(bufferIndexTX < bufferLenghtTX){
	        SEND_ACK();
            }else{
                SEND_NACK();
	    }
	    break;
	case ST_DATA_NACK: //received NACK, we are done
	case ST_LAST_DATA: //received ACK but we are done already
            //ACK for future responses
            SEND_ACK();
	    //slave stays in receiver state
            i2c_info.mode = READY;
	    break;
	//different other states
	case NO_RELEVANT_INFO://it is not possible but used here to jump between operations
	    break;
	case ILLEGAL_START_STOP://illegal start or stop signal,abort and return with error
	    i2c_info.errorCode = ILLEGAL_START_STOP;
	    i2c_info.mode = READY;
	    SEND_STOP();//macro -> send stop signal to the i2c hardware
	    break;
    }


}
