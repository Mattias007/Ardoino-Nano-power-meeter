#include <Arduino.h>

float R4 = 11;  //Ratios between voltage dividers 
float R3 = 7.8;
float R2 = 5.5;
float R1 = 3.6;

float B1Voltage=0;  //Battery Voltager
float B2Voltage=0;
float B3Voltage=0;
float B4Voltage=0;
float RealBat1V = 0; //converted signal in 5v domain
float RealBat2V = 0;
float RealBat3V = 0;
float RealBat4V = 0;
float VolBat1 = 0;  //Analog channel raw value
float VolBat2 = 0; 
float VolBat3 = 0; 
float VolBat4 = 0; 

long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
     ADMUX = _BV(MUX5) | _BV(MUX0) ;
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
 
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
 
  long result = (high<<8) | low;
 
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}


void BatteryVoltage(){

float vccValue=readVcc()/1000.0;  //Arduino refence voltage

VolBat4 = analogRead(A0);    // Reading battery voltages
VolBat3 = analogRead(A1);    
VolBat2 = analogRead(A2);    
VolBat1 = analogRead(A3);    
    
  RealBat1V = VolBat1 * vccValue/1024.0;  //Converting raw value in 5v domian
  RealBat2V = VolBat2 * vccValue/1024.0;
  RealBat3V = VolBat3 * vccValue/1024;
  RealBat4V = VolBat4 * vccValue/1024;


  B4Voltage = RealBat4V * R4;     //Calculating actual voltages
  B3Voltage = RealBat3V * R3 - B4Voltage;    
  B2Voltage = RealBat2V * R2 - B4Voltage - B3Voltage;
  B1Voltage = RealBat1V * R1 - B4Voltage - B3Voltage - B2Voltage;
}


