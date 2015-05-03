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

Arduino analogRead() works synchronized. 1 read will block atleast 104 uSec before returning.
all this time, we could do other stuff. reading all 5 Analog channels will takeup more than 520 usec.
we could only do this 2000/second and nothing else.

This library splits the triggering of a ADC cycle and reading of result values. This leafs you program to do some other stuff will waiting on completion of the ADC cycle.
It is able to read all 5 ADC channels in sequence.

Setup
=====
Starting a scan
===============
Reading values
==============
