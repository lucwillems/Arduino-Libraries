#include <HardwareAnalog.h>

//List of channels to read in 1 scan 
//channels will be read in order 
byte channels[] = {A0, A1, A2, A3, A4, A5};
byte ledPin = 13;

void ADCFinished() {
     //Print result of scanning
     for (int i=0;i<6;i++) {
       Serial.print("Channel ");
       Serial.print(i);
       Serial.print(" : ");
       //Analog.analogRead() will return the last succesfull scanned value
       Serial.print(Analog.analogRead(i)); //we use index of our channels, NOT A0,A1,A2 !!
       Serial.print(" time: ");
       //Analog.analogTime() will return the time between start ADC and the interrupt send when ADC is finished
       Serial.println(Analog.analogTime(i)); //we use index of our channels, NOT A0,A1,A2 !!
     }       
     //scanTime() returns total time of scanning all channels configured in initScan()
     Serial.print("scan Time :");
     Serial.print(Analog.scanTime());
     Serial.println("");
     Serial.println("");
}

void setup() {
  // Initialize serial
  Serial.begin(115200);
  Serial.println("setup ...");
  pinMode(ledPin, OUTPUT);

  Analog.setPrescaler(PS_128);   //Select the default 128 prescaler, other values are PS_64,PS_32,PS16
  Analog.setReference(DEFAULT);  //Select the default 5v reference. 
                                 //you can use the same values as with analogReference

  Analog.setSamples(2);             //number of samples to read before storing.
  Analog.initScan(6, channels);     //initialize scanning with number of channels and channel list
  Analog.setCallBack(&ADCFinished); //set callback function which will be called when ADC is finished
  Serial.println("setup done");
  Analog.startScan();               //start scan, first scan is always slow...
}


void loop() {
  //Start a new scan, will return if there is still a scan active
  if (! Analog.startScan()) {
     digitalWrite(ledPin, HIGH); //put led on
     //You will not see this message
     //First scan in setup will give this message as it will not be finished when we start loop
     //second,third... scan are finished before all Serial messages are done.
     //you could always use a timer to trigger startScan() and use the main loop
     //for interactive usage
     Serial.println("scan is still active");
  } else {
    delay(1000);
   //Access internal temperature,Reference and GND ADC channel. this channel
   //uses internal 1.1v references and is a bit slow. 
   //these calls are blocking because switching between reference take time to settle 
   //the internal electronics.
   
   Serial.print("Temperature: ");
   Serial.println(Analog.Temperature(100));
   Serial.print("internal Reference: ");
   Serial.println(Analog.internalReference());
   Serial.print("internal GND: ");
   Serial.println(Analog.internalGND());
 }
}
