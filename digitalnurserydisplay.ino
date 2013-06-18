#define LCD_CS A3    
#define LCD_CD A2    
#define LCD_WR A1   
#define LCD_RD A0    
// you can also just connect RESET to the arduino RESET pin
#define LCD_RESET A4
//Duemilanove/Diecimila/UNO/etc ('168 and '328 chips) microcontoller:
// Color definitions
#define BLACK           0x0000
#define  BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0 
#define WHITE           0xFFFF

#define lable1 "TMP1"
#define lable2 "HUM1"
#define lable3 "PAR1"
#define lable4 "TMP2"
#define lable5 "HUM2"
#define lable6 "PAR2"

#include "TFTLCD.h"
#define BOARDID  "TFTLCD"

String serialCommand = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
unsigned long statetime,now;
int nowState;
int OP1,OP2,OP3;
int min1,min2,min3,min4,min5,min6;
int max1,max2,max3,max4,max5,max6;
int pt1,pt2,pt3,pt4,pt5,pt6;
float showOP1,showOP2,showOP3;

TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup(void) {
  Serial.begin(115200);
  Serial.println(BOARDID);
  serialCommand.reserve(64);
  statetime = millis();
  nowState = 0;
  
  tft.reset();
  tft.initDisplay();
  
  delay(500);
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.setRotation(1);
 
  tft.println("Digital");
  tft.println("Nursery"); 
  min1=min2=min3=min4=min5=min6 = 0;
  max1=max2=max3=max4=max5=max6 = 1024;
  pt1=pt2=pt3=pt4=pt5=pt6=1;
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
  case 1://Clear Screen
  {
     tft.fillScreen(BLACK);
     tft.setCursor(0, 0);
     statetime = now;
     nowState = 9;
  }
  break;
  case 2://Show 1
  {
     //tft.fillScreen(BLACK);
     tft.setCursor(0, 0);
     tft.setTextSize(4);
     tft.print(lable1);tft.print(" = ");showNum(min1,max1,pt1,OP1);
     tft.print(lable2);tft.print(" = ");showNum(min2,max2,pt2,OP2);
     tft.print(lable3);tft.print(" = ");showNum(min3,max3,pt3,OP3);
     statetime = now;
     nowState = 9;
  }
  break;
  case 3://Show 2
  {
     //tft.fillScreen(BLACK);
     tft.setCursor(0, 120);
     tft.setTextSize(4);
     tft.print(lable4);tft.print(" = ");showNum(min4,max4,pt4,OP1);
     tft.print(lable5);tft.print(" = ");showNum(min5,max5,pt5,OP2);
     tft.print(lable6);tft.print(" = ");showNum(min6,max6,pt6,OP3);
     statetime = now;
     nowState = 9;
  }
  break;  
  case 9:
  break;  
  }
}

long getdec(int pt)
{
  switch (pt) {
    case 0: return 1;
    case 1: return 10;
    case 2: return 100;
    case 3: return 1000;
    default:return 1;
  }
}
void showNum(int minv,int maxv,int pt,int value)
{
  tft.setTextColor(GREEN);
  if (value >= maxv) tft.setTextColor(RED);
  if (value <= minv) tft.setTextColor(BLUE);
  int v1 = value/getdec(pt);
  int v2 = value%getdec(pt);
  //Serial.println(value);
  //Serial.println(v1);
  //Serial.println(v2);
  tft.print(v1);tft.print('.');tft.println(v2);
  
  tft.setTextColor(GREEN);
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
      nowState = 1;
    }
    if (command.equals("CLRS")) {
      nowState = 1;
    }
    if (command.equals("SHW1")) {
      nowState = 2;
      OP1 = parseArgument(5);
      OP2 = parseArgument(10);
      OP3 = parseArgument(15);
    }
    if (command.equals("SHW2")) {
      nowState = 3;
      OP1 = parseArgument(5);
      OP2 = parseArgument(10);
      OP3 = parseArgument(15);
    }   
    if (command.equals("MIN1")) {
      nowState = 9;
      min1 = parseArgument(5);
      min2 = parseArgument(10);
      min3 = parseArgument(15);
    }    
    if (command.equals("MIN2")) {
      nowState = 9;
      min4 = parseArgument(5);
      min5 = parseArgument(10);
      min6 = parseArgument(15);
    }  
    if (command.equals("MAX1")) {
      nowState = 9;
      max1 = parseArgument(5);
      max2 = parseArgument(10);
      max3 = parseArgument(15);
    }    
    if (command.equals("MAX2")) {
      nowState = 9;
      max4 = parseArgument(5);
      max5 = parseArgument(10);
      max6 = parseArgument(15);
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
