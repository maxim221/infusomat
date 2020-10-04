#include <AccelStepper.h>
#include <SoftwareSerial.h>
#include <Buzzer.h>
#include <Wire.h>

#include <I2C_Anything.h>


Buzzer buzzer(11, 12);
SoftwareSerial mySerial(8, 9); // RX, TX

AccelStepper stepper(AccelStepper::DRIVER, 2, 3);

#define WIRE_STEPS_DEVICE 4

int spd = 1000;    // The current speed in steps/second
int sign = 1;      // Either 1, 0 or -1
String lastCommand;
String c;

String speedVerify = "";
String thresholdVerify = "";
String doseVerify = "";
String endVerify = "";

String tempSpeedVerify = "";
String temp1Verify = "";
String temp2Verify = "";
String endTempVerify = "";

String fill = "";
String holder = "";
String error = "";


float speedValue;
float thresholdValue;
float doseValue;

float temp1;
float temp2;

float interSpeedValue;
float mL;

int counter1;
int counter2;
int feedback;
int margin2;
int margin3;
int Switch;
int presence;
int stepCount = 0;
int numberOfSteps = 1;
int minutesDelay = 0;
int secondsDelay = 0;

unsigned long lastSendDataTime;

int sendDataDelay = 5000;

long stepDelay = 0;
long oldStepDelay = 0;

int buzzerTime = 500;

boolean counterTic = true;
boolean oldTick;

boolean commandReceived = false;
boolean FFd = false;

boolean playStopSoud = false;

void setup()
{ 
  mySerial.begin(4800);
  Serial.begin(19200);
  Wire.begin(WIRE_STEPS_DEVICE);
  Wire.onRequest(requestEvent);
  stepper.setMaxSpeed(1000);
  stepper.setSpeed(1000); 

  pinMode(A0, OUTPUT);  
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
//  pinMode(A4, INPUT_PULLUP);
  pinMode(A0, INPUT_PULLUP); // Switch
  pinMode(A6, INPUT);
  pinMode(A7, INPUT_PULLUP);
  pinMode(13, INPUT);
  pinMode(12, INPUT);
  pinMode(11, INPUT);
  pinMode(10, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  digitalWrite(4, LOW);  
}

volatile float floatVal;
volatile int intVal;


void requestEvent()
{  
 intVal = 300;
  
 I2C_singleWriteAnything(intVal);
}

void loop()
{ 
if(millis()-lastSendDataTime > sendDataDelay){
 String dataToSend = "";
 float tempDiff = 0;
 dataToSend = "temp1:";
 dataToSend += temp1;
 dataToSend += "temp2:";
 dataToSend += temp2; 
// dataToSend += "difference:";
// if(temp1>temp2){tempDiff = temp1-temp2;}
// if(temp1<temp2){tempDiff = temp2-temp1;}
// dataToSend += tempDiff; 
 dataToSend += "speed:";
 dataToSend += speedValue; 
 dataToSend += "dose:";
 dataToSend += doseValue;
 dataToSend += "threshold:";
 dataToSend += thresholdValue;
 dataToSend += "injection:";
 dataToSend += mL;
 dataToSend += "fill:";
 dataToSend += fill; 
 dataToSend += "holder:";
 dataToSend += holder; 
 dataToSend += "error:";
 dataToSend += error;
 Serial.println(dataToSend);
 lastSendDataTime = millis();
}

//  feedback = analogRead(A0);  
  counter1 = analogRead(A1);  
  counter2 = analogRead(A2);
  margin2 = analogRead(A7);
  margin3 = analogRead(A3);  
  Switch = analogRead(A0);
  presence = analogRead(A6);


  if(mySerial.available()>0) {
    c = mySerial.readStringUntil("\n");
    int beginningOfMessage = c.indexOf("speed");
    c.remove(0,beginningOfMessage);
//    Serial.print(c);
//    Serial.println(c.length());
  
  switch (c.length()){
  case 65:
             
    speedVerify = c.substring(0,5);
    thresholdVerify = c.substring(39,48);
    doseVerify = c.substring(52,56);
    endVerify = c.substring(60,63);
//    Serial.println(speedVerify);  
//    Serial.println(thresholdVerify);  
//    Serial.println(doseVerify);  
//    Serial.println(endVerify);

if(speedVerify == "speed" && thresholdVerify == "threshold" && doseVerify == "dose" && endVerify == "end" && commandReceived == false){
//    Serial.println(speedVerify);
//    Serial.println(thresholdVerify);
//    Serial.println(doseVerify);
//    Serial.println(endVerify);
    int cleanTemp = c.indexOf("\n");
    c.remove(0,cleanTemp);
//Serial.print("Case 65:");
//Serial.print(c);

    speedValue = c.substring(6,9).toFloat();
    thresholdValue = c.substring(19,22).toFloat();
    doseValue = c.substring(27,31).toFloat();

//    Serial.print("speedValue:");
//    Serial.println(speedValue); 

    numberOfSteps = doseValue*75;
    minutesDelay = int(speedValue);
    minutesDelay = minutesDelay*60;
    
//    Serial.print("minutesDelay:");
//    Serial.println(minutesDelay); 
//    Serial.print("secondsDelay:");
       
    secondsDelay = ((speedValue*100) - int(speedValue)*100);
//    Serial.println(secondsDelay);
//    secondsDelay = speedValue - secondsDelay;
//    Serial.println(secondsDelay);
//    secondsDelay = secondsDelay*1000;
//    Serial.println(secondsDelay);
    stepDelay = (minutesDelay+secondsDelay)-58;
    stepDelay = stepDelay*1000;
    stepDelay = stepDelay/numberOfSteps;
//    Serial.print("stepDelay:");
//    Serial.println(stepDelay);
//    Serial.print("numberOfSteps:");    
//    Serial.println(numberOfSteps);
//    Serial.println(speedValue);
//    Serial.println(thresholdValue);
//    Serial.println(doseValue);
 //   commandReceived = true;
    playStopSoud = true;
    analogWrite(10, HIGH);
    
//    mySerial.println("initialisation");
//    mySerial.println("\n");    
//    mySerial.println("commandRecieved");
 //   Serial.println("new command recieved!");
 //   buzzer.sound(NOTE_C3, 1000);
//    Serial.println(speedValue);
//    Serial.println(thresholdValue);
//    Serial.println(doseValue);
      c ="";
  }
  
  break;  

  case 30:
  tempSpeedVerify = c.substring(0,5);
  temp1Verify = c.substring(5,10);
  temp2Verify = c.substring(15,20);
  endTempVerify = c.substring(25,28);
    if(tempSpeedVerify == "speed" && temp1Verify == "temp1" && temp2Verify == "temp2" && endTempVerify == "end" && commandReceived == false){
  temp1 = c.substring(10,15).toFloat();
  temp2 = c.substring(20,25).toFloat();
  if(temp1<10){
  error = "SensErr!";  
  }
  if(temp1>10){
  error = "SensorOk";  
  }  
//  Serial.println(temp1);
//  Serial.println(temp2);    
    
    }
  break;
  }
  

    
  
  }
  if(speedValue !=0){
    FFd = true;
    sign = 1;
    spd = 1000;

//    Serial.println("speedOK");

  }
  if(c == "stop"){
    FFd = false;
    sign = 0;
    stepCount = 0;
    analogWrite(10, HIGH);
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);
  }
  if(FFd){
    
//    if(oldStepDelay - millis() > stepDelay){
//    sign = 1;
    pinMode(4, INPUT);
    stepper.setSpeed(sign * spd);
    stepper.runSpeed();
    analogWrite(A0, 0);
//    oldStepDelay = millis();
//    }
//    else{
//    sign = 0;
//    }
  }

  lastCommand = c;
  
  
if(counterTic){
if(counter1>=1000 && counter2>=1000){
counterTic = false;
stepCount++;                 //one step costs 100ms 2 seconds error for minute
mL = ((doseValue/numberOfSteps)*stepCount);
delay(stepDelay);
//    Serial.println("UP");
// Serial.print("  Step:");
// Serial.println(stepCount);
}
}
if(counterTic == false){
  if((counter2-counter1) >=900){
    counterTic = true;
//    mySerial.println(stepCount);
//    Serial.println("DOWN");

  }
}
//  Serial.print("  counter1:");
//  Serial.print(counter1);
//  Serial.print("  counter2:");
//  Serial.print(counter2);

//  Serial.print("  feedback:");
//  Serial.println(feedback);
//  Serial.print("  margin2:");
//  Serial.print(margin2);
//  Serial.print("  margin3:");
//  Serial.println(margin3);
//  Serial.print("  Switch:");
//  Serial.println(Switch);
//  delay(1000);
//  Serial.print("  presence:");
//  Serial.println(presence);
if(stepCount>=numberOfSteps){
  FFd = false;
  sign = 0;
  stepper.setSpeed(0);
  speedValue = 0;
  thresholdValue = 0;
  doseValue = 0;
  mL = 0;
//  mySerial.println("complete!");
  c = "stop";
  
   buzzer.begin(100);
if(playStopSoud){
  if(stepCount>0){
  buzzer.sound(NOTE_E7, 80);
  buzzer.sound(NOTE_E7, 80);
  buzzer.sound(0, 80);
  buzzer.sound(NOTE_E7, 80);
  buzzer.sound(0, 80);
  buzzer.sound(NOTE_C7, 80);
  buzzer.sound(NOTE_E7, 80);
  buzzer.sound(0, 80);
  buzzer.sound(NOTE_G7, 80);
  
  buzzer.end(2000);
  }
  playStopSoud = false;
}
}
  if(Switch>1000 || margin3>1000){
    
//  Serial.println("stopped");
    FFd = false;
    buzzer.sound(NOTE_E7, 100);
    buzzer.sound(NOTE_G7, 200);

   }
    if(Switch>1000){
    holder = "Open!!";  
    }
    if(Switch<1000){
    holder = "closed";  
    }
    if(margin3>1000){
    fill = "End!";  
    }
    if(margin3<1000){
    fill = "full";  
    }
  if(Switch<1000 && margin3<800){


//  Serial.println("running");
  }
  if(oldTick != counterTic){
    
 //   Serial.println(counterTic);
  }

  
}
