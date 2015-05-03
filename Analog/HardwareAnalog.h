/*
 * Copyright 2015 Luc Willems (T.M.M.)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ADC_h
#define ADC_h

#include "Arduino.h"

//#define ADC_TIMING
//#define SERIAL_DEBUG 
#define SERIAL_IDLE_DEBUG

// Define cbi() and sbi() for clearing and setting bits in the
// ADC registers.
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// Define various ADC prescaler
const byte PS_2  =  (1 << ADPS0);
const byte PS_4  =  (1 << ADPS1);
const byte PS_8  =  (1 << ADPS1) | (1 << ADPS0);
const byte PS_16  = (1 << ADPS2);
const byte PS_32  = (1 << ADPS2) | (1 << ADPS0);
const byte PS_64  = (1 << ADPS2) | (1 << ADPS1);
const byte PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

const byte MAX_CHANNELS = 6;
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

// CallBack function called when all channels are finished
typedef void (*scanFinishCallback) ();

class HardwareAnalog {

 private:
  //settings
  byte analogRef;
  byte prescaler;
  byte sampleCount;  

  //status flag  
  bool scanActive;
  bool lock;
  bool scanRequested;
  
  scanFinishCallback pcallBack;
  
  unsigned int values[MAX_CHANNELS];
  byte channels[MAX_CHANNELS];

#ifdef ADC_TIMING
  unsigned long startADCTime;
  unsigned long times[MAX_CHANNELS];
#endif

  byte currentChannel;
  byte currentSample;
  byte nrOfChannels;

  void ADCinit();
  inline void ADCstart();

  void ADCinitInternal(byte mux);
  int  scanInternal(byte n,byte mux);
  
public:
 
  void setPrescaler(byte x);
  void setReference(byte reference);
  void setSamples(byte count);
  void setCallBack(scanFinishCallback callback);
  
  int analogRead(byte channel);
  int Temperature(byte samples);
  int internalReference();
  int internalGND();
  
#ifdef ADC_TIMING
  unsigned long analogTime(byte channel);
  unsigned long scanTime();
  unsigned long interruptCount;
#endif

  void idle();
  
  void initScan(byte n, const byte channels[]);
  bool startScan();
  inline bool isScanActive();
  
  inline HardwareAnalog();
  inline void ADC_ISR();
  
};

extern HardwareAnalog Analog;  



#endif 