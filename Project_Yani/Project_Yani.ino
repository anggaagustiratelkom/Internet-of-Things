#include <IRremote.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Sim800l.h>
#include <SoftwareSerial.h>

Sim800l gsmModule;
//-------------------------------
LiquidCrystal_I2C lcd(0x3F,  2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
//--------------------------------------------------------------
const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;

//byte statusLed = 13;
byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 7;



// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor =   4.5;

volatile byte pulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

//Create software serial object to communicate with SIM800L
//SoftwareSerial mySerial(3, 2); //SIM800L Tx & Rx is connected to Arduino #3 & #2


void setup(){
    Serial.begin(9600);
//  gsmModule.begin(); // initializate the library.
//  gsmModule.sendSms("+6281395145194","the text go here"); // (Mobile Number, Text to be sent).
  
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);
  //==================================SIM 800L
//  Serial.println("Initializing...");
//  delay(1000);
//
//  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
//  updateSerial();
//  mySerial.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
//  updateSerial();
//  mySerial.println("AT+CCID"); //Read SIM information to confirm whether the SIM is plugged
//  updateSerial();
//  mySerial.println("AT+CREG?"); //Check whether it has registered in the network
//  updateSerial();
//  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
//  updateSerial();
//  mySerial.println("AT+CMGS=\"+6281395145194\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
//  updateSerial();
//  mySerial.print("Last Minute Engineers | lastminuteengineers.com"); //text content
//  updateSerial();
//  mySerial.write(26);
  //========================================
  lcd.begin(16,2);
  irrecv.enableIRIn();
  irrecv.blink13(true);

   // Initialize a serial connection for reporting values to the host
//  Serial.begin(9600);
   
  // Set up the status LED line as an output
//  pinMode(statusLed, OUTPUT);  // We have an active-low LED attached
  
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

void loop(){
  updateSerial();
  lcd.setCursor(0,0);
  lcd.print("Msukan Password");
  if (irrecv.decode(&results)){
        Serial.println(results.value, HEX);
        lcd.setCursor(1,0);
        lcd.print(results.value, HEX);
        irrecv.resume();
  }


    if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);
        
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
      
    unsigned int frac;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space

    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");        
    Serial.print(totalMilliLitres);
    Serial.println("mL"); 
    Serial.print("\t");       // Print tab space
  Serial.print(totalMilliLitres/1000);
  Serial.print("L");
    

    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}

/*
Insterrupt Service Routine
 */
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
