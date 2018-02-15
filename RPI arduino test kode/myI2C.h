#ifndef MY_I2C__H__
#define MY_I2C__H__
// TWI bit rate
#define FREQ 100000L
#define F_CPU 16000000L   // in Hz  

// Get TWI status
#define STATUS	(TWSR & 0xF8) 

// Transmit buffer length
#define TX_BUFFER_LENGHT 20
// Receive buffer length
#define RX_BUFLEN_LENGHT 20
// Global transmit buffer
uint8_t transmitBuffer[TX_BUFFER_LENGHT];
// Global receive buffer
volatile uint8_t receiveBuffer[TX_BUFFER_LENGHT];
// Buffer indexes
volatile int bufferIndexTX; // Index of the transmit buffer. Is volatile, can change at any time.
int bufferIndexRX; // Current index in the receive buffer
// Buffer lengths
int bufferLenghtTX; // The total length of the transmit buffer
int bufferLenghtRX; // The total number of bytes to read (should be less than  RX_BUFLEN_LENGHT)

typedef enum {
  READY,
  INITIALIZING,
  REPEATED_START_SENT,
  MASTER_TRANSMITTER,
  MASTER_RECEIVER,
  SLAVE_TRANSMITTER,
  SLAVE_RECEIVER
}TWI_Mode;

typedef struct {
  TWI_Mode mode;
  uint8_t errorCode;
  uint8_t repStart;
}TWI_Status;
TWI_Status i2c_info;

//TWI control Macros to control the TWI hardware and 
// for setting the control register TWCR 
#define SEND_START() (TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWIE) )
#define SEND_STOP() (TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)|(1<<TWIE) )
#define SEND_TRANSMIT() (TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE) )
#define SEND_ACK() (TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE) )
#define SEND_NACK() (TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE) )

//Status codes
#define START_SENT      0x08 //start was sent 
#define REP_START_SENT  0x10 //repeat start was sent
//Master Transmitter Mode
#define MT_SLAW_ACK     0x18 //SLA+W sent and ACK received
#define MT_SLAW_NACK    0x20 //SLA+W sent and NACK received
#define MT_DATA_ACK     0x28 //Data sent and ACK
#define MT_DATA_NACK    0x30 //Data sent and NACK 
//Master Receiver  Mode
#define MR_SLAR_ACK     0x40 //SLA+R sent and ACK received
#define MR_SLAR_NACK    0x48 //SLA+R sent and NACK received
#define MR_DATA_ACK     0x50 //Data received and ACK
#define MR_DATA_NACK    0x58 //Data received and NACK 
//Slave Receiver mode
#define SR_SLA_ACK           0x60 //SLA+W was addressed and ACk was returned
#define SR_GCALL_ACK          0x70 //addressed generally and ACK was returned
#define SR_ARB_LOST_SLA_ACK   0x68 //arbitration lost in SCL+R/W and own SLA+W addressed and ACK returned
#define SR_ARB_LOST_GCALL_ACK 0x78 //arbitration lost in SLA+R/W amd generral call was received and ACK returned

#define SR_DATA_ACK       0x80 //data received and ACK returned
#define SR_GCALL_DATA_ACK 0x90 //general call, data received and ACK returned
#define SR_STOP           0xA0 //stop or repeted start condition received

#define SR_DATA_NACK       0x88 //Data received and NACK returned
#define SR_GCALL_DATA_NACK 0x98 //Data received from general call and NACK returned

//Slave Transmitter
#define ST_SLA_ACK          0xA8 //SLA+R (master whats to read) addressed , Ack returned
#define ST_ARB_LOST_SLA_ACK 0xB0 //someone lost arbitration, SLA+R addressed, ACK returned

#define ST_DATA_ACK 0xB8 //one data byte was transmitted and got ACK from the receiver 

#define ST_DATA_NACK 0xC0 //one data byte was transmitted and got NACK from the receiver(transmission is over)
#define ST_LAST_DATA 0xC8 //last data byte was transmitted and ACK was received

//function declarations
uint8_t transmitData(void * const dataToTransmit, uint8_t size, uint8_t isRepeatedStart);
void init(void);
uint8_t readData(uint8_t address, uint8_t bytesToread, uint8_t isRepeatedStart);
uint8_t isReady(void);

//different other states
#define LOST_ARBIT         0x38 //arbitration is lost
#define NO_RELEVANT_INFO   0xF8 //
#define ILLEGAL_START_STOP 0x00 //illegal start or stop has been detected
#define SUCCESS            0xFF //Successful transfer, this state is impossible from TWSR as bit2 is 0 and read only

//Master as Transmitter 
#define TX_MAX_BUFFER_LENGHT 14
uint8_t TWITransmitBuffer[TX_MAX_BUFFER_LENGHT];
volatile int TXBufferIndex = 0;
int TXBufferLenght = 0;

//Master as Receiver 
#define RX_MAX_BUFFER_LENGHT 14
volatile uint8_t TWIReceiveBuffer[RX_MAX_BUFFER_LENGHT];
int RXBufferIndex = 0;
int RXBufferLenght = 0;

#endif //MY_I2C__H__
