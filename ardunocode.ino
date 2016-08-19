#include <SoftwareSerial.h>
//#include "AltSoftSerial.h"

//Wifi device -----------------------------
SoftwareSerial wifiSerial(8,9); // RX, TX 8,9

bool safe = true;

// Sensor pins
const int s1trigPin = 10;
const int s1echoPin = 11;
const int s2trigPin = 12;
const int s2echoPin = 13;
long duration1, inches_1, cm_1,duration2, inches_2, cm_2;

//Setup and Loop --------------------------------------------------------------------------------------------
unsigned long lastTime;
void setup() {


  

  
  

  setupMotors();
//  forward(255);

  //Set the time
  lastTime = millis();
  
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  
  
  digitalWrite(13, HIGH);
  wifiSerial.begin(9600);
  Serial.begin(9600);
  delay(500);
  digitalWrite(13, LOW);
  
  
  //Setting up
  Serial.println("Me:Setting up");

  //Set mode for both clien and server mode
  sendToWifi("AT+CWMODE=3");
  
  //connect
  Serial.println("Me:Tring to connect");
  //Connect to hostpot
  sendToWifi("AT+CWJAP=\"NO FREE\",\"a92*eRrA9$\"");

  delay(6000);
 // Serial.println("Enabling Multi Channel");
  //Enable multi channel
  sendToWifi("AT+CIPMUX=1");
  //Get IP
 // Serial.println("Getting IP");
  sendToWifi("AT+CIFSR");
  //Start server

  
 // Serial.println("Starting Serverss");
  sendToWifi("AT+CIPSERVER=1,1336");

  setupMotors();
  blk(5); 
}

//-----------------------------------------------------------------------------------Loop
void loop() {
  // put your main code here, to run repeatedly: 

  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(s1trigPin, OUTPUT);
  digitalWrite(s1trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(s1trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(s1trigPin, LOW);

  pinMode(s2trigPin, OUTPUT);
  digitalWrite(s2trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(s2trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(s2trigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(s1echoPin, INPUT);
  duration1 = pulseIn(s1echoPin, HIGH);
   duration2 = pulseIn(s2echoPin, HIGH);
  

  // convert the time into a distance
  inches_1 = microsecondsToInches(duration1);
  cm_1 = microsecondsToCentimeters(duration1);
   inches_2 = microsecondsToInches(duration2);
  cm_2 = microsecondsToCentimeters(duration2);

  
  

 

  //else{
   // digitalWrite("not stop");
  //}
  
  if (wifiSerial.available())
  {
    String s = wifiSerial.readStringUntil('\n');
    String extracted  = extract(s);
    if(extracted=="")
      Serial.println(s);
    else
    {
      Serial.print("Original:"+s+" | ");
      Serial.println("Extracted:"+extracted);
    }
    process(extracted);
  }
  if (Serial.available())
  {
    String s = Serial.readStringUntil('\n');
    process(s);
    wifiSerial.println(s);
    Serial.println(s);
  }

  

  //Eranga

  
}

void blk(int count)
{
  digitalWrite(13,LOW);
  delay(100);
  for(int i=0; i<count; i++)
  {
    digitalWrite(13, HIGH);
    delay(50);
    digitalWrite(13,LOW);
    delay(50);
  }
}


void sendToWifi(String str)
{
  sendToWifi(str,2000);
}

void sendToWifi(String str, int t)
{
  delay(t);
  while(wifiSerial.available())
  {
    Serial.println(wifiSerial.readStringUntil('\n'));
  }
  wifiSerial.println(str);
  blk(2);
  delay(t);
  while(wifiSerial.available())
  {
    Serial.println(wifiSerial.readStringUntil('\n'));
  }
}

void sendToClient(String str, int chn)
{
  wifiSerial.println("AT+CIPSEND="+(String)chn+","+(String)str.length());
  delay(100);
  wifiSerial.println(str);
}


String extract(String s)
{
  if(!(s[0]=='+' && s[1]=='I' && s[2]=='P' && s[3]=='D')) return "";
  for(int i=0; i<s.length(); i++)
  {
    if(s[i]==':')
    {
      String msg = s.substring(i+1);
      return msg;
    }
  }
  return "";
}

void process(String s)
{
  if(s=="") return;
  for(int i=0; i<s.length(); i++)
  {
    if(s[i] == 'f')
      
    forward(100);
  
    
      
      
    if(s[i] == 'b')
       backward(100);
   
    
    
    if(s[i] == 's')
      stopMotors();
    
    if(s[i] == 'l')
      left(100);
    
    if(s[i] == 'r')
      right(100);
      
  }
}



//---------------------------------------------------------------- Motor Control

//---------------------------------------------------------------- Motor Control
unsigned int pins[] = {5,6,7,3,2,4}; // pwm, others
void setupMotors()
{
  for(int i=0; i<6; i++)
  {
    pinMode(pins[i], OUTPUT);
  }
}

void setMotor(unsigned int motor, unsigned int spd, bool dir)
{
  unsigned int shift = 3*motor;
  analogWrite(pins[shift], spd);

  if(dir)
  {
    digitalWrite(pins[1+shift], HIGH);
    digitalWrite(pins[2+shift], LOW);
  }
  else
  {
    digitalWrite(pins[1+shift], LOW);
    digitalWrite(pins[2+shift], HIGH);
  }
}

void stopMotors()
{
  digitalWrite(pins[1],HIGH) ;
  digitalWrite(pins[2],HIGH) ;
  digitalWrite(pins[4],HIGH) ;
  digitalWrite(pins[5],HIGH) ;
  Serial.print("stop");
}

// Other functions
void forward(unsigned int spd)
{
  setMotor(0,spd,true);
  setMotor(1,spd,true);
  Serial.print("forward");
  
}

void backward(unsigned int spd)
{
  setMotor(0,spd,false);
  setMotor(1,spd,false);
  Serial.print("back");
}

void left(unsigned int spd)
{
  setMotor(0,spd,false);
  setMotor(1,spd,true);
  Serial.print("left");
  stopMotors();
}

void right(unsigned int spd)
{
  setMotor(0,spd,true);
  setMotor(1,spd,false);
  Serial.print("right");
  stopMotors();
}

long microsecondsToInches(long microseconds)
{
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

