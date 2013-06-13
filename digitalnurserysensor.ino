
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
  
Barometer myBarometer;
DHT dht(DHTPIN, DHTTYPE);
SHT1x sht1x(dataPin, clockPin);


void setup()
{
   Serial.begin(38400); // Open serial connection to report values to host
   Serial.println("Starting up");
   myBarometer.init();
}

void loop()
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
  
  debugOut();
  
  delay(2000);
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
  
  Serial.println();
}
