#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 40, d5 = 41, d6 = 42, d7 = 43;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
 
/////////////PIN CONFIG////////////
#define PIN 8
#define PZ_PIN 37
 
#define VRX1 A1
#define VRY1 A2
#define Switch1 2
 
#define VRX2 A3
#define VRY2 A4
#define Switch2 3
////////////////////////////////////

////////////TONE MACROS/////////////
#define ERR_SOUND tone(PZ_PIN, 100, 75);
#define PRESS_SOUND 
#define WIN_SOUND
#define DRAW_SOUND
#define PUT_SOUND tone(PZ_PIN, 2200, 100);


//bitSet(TIMSK5, TOIE5);  //sets "TOIEn" in register "TIMSKn" to one. This bit enables timer overflow interrupts.

//    Check timer 5 registres
/*
 *   Serial.print("TCCR5A:");
 *   Serial.println(TCCR5A);
 *   Serial.print("TCCR5B:");
 *   Serial.println(TCCR5B);
 *   Serial.print("TCCR5C:");
 *   Serial.println(TCCR5C);
 *   Serial.print("OCR5AH:");
 *   Serial.println(OCR5AH);
 *   Serial.print("OCR5AL:");
 *   Serial.println(OCR5AL);
 *   Serial.print("OCR5BH:");
 *   Serial.println(OCR5BH);
 *   Serial.print("OCR5BL:");
 *   Serial.println(OCR5BL);
 */
 
//ISR for timer overflow:
/*
  ISR(TIMER5_OVF_vect){
    cli();
    OCR3A = sigTable[mode][i]; //PWM on pin 5 uses OCR3A as trigger value, set it to defind value
  
    i+=1;
    if (i >= SAMPLESIZE - 1){
       i = 0;
     }//end if
    sei();
  }//end ISR
*/
/////////////SD CARD////////////////
#include <SD.h>
#define SD_PIN 53

//#define DEBUG_writeWinnerToSD //0.53s runtime extra

struct dataStructure {
  char points[3];
  char space = ' ';
  char nameSpace[6];
  char newlineC = '\n';
};



////////////COLOR CONFIG////////////
 
#define PLAYER_1_COLOR matrix.Color(0,0,255)
#define PLAYER_2_COLOR matrix.Color(0,255,0)
#define PLAYER_3_COLOR matrix.Color(150,150,150)
#define CURSOR_1_COLOR matrix.Color(255,0,255)
#define CURSOR_2_COLOR matrix.Color(255,255,0)
#define GRID_COLOR matrix.Color(150,150,150)
////////////////////////////////////
#define MENU_DELAY 250
#define GAME_DELAY 200

#define NAME_LENGTH 5
#define menuSize 2
#define SCORE_LENGTH 3
#define LINE_LENGTH (SCORE_LENGTH+NAME_LENGTH+1)
#define MAX_SCORE 999 // This can only be the number of digits defined by SCORE_LENGTH

#define PLAYER1 false
#define PLAYER2 true

#define NM

#define SLAVE_ADDRESS 0x04

#define RPI_cp 22
//#define DEBUG_sendData
#define DEBUG_timing



///////Macros and variables///////// 
 
#define CURSOR_POS_IN_AREA gameArea[cursorPos[1]][cursorPos[0]]

char gameArea[3][3] = {{ 0,0,0 },
                       { 0,0,0 },
                       { 0,0,0 }};
                       // 1'taller er player 1 og 0 er tom        
 
boolean turn = PLAYER1; //FALSE eller 0 er player 1
boolean inMenu = false; // False if the game is started and 1 if the statemachine is in the menu

char cursorPos[2] = {0,0};
char Move[2] = {0,0};
char latestPut[2] = {0,0};
char turns = 0;
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned int interval = 0;           // interval at which to blink (milliseconds)
 
volatile boolean button1_pressed_ISR = false;
volatile boolean button2_pressed_ISR = false;
 
boolean button1_detected = false;
boolean button2_detected = false;
 
enum gamestates {start, going, wonP1, wonP2, draw};
gamestates gamestate = start;


enum menustates {start_menu, running_menu, chnm_menu, game_menu};
menustates menustate = start_menu;
menustates previousMenustate = start_menu; 
enum players {p1 = 0, p2 = 1};
players player; 


char player1Name[NAME_LENGTH] = {'M','A','D','S',' '}; 
char player2Name[NAME_LENGTH] = {'M','A','R','K',' '};

volatile boolean sendingDataToRPI = false;
boolean doneSending = false;
 
///////Functions/////////
extern void MoveInArray();
extern bool gameAreaFiled(char xPos, char yPos);
extern bool outOfBounds(char xPos, char yPos);
extern void animate();
extern void drawPiece(char posX, char posY, int color);

//new matrix functions
extern void animateNM();
extern void drawPieceNM(char posX, char posY, int color);
extern void drawGridNM();
 
extern void detectinput(bool turn);
extern void detectput(bool turn);
extern boolean find_new_pos();
extern void Turndecide();
extern void detectGameCondition();
extern void p1win();
extern void p2win();
extern void drawWin();
extern void rungame();
extern void playerTurnLCD(boolean playerTurn);
extern void resetgame();
extern void inputName(bool chnmPlayer);

//SD card func
extern void serialPrintArr(char* arr, int len);
extern void writeWinnerToSD(boolean winner);
 
// Debug func
extern void updateD();
extern void printNamesSerial();

// Debug macros
//#define DEBUG_detectGameCondition
//#define DEBUG_Turndecide
//#define DEBUG_MoveInArray
//#define DEBUG_animate
//#define DEBUG_detectinput
//#define DEBUG_find_new_pos
//#define DEBUG_detectput
//#define DEBUG_winner
 
////////////////////////////////////
// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
 
 
// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, PIN,
                                               NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
                                               NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                                               NEO_GRB            + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  /////////////////////////////////////
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  /////////////////////////////
  Serial.begin(9600);
  /////////////////////////////////////////
  while (!Serial) {};
  if (!SD.begin(SD_PIN)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  //////////////////////////////////////////

  
  matrix.begin();
  matrix.setBrightness(40);
  matrix.fillScreen(0);
  matrix.show();
 
  pinMode(VRX1, INPUT);
  pinMode(VRY1, INPUT);
  pinMode(Switch1, INPUT);
  digitalWrite(Switch1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Switch1),ISR_button1,FALLING);
 
  pinMode(VRX2, INPUT);
  pinMode(VRY2, INPUT);
  pinMode(Switch2, INPUT);  
  digitalWrite(Switch2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Switch2),ISR_button2,FALLING);

  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(sendData);
  pinMode(RPI_cp, OUTPUT);
  
}
 
void loop() {
  unsigned long currentMillis = millis();


  switch(menustate){
      // {start_menu, running_menu, chnm_menu, game_menu};
    case start_menu:
                    static char toggle_menu = 0;
                    inMenu = true; // The ISR of button1 now know that the player1 is controlling the menu 
                    //turn = 0;
                    button1_pressed_ISR = 0;
                    lcd.setCursor(0, 0);
                    lcd.print("Welcome...            ");
                    lcd.setCursor(0,1);
                    lcd.print("Play Tic Tac Toe");
                    toggle_menu = 0; // The menu is now on "Play Tic Tac Toe"
                    menustate = running_menu; // Go to running_menu
                    break;

    case running_menu:
                      lcd.setCursor(0, 1);
                      detectinput(PLAYER1);
                 
                      //move in menu
                      if(Move[0] > 0){
                         
                        toggle_menu++;      //move right in menu
                        if(toggle_menu == menuSize){toggle_menu=0;};  //wrap around up
                       
                      } else if (Move[0] < 0){
                        
                        toggle_menu--;      //move left in menu
                        if(toggle_menu == -1){toggle_menu = menuSize-1;}; //wrap around down
 
                      }//end if move left or right
 
                      //update menu display if changed
                      if (Move[0] != 0){
                          if(toggle_menu == 0){
                            lcd.print("Play Tic Tac Toe ");
                          }else if(toggle_menu == 1){
                            lcd.print("Change name      ");
                          }
                       } // if Moved                      

                       //update if the button is pressed
                       if(button1_pressed_ISR){
                          if(toggle_menu == 0){ // Go to Game
                            menustate = game_menu; // Go to the game_menu next
                            
                          }else if (toggle_menu == 1){ // Go to "Change name"
                            menustate = chnm_menu;
                          }
                          button1_pressed_ISR = false;
                       }
                    if(menustate != running_menu){
                        inMenu = false; // The Menu StateMachine is now changing to another menu than "start_menu"
                    }
                    break;
                    
    case chnm_menu:
                    inputName(PLAYER1); // player1
                    inputName(PLAYER2); // player2
                    menustate = start_menu;
                    break;

    case game_menu:
      //{start, going, wonP1, wonP2, draw};
                    rungame();
      
                    break; // Break for the game state in the menu SM
  } // Switch/Menu State Machine   


  //if menustate chagnges, change delay
  if (previousMenustate != menustate){
    if( menustate ==  running_menu){
      interval = MENU_DELAY ;           //if in menu, apply menu delay
    } else if (menustate == game_menu){
      interval = GAME_DELAY;            //if in game, apply game delay
    }
    previousMenustate = menustate;
  }//end if - previousMenustate

  //while this runtime for the program is less than the inteval
  while(currentMillis - previousMillis <= interval) {
    currentMillis = millis();
    //Serial.println(currnetMillis);
    
    //Here is the place to add sender code
  }
  
  currentMillis = millis();             //update the current time, if we did not enter while loop
  previousMillis = currentMillis;       //update previous time for next compare 
  
  #ifdef DEBUG_timing
  Serial.print("Ude, det tog:");
  Serial.println(currentMillis - previousMillis);
  #endif
  
  
    
} // void loop
  
////////////////////////////////////////////////////
// ISR til knapperne
void ISR_button1(){
   static unsigned long last_interrupt_time = 0;
   unsigned long interrupt_time = millis();
   // If interrupts come faster than 200ms, assume it's a bounce and ignore
   if (interrupt_time - last_interrupt_time > 200) 
   {
      if(turn != true || inMenu){ // If it's the player 1's turn or the Menu StateMachine is in Menu.
        button1_pressed_ISR = true;
      }  
   }
   last_interrupt_time = interrupt_time;
}      

 
void ISR_button2(){
   static unsigned long last_interrupt_time = 0;
   unsigned long interrupt_time = millis();
   // If interrupts come faster than 200ms, assume it's a bounce and ignore
   if (interrupt_time - last_interrupt_time > 200) 
   {
       if(turn){
          button2_pressed_ISR = true;
       }
   }
   last_interrupt_time = interrupt_time;
}    
