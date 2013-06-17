#include <WString.h>
#define BOARDID  "Actuator 1"

String serialCommand = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
unsigned long statetime,now;
int nowState;
int OP1,OP2,OP3,OP4;

// motor A
int dir1PinA = 13;
int dir2PinA = 12;
int speedPinA = 10;

// motor B
// motor A
int dir1PinB = 11;
int dir2PinB = 8;
int speedPinB = 9;

//LED Driver Controller Pin
#include "RGBdriver.h"
#define CLK 2//pins definitions for the driver        
#define DIO 3
RGBdriver Driver(CLK,DIO);
int LEDA1,LEDA2,LEDA3;
int LEDB1,LEDB2,LEDB3;

void setup()
{
   motorInit();
   Serial.begin(115200); // Open serial connection to report values to host
   Serial.println(BOARDID);
   serialCommand.reserve(64);
   statetime = millis();
   nowState = 0;
   LEDA1 = 0;
   LEDA2 = 0;
   LEDA3 = 0;
   LEDB1 = 0;
   LEDB2 = 0;
   LEDB3 = 0;
   
}

void loop()
{
  now = millis();
  parseCommand();
  switch(nowState) {
  case 0: //Wait HandShake
  {
    if ((now - statetime) >= 5000) {
      Serial.println(BOARDID);
      statetime = now;
      nowState = 0;
    }
  }
  break;
  case 1://Set LED
  {
      //readSensor();
      Serial.println("Set LED");
      showLED(OP1);
      Serial.println(OP1);
      Serial.println(OP2);
      Serial.println(OP3);
      statetime = now;
      nowState = 9;
      break;
  }
  case 5://Turn On Pump
  {
      if (OP1 == 1) {
        if (OP2 == 1) {
          Serial.println("Turn On Pump 1");
          pumpOn(1);
        } else {
          Serial.println("Turn Off Pump 1");
          pumpOff(1);
        }
      }
      if (OP1 == 2) {
        if (OP2 == 1) {
          Serial.println("Turn On Pump 2");
          pumpOn(2);
        } else {
          Serial.println("Turn Off Pump 2");
          pumpOff(2);
        }
      }
      statetime = now;
      nowState = 9;
    break;
  }
  case 9:
  break;
  }
}
//Serial Command
/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() 
{
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    serialCommand += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}

void parseCommand() 
{
  if (stringComplete) {
    Serial.println(serialCommand); 
    String command=serialCommand.substring(0,4);
    //Serial.println(command); 
    if (command.equals("HOST")) {
      //Serial.println("Got it");
      nowState = 9;
    }
    if (command.equals("LEDA")) {
      //Serial.println("Set LED A");
      nowState = 1;
      LEDA1 = parseArgument(5);
      LEDA2 = parseArgument(10);
      LEDA3 = parseArgument(15);
    }
    if (command.equals("LEDB")) {
      Serial.println("Set LED B");
      nowState = 2;
      LEDB1 = parseArgument(5);
      LEDB2 = parseArgument(10);
      LEDB3 = parseArgument(15);
    }    
    if (command.equals("PUMP")) {
      Serial.println("Set Pump");
      nowState = 5;
      OP1 = parseArgument(5);
      OP2 = parseArgument(10);
    }
    serialCommand = "";
    stringComplete = false;    
  }
}

int parseArgument(int argOffset){
  int ret = 0;
  String arg = serialCommand.substring(argOffset, argOffset + 4);
  //Serial.println(arg);
  if(isNumeric(arg.charAt(3))){
    ret = arg.charAt(3) - 48;
    if(isNumeric(arg.charAt(2))){
      ret = ret + (10 * (arg.charAt(2) - 48));
      if(isNumeric(arg.charAt(1))){
        ret = ret + (100 * (arg.charAt(1) - 48));
        if(arg.charAt(0) == '-'){
          ret = -ret;
        } else {
          if(isNumeric(arg.charAt(0))){
            ret = ret + (1000 * (arg.charAt(0) - 48));
          }
        }
      }
    }
  }
  return ret;
}

boolean isNumeric(char character){
  boolean ret = false;
  if(character >= 48 && character <= 75){
    ret = true;
  }
  return true;
}

String getArgument(int argOffset){
  return serialCommand.substring(argOffset, argOffset + 4);
}

void motorInit()
{
    pinMode(dir1PinA, OUTPUT);
    pinMode(dir2PinA, OUTPUT);
    pinMode(speedPinA, OUTPUT);
    pinMode(dir1PinB, OUTPUT);
    pinMode(dir2PinB, OUTPUT);
    pinMode(speedPinB, OUTPUT);
    
  //No Reverse
    digitalWrite(dir1PinA, LOW);
    digitalWrite(dir2PinA, HIGH);
    
    digitalWrite(dir1PinB, LOW);
    digitalWrite(dir2PinB, HIGH);  
    
    analogWrite(speedPinA, 0);
    analogWrite(speedPinB, 0);
}

void pumpOff(int id)
{
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
    
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);  
    
  if (id == 1) {
    analogWrite(speedPinA, 0);
  }
  if (id == 2) {
    analogWrite(speedPinB, 0);
  }  
}

void pumpOn(int id)
{
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
    
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);  
    
  if (id == 1) {
    analogWrite(speedPinA, 128);
  }
  if (id == 2) {
    analogWrite(speedPinB, 128);
  }  
}

void showLEDAll()
{
  Driver.begin(); // begin
  Driver.SetColor(LEDA1, LEDA2, LEDA3); 
  Driver.SetColor(LEDB1, LEDB2, LEDB3); 
  Driver.end();  
}
//Use Motor Shield
void showLED(int brt)
{
  brt = max(brt,0);
  brt = min(brt,255);
  digitalWrite(dir1PinA, LOW);
  digitalWrite(dir2PinA, HIGH);
    
  digitalWrite(dir1PinB, LOW);
  digitalWrite(dir2PinB, HIGH);  
  
  analogWrite(speedPinB, brt);
}
