#include <HardwareAnalog.h>
//This sketch is used as test for speed and locking.
//
//it will set the prescaler to absurt low number , causing high frequency ADC sampling
//it will sample ALL analog channels + 3 internal channels.
//Note that a new ADC cycle is start as part of the callback function when all
//channels are scanned. this is the fasted way todo this.
//


//channels will be read in order 
const byte channels[] = {A0, A1, A2,A3,A4,A5};
const byte ledPin = 13;
long loopCounter;
long scanCounter;
long prevCounter;
long time;
bool flag;

void ADCFinished() {
     scanCounter++;
     long x=0;
     x +=Analog.analogRead(0);
     x +=Analog.analogRead(1);
     x +=Analog.analogRead(2);
     x +=Analog.analogRead(3);
     x +=Analog.analogRead(4);
     x +=Analog.analogRead(5);
     Analog.startScan();//start the next scan
}

void setup() {
  // Initialize serial
  Serial.begin(115200);
  Serial.println("setup ...");
  pinMode(ledPin, OUTPUT);

  Analog.setPrescaler(PS_4);     //should really be fast, but resolution of result is low
  Analog.setReference(DEFAULT);  //Select the default 5v reference. 
  Analog.setSamples(1);          //number of samples to read before storing.
  Analog.initScan(sizeof(channels), channels);     //initialize scanning with number of channels and channel list
  Analog.setCallBack(&ADCFinished); //set callback function which will be called when ADC is finished
  Serial.println("setup done");
  Analog.startScan();               //start scan, first scan is always slow...
  time=millis();
}


void loop() {
  flag=!flag;
  digitalWrite(ledPin, flag); //put led on
  if (millis() >= time+5000) {
     //Access internal temperature,Reference and GND ADC channel. this channel
     //uses internal 1.1v references and is a bit slow. 
     //these calls are blocking because switching between reference take time to settle 
     //the internal electronics.
     for (int i=0;i<6;i++) {
        Serial.print("channel ");
        Serial.print(i);
        Serial.print(" : ");
        Serial.println(Analog.analogRead(i));
     }
     Serial.print("Temperature: ");
     Serial.println(Analog.Temperature(100));
     Serial.print("internal Reference: ");
     Serial.println(Analog.internalReference());
     Serial.print("internal GND: ");
     Serial.println(Analog.internalGND());
     Serial.print("SCAN count: ");
     Serial.println(scanCounter/5);
     Serial.print("loop count: ");
     Serial.println(loopCounter/5);
     Serial.println("");
     scanCounter=0;
     loopCounter=0;
     time=millis();
 }
 loopCounter++;
}
