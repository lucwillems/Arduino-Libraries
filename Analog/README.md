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
