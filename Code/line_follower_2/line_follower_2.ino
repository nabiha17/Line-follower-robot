#include <QTRSensors.h>
QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

float Kp = 2.0; //set up the constants value
float Ki = 0.18;
float Kd = 0.35;
int P;
int I;
int D;

int lastError = 0;

//Increasing the maxspeed can damage the motors - at a value of 255 the 6V motors will receive 7,4 V 
const uint8_t maxspeeda = 60;
const uint8_t maxspeedb = 60;
const uint8_t basespeeda = 100;
const uint8_t basespeedb = 100;

//Set up the drive motor carrier pins
int bphasex=7;
int bphase = 8;
int benbl = 6;

int aphase = 5;
int aphasex=4;
int aenbl = 3;

int mode=10;

char t;

boolean Ok = false;
void setup() {
  Serial.begin(9600);
  qtr.setTypeAnalog();
  //Set up the sensor array pins
  qtr.setSensorPins((const uint8_t[]){A0, A1, A2, A3, A4, A5, A6, A7}, SensorCount);
  qtr.setEmitterPin(2);//LEDON PIN

  pinMode(aphasex, OUTPUT);
  pinMode(aphase, OUTPUT);
  pinMode(aenbl, OUTPUT);
  pinMode(bphase, OUTPUT);
  pinMode(bphasex, OUTPUT);
  pinMode(benbl, OUTPUT);

  pinMode(mode, INPUT);


  delay(500);
  pinMode(LED_BUILTIN, OUTPUT);

  
  
  forward_brake(0, 0);
}

void calibration() {
  digitalWrite(LED_BUILTIN, HIGH);
  for (uint16_t i = 0; i < 400; i++)
  {
    qtr.calibrate();
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  int Mode=digitalRead(mode);
  if(Mode==1){
    while (Ok == false) { //the loop won't start until the robot is calibrated
      calibration(); //calibrate the robot for 10 seconds
      Ok = true;
  }
    PID_control();
     Serial.println("Maze robot");
  }
  else{
    Serial.println("BLE robot");
    if(Serial.available()){
  t = Serial.read();
  Serial.println(t);
  if(t == 'F'){            //move forward(all motors rotate in forward direction)
  forward();
}
 
else if(t == 'B'){     
  back();
}
else if(t == 'L'){      
  left();
}
else if(t == 'R'){      
  right();
}
else if(t == 'S'){     
  stops();
}
}
  }
  /*
  else {
    forward_brake(0,0);
  }*/
}
void forward_brake(int posa, int posb) {
  //set the appropriate values for aphase and bphase so that the robot goes straight
  digitalWrite(aphase, LOW);
  digitalWrite(aphasex, HIGH);
  digitalWrite(bphase, HIGH);
  digitalWrite(bphasex, LOW);
  analogWrite(aenbl, posa);
  analogWrite(benbl, posb);
}
void forward() {
  //set the appropriate values for aphase and bphase so that the robot goes straight
  digitalWrite(aphase, LOW);
  digitalWrite(aphasex, HIGH);
  digitalWrite(bphase, HIGH);
  digitalWrite(bphasex, LOW);
  analogWrite(aenbl, 100);
  analogWrite(benbl, 100);
}
void back() {
  //set the appropriate values for aphase and bphase so that the robot goes straight
  digitalWrite(aphase, HIGH);
  digitalWrite(aphasex, LOW);
  digitalWrite(bphase, LOW);
  digitalWrite(bphasex, HIGH);
  analogWrite(aenbl, 100);
  analogWrite(benbl, 100);
}
void left() {
  //set the appropriate values for aphase and bphase so that the robot goes straight
  digitalWrite(aphase, LOW);
  digitalWrite(aphasex, HIGH);
  digitalWrite(bphase, HIGH);
  digitalWrite(bphasex, LOW);
  analogWrite(aenbl, 0);
  analogWrite(benbl, 100);
}
void right() {
  //set the appropriate values for aphase and bphase so that the robot goes straight
  digitalWrite(aphase, LOW);
  digitalWrite(aphasex, HIGH);
  digitalWrite(bphase, HIGH);
  digitalWrite(bphasex, LOW);
  analogWrite(aenbl, 100);
  analogWrite(benbl, 0);
}
void stops() {
  //set the appropriate values for aphase and bphase so that the robot goes straight
  digitalWrite(aphase, HIGH);
  digitalWrite(aphasex, LOW);
  digitalWrite(bphase, LOW);
  digitalWrite(bphasex, HIGH);
  analogWrite(aenbl, 0);
  analogWrite(benbl, 0);
}
void PID_control() {
  uint16_t position = qtr.readLineBlack(sensorValues);

  // print the sensor values as numbers from 0 to 1000, where 0 means maximum
  // reflectance and 1000 means minimum reflectance, followed by the line
  // position
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    delay(50);
    Serial.print('\t');
  }
  Serial.println(position);
  int error = 3500 - position;

  P = error;
  I = I + error;
  D = error - lastError;
  lastError = error;
  int motorspeed = P*Kp + I*Ki + D*Kd;
  
  int motorspeeda = basespeeda + motorspeed;
  int motorspeedb = basespeedb - motorspeed;
  
  if (motorspeeda > maxspeeda) {
    motorspeeda = maxspeeda;
  }
  if (motorspeedb > maxspeedb) {
    motorspeedb = maxspeedb;
  }
  if (motorspeeda < 0) {
    motorspeeda = 0;
  }
  if (motorspeedb < 0) {
    motorspeedb = 0;
  } 
  // Serial.print(motorspeed);Serial.print(" ");Serial.print(motorspeeda);Serial.print(" ");Serial.println(motorspeedb);
  forward_brake(motorspeeda, motorspeedb);
  
}
