#ifndef ADC_h
#define ADC_h

#include "Arduino.h"

//#define SERIAL_DEBUG 

// Define various ADC prescaler
const byte PS_2  =  (1 << ADPS0);
const byte PS_4  =  (1 << ADPS1);
const byte PS_8  =  (1 << ADPS1) | (1 << ADPS0);
const byte PS_16  = (1 << ADPS2);
const byte PS_32  = (1 << ADPS2) | (1 << ADPS0);
const byte PS_64  = (1 << ADPS2) | (1 << ADPS1);
const byte PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

const byte MAX_CHANNELS = 5;
const byte ADC_A0=0x00;
const byte ADC_A1=0x01;
const byte ADC_A2=0x02;
const byte ADC_A3=0x03;
const byte ADC_A4=0x04;
const byte ADC_A5=0x05;
const byte ADC_A6=0x06;
const byte ADC_A7=0x07;

const byte ADC_ARDUINO_A0=14;
const byte ADC_ARDUINO_A1=15;
const byte ADC_ARDUINO_A2=16;
const byte ADC_ARDUINO_A3=17;
const byte ADC_ARDUINO_A4=18;
const byte ADC_ARDUINO_A5=19;
const byte ADC_ARDUINO_A6=20;
const byte ADC_ARDUINO_A7=22;

//const byte ADC_MAX=0x1F; //max channels

class HardwareAnalog {

 private:
  //settings
  byte analogRef;
  byte prescaler;
  byte sampleCount;  

  //status counters/flags  
  unsigned long scanCounter;
  bool scanActive;

  unsigned long startADCTime;
  
  volatile int values[MAX_CHANNELS];
  volatile byte channels[MAX_CHANNELS];
  volatile unsigned long times[MAX_CHANNELS];
  volatile byte currentChannel;
  volatile byte currentSample;
  volatile byte nrOfChannels;

  void ADCinit();
  inline void ADCstart();

  void ADCinitInternal(byte mux);
  int  scanInternal(byte n,byte mux);
  
public:
 
  void setPrescaler(byte x);
  void setReference(byte reference);
  void setSamples(byte count);
  
  int analogRead(byte channel);
  int Temperature(byte samples);
  int internalReference();
  int internalGND();
  
  unsigned long analogTime(byte channel);
  unsigned long scanTime();
  unsigned long Count();

  void idle();
  
  void initScan(byte n, const byte channels[]);
  bool startScan();
  bool isScanActive();
  
  inline HardwareAnalog();
  inline void ADC_ISR();
  
};

extern HardwareAnalog Analog;  



#endif 