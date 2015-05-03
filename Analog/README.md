Analog
======
This library enables full async usage of ADC (analog to digital converter) on the Arduino UNO board.
it uses interrupt driven approach so scanning and reading of ADC in non blocking. 
Multible Analog channels can be scanned in 1 round.

Something about Arduino ADC Speed.
==================================
The speed of ADC is determant by multible factors
- prescaler factor of 1/128
- 13 ADC clock cycles for typical conversion, first conversion taks up to 23 ADC clock cycles

on a typical Arduino UNO with 16MHz clock , a ADC sample takes up to 
          
          0,0625 uSec * 128 * 13 = 104 uSec.

Arduino analogRead() works synchronized. the analogRead() call will block atleast 104 uSec before returning.
all this time, we can not do other stuff. 
Reading all 5 Analog channels will takeup more than 520 usec. This limits the reading speed of the ADC channels to 10000/sec for 1 channel to 2000/second for all channels. Ofcourse this would
not be praktical as we will not be able todo anything else.

This library splits the triggering of a ADC cycle and reading of result values. This leafs your program to do some other stuff will waiting on completion of the ADC cycle.

It is able to read all 5 ADC channels in sequence in 340 uSeconds using 128 prescaller.

Usage
=====
The library exports a single ```Analog``` class which can be used to access the analog channels.

See examples/Simple.ino for basic usage example
```C
constant byte nrOfChannels=5;
const byte channels[] = {A0, A1, A2, A3, A4, A5};

setup() {
  Analog.initScan(nrOfChannels,channels);
  Analog.startScan(); //start first scan
}

loop() {
   if (Analog.isScanFinished()) {
      //read the channels based on there index number
      int value=Analog.analogRead(0);
      //start new scan
      Analog.startScan();
   };
   //do other stuff...
}
```

Using callback function we can handle ADC conversion outside the loop() function.

```C
constant byte nrOfChannels=5;
const byte channels[] = {A0, A1, A2, A3, A4, A5};

void ADC() {
  //this function is called as parted of ADC ISR when all channels are scanned
  //keep it short, don't try to calculate PI here ;-)

  //read the channels based on there index number
  int value=Analog.analogRead(0);
  //start new scan
  Analog.startScan();
}

void setup() {
  Analog.initScan(nrOfChannels,channels);
  Analog.setCallBack(&ADC);
  Analog.startScan(); //start first scan
}

void loop() {
   //do other stuff...
}
```

Configuring the ADC process
===========================

following methods can be used to configure the ADC process.
- setPrescaler(PS_xxxx) : set the ADC by selecting a correct prescaling factor. values tha can be used are PS_128,PS_64,PS_32,PS_16,PS_8,PS_4,PS_2
- setReference(int reference) : set the ADC reference. see analogReference in arduino documentation
- setSamples(byte n) : set the number of successive samples we take. the last sample is stored and can be retrieved via analogRead() function.
 
Prescaling and resolution
=========================
Arduino software uses by default a prescaling of 128. with clock speed of 16MHZ, we will have a ADC clock of 16Mhz/128 = 125Khz.

The effective resolution of the ADC process is depending on speed of the ADC clock.

ADC clock speeds below 200 Khz, will give 10bit resolution. Higher speeds (prescaller = 64 or lower) reduces the resolution and effective bits.
Note that we will always read 10bits. it up to you to right shift the data during processing if needed.

more info see :

http://www.microsmart.co.za/technical/2014/03/01/advanced-arduino-adc/

License
=======
Copyright (c) T.M.M.  Luc Willems. All right reserved.
Arduino is copyright by Arduino LLC. All right reserved.

This library is released as LGPL v3.0

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


