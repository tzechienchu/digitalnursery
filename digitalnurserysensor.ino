
//SHT1X Soil Temperature and Humidity
#include <SHT1x.h>
#define dataPin  10     //Blue
#define clockPin 11     //Yellow
//AM2301 Air Temperature and Humidity
#include "DHT.h"
#define DHTPIN 2        // what pin we're connected to
#define DHTTYPE DHT21   // DHT 21 (AM2301)
//BMP085
#include "Barometer.h"
#include <Wire.h>
//CO2
#define CO2Pin A0
//Light Sensor I2C
#include <Digital_Light_TSL2561.h>

#define QUOTE Serial.write(34)
#define TABSIZE 8

float Stemp_c;
float Stemp_f;
float Shumidity;
float Atemp_c;
float Ahumidity;
float Btemperature;
float Bpressure;
float Batm;
float Baltitude;
int   CO2Level;
unsigned long  Lux;

Barometer myBarometer;
DHT dht(DHTPIN, DHTTYPE);
SHT1x sht1x(dataPin, clockPin);

#define BOARDID  "Sensory 1"
String serialCommand = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
unsigned long statetime,now;
int nowState;
int srate = 5;

void setup()
{
   Wire.begin(); 
   Serial.begin(115200); // Open serial connection to report values to host
   Serial.println(BOARDID);
   serialCommand.reserve(64);
   statetime = millis();
   nowState = 0;
   //myBarometer.init();
   //TSL2561.init(); 
   
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
  case 1://Period Sensor Reading
  {
    if ((now - statetime) >= (srate*1000)) {
      //readSensor();
      Serial.println("Read Sensor");
      statetime = now;
      nowState = 1;
    }
    break;
  }
  case 2://Send JSON to Serial Port
  {
    jsonOut();
    statetime = now;
    nowState = 1;
    break;
  }

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
    if (command.equals("HOST")){
      //Serial.println("Got it");
      nowState = 1;
    }
    if (command.equals("READ")){
      nowState = 2;
    }
    if (command.equals("RATE")){
      srate = parseArgument(5);
      nowState = 2;
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
//Json Related
void showIndent(int indent)
{
  for(int i=0;i<indent*TABSIZE;i++) Serial.print(" ");
}
void jsonSectionOut(int indent,char *key)
{
  showIndent(indent);
  QUOTE;Serial.print(key);QUOTE;Serial.println(": {"); 
}
void jsonSectionEndOut(int indent,int final)
{
  showIndent(indent);
  Serial.print("}"); 
  if (final == 0) {
    Serial.println(",");
  } else {
    Serial.println();
  }
}
void jsonKVOut(int indent,char *key,float value,int final)
{
  showIndent(indent);
  QUOTE;Serial.print(key);QUOTE;Serial.print(": "); 
  Serial.print(value, DEC);
  if (final == 0) {
    Serial.println(",");
  } else {
    Serial.println();
  }
}
void jsonOut()
{
  Serial.print("{");QUOTE;Serial.print(BOARDID);QUOTE;Serial.println(": {");
    jsonSectionOut(1,"Soil");
      jsonKVOut(2,"Temperature",Stemp_c,0);
      jsonKVOut(2,"Humidity",Shumidity,1);
    jsonSectionEndOut(1,0);
    jsonSectionOut(1,"Air");
      jsonKVOut(2,"Temperature",Atemp_c,0);
      jsonKVOut(2,"Humidity",Ahumidity,1);
    jsonSectionEndOut(1,0);
    jsonSectionOut(1,"Barometer");
      jsonKVOut(2,"Temperature",Btemperature,0);
      jsonKVOut(2,"Pressure",Bpressure,1);
    jsonSectionEndOut(1,0);
    jsonSectionOut(1,"Other");
      jsonKVOut(2,"CO2",CO2Level,0);
      jsonKVOut(2,"Lux",Lux,1);
    jsonSectionEndOut(1,1);    
  Serial.println("}}");
}

void readSensor()
{
  // Read values from the sensor
  Stemp_c = sht1x.readTemperatureC();
  Stemp_f = sht1x.readTemperatureF();
  Shumidity = sht1x.readHumidity();
  
  Ahumidity = dht.readHumidity();
  Atemp_c = dht.readTemperature();

  Btemperature = myBarometer.bmp085GetTemperature(myBarometer.bmp085ReadUT()); //Get the temperature, bmp085ReadUT MUST be called first
  Bpressure = myBarometer.bmp085GetPressure(myBarometer.bmp085ReadUP());//Get the temperature
  Baltitude = myBarometer.calcAltitude(Bpressure); //Uncompensated caculation - in Meters 
  Batm = Bpressure / 101325;  
  
  CO2Level = analogRead(CO2Pin);   
  TSL2561.getLux();
  Lux=TSL2561.calculateLux(0,0,1);
}

void debugOut()
{
  // Print the values to the serial port
  Serial.print("Temperature: ");
  Serial.print(Stemp_c, DEC);
  Serial.print("C / ");
  Serial.print(Stemp_f, DEC);
  Serial.print("F. Humidity: ");
  Serial.print(Shumidity);
  Serial.println("%");
  
  Serial.print("Temperature: ");
  Serial.print(Atemp_c, DEC);
  Serial.print("C. Humidity: ");
  Serial.print(Ahumidity);
  Serial.println("%");
  
  Serial.print("Temperature: ");
  Serial.print(Btemperature, 2); //display 2 decimal places
  Serial.println("deg C");

  Serial.print("Pressure: ");
  Serial.print(Bpressure, 0); //whole number only.
  Serial.println(" Pa");
  Serial.print("Ralated Atmosphere: ");
  Serial.println(Batm, 4); //display 4 decimal places
  Serial.print("Altitude: ");
  Serial.print(Baltitude, 2); //display 2 decimal places
  Serial.println(" m");

  Serial.print("CO2 = " );                       
  Serial.print(CO2Level);     
  Serial.println();
  Serial.print("Lux = ");
  Serial.println(Lux);
  
  Serial.println();
}
