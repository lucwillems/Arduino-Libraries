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

#include <Arduino.h>
#include <HardwareAnalog.h>

HardwareAnalog::HardwareAnalog() {
  analogRef = DEFAULT;
  prescaler = PS_128;
  sampleCount = 1; 
  currentChannel=0;
}

void HardwareAnalog::setPrescaler(byte x) {
#ifdef SERIAL_DEBUG
  Serial.print("ADC prescaler=");
  Serial.println(x,HEX);
#endif
  prescaler = x & 0x07;
}

void HardwareAnalog::setReference(byte reference) {
#ifdef SERIAL_DEBUG
  Serial.print("ADC reference=");
  Serial.println(reference,HEX);
#endif
  analogRef=reference & 0x03;
}

void HardwareAnalog::setSamples(byte count) {
#ifdef SERIAL_DEBUG
  Serial.print("ADC samples=");
  Serial.println(count,HEX);
#endif
  if (count>0) {
     sampleCount=count;
  } else {
     sampleCount=1;
  }
}
  
void HardwareAnalog::initScan(byte n, const byte analogChannels[]) {
  nrOfChannels=min(MAX_CHANNELS,n);
#ifdef SERIAL_DEBUG
  Serial.println("ADC init scan ");
  Serial.println(nrOfChannels);
#endif
  for (int i=0; i < nrOfChannels; ++i) {
    values[i]=0;
    channels[i] = analogChannels[i];
    if (channels[i] >= ADC_ARDUINO_A0) {
      //When using A0,A1... calculate based on A0=0x00 ... A6=0x07 
      channels[i]=channels[i]-ADC_ARDUINO_A0;
    }
  } 
  ADCinit();
}

void HardwareAnalog::ADCinit() {
#ifdef SERIAL_DEBUG
  Serial.println("init ADC");
#endif
// set up the ADC
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  // you can choose a prescaler from above.
  // PS_16, PS_32, PS_64 or PS_128
  ADCSRA |= prescaler;    // set our own prescaler to 64 
  cbi(ADMUX, ADLAR); // Make sure the ADC value is right-justified.

  sbi(ADCSRA, ADEN);      // Enable the ADC.
  delay(1);
  sbi(ADCSRA, ADIE); // Enable ADC complete interrupts.
  
}

int HardwareAnalog::analogRead(byte channel) {
   int result=-1;
   if (channel < nrOfChannels) {
     noInterrupts();
     result=values[channel];
     interrupts();
  }
  return result;
}

#ifdef ADC_TIMING
unsigned long HardwareAnalog::analogTime(byte channel) {
  long result=-1;
  if (channel < nrOfChannels) {
     noInterrupts();
     result=times[channel];
     interrupts();
  }
  return result;
}

unsigned long HardwareAnalog::scanTime() {
  unsigned long time=0;
  noInterrupts();
  for (int i=0;i<nrOfChannels;i++) {
      time += times[i];
  }
  interrupts();
  return time;
}
#endif

boolean HardwareAnalog::startScan() {
  //is there a scan active, if so,we can't start a new one
	
  if (scanActive || lock) {
     scanRequested=true;
     return false;
  }
#ifdef SERIAL_DEBUG
  Serial.println("start scan");
#endif
  scanActive=true;
  currentChannel=0;
  currentSample=sampleCount;
  ADCstart(); //start the scan
  return true;
}

bool HardwareAnalog::isScanActive() {
  return scanActive;
}

void HardwareAnalog::ADCstart() {
   //Selecteer the channel
  byte c=channels[currentChannel];//calculate the channel
  byte mux=analogRef<<6 | c &0x0f;//default to channel 0;
  
#ifdef SERIAL_DEBUG
  Serial.print("ADC start: channel=");
  Serial.print(currentChannel);
  Serial.print(" mux=");
  Serial.println(mux,HEX);
#endif
   //set AD mux channel and reference
   ADMUX=mux;
   sbi(ADCSRA, ADSC); // Start the ADC conversion.
#ifdef ADC_TIMING
   startADCTime=micros();
#endif
}

void HardwareAnalog::idle() {
#ifdef SERIAL_IDLE_DEBUG
  Serial.print(".");
#endif
}

void HardwareAnalog::ADCinitInternal(byte mux) {
  ADMUX=mux; //select ref=1v1 and internal tempureture
  cbi(ADCSRA ,ADIE);  // DISABLE ADC complete interrupts,we will poll
  sbi(ADCSRA ,ADPS2); //set 128 prescale
  sbi(ADCSRA ,ADPS1);
  sbi(ADCSRA ,ADPS0);
  sbi(ADCSRA ,ADEN);  // Enable the ADC
  delay(10);
  sbi(ADCSRA ,ADSC);  // Start the ADC conversion
}

int HardwareAnalog::scanInternal(byte n,byte mux) {
  //we can't run scans and this together
  //we should have some timeout here ?
  lock=true;
  while(scanActive) { idle();}
  scanActive=1; //block any new scan until we are finished
  lock=false;
  ADCinitInternal(mux);
  //start manual scan
  byte samples=n;      // we take average of 200 samples
  long value=0;
  while (samples>0) {
     while(ADCSRA & (1 << ADSC)); // waits for the ADC to finish 
     samples--;
     value +=ADCW;  //read value    
  }
  ADCinit();
  scanActive=0;
  if (scanRequested) {
     lock=false;
     startScan();
  }
  return (int)(value/(long)n);
}

int HardwareAnalog::Temperature(byte n) {
  return scanInternal(n,0xC8);
}

int HardwareAnalog::internalReference() {
  return scanInternal(5,0xCE);
}

int HardwareAnalog::internalGND() {
  return scanInternal(5,0xCF);
}

void HardwareAnalog::setCallBack(scanFinishCallback callback) {
  pcallBack=callback;
}

void HardwareAnalog::ADC_ISR() {
#ifdef ADC_TIMING
  interruptCount++;
#endif
#ifdef SERIAL_DEBUG
  Serial.print("ADC ISR: channel=");
  Serial.print(currentChannel);
  Serial.print(" sample= ");
  Serial.println(currentSample);
#endif
  int value=ADCW; //Read ADC in word format
  if (--currentSample == 0) {
      #ifdef SERIAL_DEBUG
        Serial.println("ADC ISR: sample finished");
      #endif
      values[currentChannel]=value; //read ADC as word,take care of ADCL/ADCH order
#ifdef ADC_TIMING
      times[currentChannel]=micros()-startADCTime;
#endif
      //start next conversion if currentChannel < nrOfChannels;
      if (++currentChannel >= nrOfChannels) {
         #ifdef SERIAL_DEBUG
           Serial.print("ADC ISR: scan finished");
         #endif
         scanActive=false;
	 //callback if defined
	 if (pcallBack) {
	     #ifdef SERIAL_DEBUG
               Serial.print("callBack() ..");
             #endif
             (*pcallBack)(); //call it 
	     #ifdef SERIAL_DEBUG
               Serial.print("callBack() done");
             #endif
	 }
	 //scan is finished, don't start a new one
         return;
      } else {
	 //start new channel sampling
	 currentSample=sampleCount;
      }
  }
  //Start next channel scan
  ADCstart();
}

// Interrupt service function when ADC is finished
ISR(ADC_vect) {
   Analog.ADC_ISR();
}

//Singleton class to use with sketches
HardwareAnalog Analog;
