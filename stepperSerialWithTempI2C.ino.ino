#include <AccelStepper.h>
#include <SoftwareSerial.h>
#include <Buzzer.h>
#include <Wire.h>

#include <I2C_Anything.h>


Buzzer buzzer(11, 12);
SoftwareSerial mySerial(8, 9); // RX, TX

AccelStepper stepper(AccelStepper::DRIVER, 2, 3);

#define WIRE_STEPS_DEVICE 4

#define DOSE_STEPS_RATIO 75


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
  I2C_singleWriteAnything(stepCount);
}

void sendData(){
    if(millis()-lastSendDataTime > sendDataDelay){
     String dataToSend = "";
     float tempDiff = 0;
     dataToSend = "temp1:";
     dataToSend += temp1;
     dataToSend += "temp2:";
     dataToSend += temp2;  
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
//     Serial.println(dataToSend);
     lastSendDataTime = millis();
  }
}

void readParams(){
  counter1 = analogRead(A1);
  counter2 = analogRead(A2);
  margin2 = analogRead(A7);
  margin3 = analogRead(A3);
  Switch = analogRead(A0);
  presence = analogRead(A6);
}

void readFromInfusomat(){
  int prefixLength = 4;
  String cmd;
  if(mySerial.available()>0) {
    c = mySerial.readStringUntil("\n");
    while(c.length()>0){
      cmd = c.substring(0,prefixLength);
      c.remove(0,prefixLength);
      if (c.length() <= prefixLength){
        break;  
      }
      
      Serial.print("Length: ");
      Serial.println(c.length());
      Serial.print("Command prefix: ");
      Serial.println(cmd);
      
      if (cmd == "cmnd"){
          Serial.print("Parsing: ");
          Serial.println(c);
          Serial.println(c.substring(0,4));
          Serial.print("Parsed speed: ");
          speedValue = c.substring(0,4).toFloat();
          Serial.println(speedValue);
  
          Serial.println(c.substring(13,17));
          Serial.print("Parsed treshold: ");          
          thresholdValue = c.substring(13,17).toFloat();
          Serial.println(thresholdValue);
  
          Serial.println(c.substring(21,25));
          Serial.print("Parsed dose: ");
          doseValue = c.substring(21,25).toFloat();
          Serial.println(doseValue);
                      
          numberOfSteps = doseValue*DOSE_STEPS_RATIO;
          minutesDelay = int(speedValue) * 60;
                 
          secondsDelay = ((speedValue*100) - int(speedValue)*100); 
          stepDelay = (minutesDelay+secondsDelay)-30;//хуй знает почему 30
          stepDelay = stepDelay*1000;
          stepDelay = stepDelay/numberOfSteps;
          playStopSoud = true;
          c.remove(0,29);
      }
      
      if (cmd == "data"){
          Serial.print("Parsing: ");
          Serial.println(c);
          temp1Verify = c.substring(0,5);
          temp2Verify = c.substring(10,15);
          endTempVerify = c.substring(20,23);
          if(temp1Verify == "temp1" && temp2Verify == "temp2" && endTempVerify == "end"){
            temp1 = c.substring(5,15).toFloat();
            temp2 = c.substring(15,20).toFloat();
            Serial.print("Temp1: ");
            Serial.println(temp1);
            Serial.print("Temp2: ");
            Serial.println(temp2);
            if(temp1<10){
              error = "SensErr!";
            }
            if(temp1>10){
              error = "SensorOk";
            }
          } 
          c.remove(0,25);
      }
    }
  }
}

void loop()
{ 
  sendData();
  readParams();
  readFromInfusomat();
  
  if(speedValue != 0){
    FFd = true;
    sign = 1;
    spd = 1000;
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
    pinMode(4, INPUT);
    stepper.setSpeed(sign * spd);
    stepper.runSpeed();
    analogWrite(A0, 0); 
  }

  lastCommand = c;

  if(counterTic){
    if(counter1 >= 1000 && counter2 >= 1000){
      counterTic = false;
      stepCount++;       //one step costs 100ms 2 seconds error for minute
      mL = ( ( doseValue / numberOfSteps ) * stepCount);
      delay(stepDelay); 
    }
  }
  
  if(counterTic == false){
    if((counter2-counter1) >=900){
      counterTic = true; 
    }
  }
    
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
}
