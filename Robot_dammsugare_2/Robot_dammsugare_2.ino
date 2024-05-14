





#define ML_Ctrl 4     //define direction control pin of B motor
#define ML_PWM 5   //define PWM control pin of B motor
#define MR_Ctrl 2    //define direction control pin of A motor
#define MR_PWM 9   //define PWM control pin of A motor
#define MB_Ctrl 3   //define the controle pin of brush motor
#include <IRremote.h>   //function library of IR remote control
int RECV_PIN = A0;   // set the pin of IR receiver to A0
IRrecv irrecv(RECV_PIN);   // irrecv receives values from IR-sensor
   //introduce variables
long irr_val;   
decode_results results;
int mode = 0;
int right_left = 0;
int brush = 0;

int trigPinFront = 12;    // Trigger
int echoPinFront = 13;    // Echo
int trigPinRight = 10;    // Trigger
int echoPinRight = 11;    // Echo

int distensForward;
int distensToTheRight;
int dif;

void setup() {
  pinMode(ML_Ctrl, OUTPUT);//define direction control pin of B motor to OUTPUT
  pinMode(ML_PWM, OUTPUT);//define PWM control pin of B motor to OUTPUT
  pinMode(MR_Ctrl, OUTPUT);//define direction control pin of A motor to OUTPUT
  pinMode(MR_PWM, OUTPUT);//define PWM control pin of A motor to OUTPUT
  pinMode(MB_Ctrl, OUTPUT);//define control pin of brush motor to OUTPUT
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  pinMode(trigPinFront, OUTPUT);
  pinMode(echoPinFront, INPUT);
  pinMode(trigPinRight, OUTPUT);
  pinMode(echoPinRight, INPUT);
}


void loop(){

  if (irrecv.decode(&results)){
    irr_val = results.value;
    switch(irr_val){   //if irr_val is the same as following cases it will follow that particular case
      case 0x807F10EF : mode = 1; break;   
      case 0x807F906F : mode = 2; break;
      case 0x807F609F : BRUSH_M(); break;
      case 0x807F30CF : mode = 0; break;
      case 0x807F40BF : stepLeft(); break;
      default : mode = mode; break;   //if the signal wasn't the same ass any of above mode will not change
    }
    MODE() ;// calls the function MODE
    irrecv.resume();   // Receive the next value
  }
}


void MODE(){ // MODE is the function that controls what the dust swepper 
  
  // mode = 0 stop
  // mode = 1 forward
  // mode = 2 backward

  switch(mode){ // checks if mode is ether 0, 1 or 2
      
    case 0 :
      switch(irr_val){ // checks if IR gets a signal
          case 0x807FD02F : right_left = 1; break; //if left button is pushed
          case 0x807F50AF : right_left = 2; break; //if right button is pushed
          case 0xFFFFFFFF : right_left = right_left; break;  // if eror IR message is received
          default : right_left = 0; break; //if no signal is received
          }
      switch(right_left){ //checks if it should turn or not
          case 0 : car_Stop(); break;   //no turn to left or right
          case 1 : car_left_turn(); break;   //turn left
          case 2 : car_right_turn(); break;   //turrn right
          default : car_Stop(); break;   //default stop
          }
      break;
          
    case 1 :
      switch(irr_val){
          case 0x807FD02F : right_left = 1; break;
          case 0x807F50AF : right_left = 2; break;
          case 0xFFFFFFFF : right_left = right_left; break;
          default : right_left = 0; break;  
          }
      switch(right_left){
          case 0 : car_front(); break;
          case 1 : car_left(); break;
          case 2 : car_right(); break;
          default : car_front(); break;   //default front
          }
      break;

    case 2 :
      switch(irr_val){
          case 0x807FD02F : right_left = 1; break;
          case 0x807F50AF : right_left = 2; break;
          case 0xFFFFFFFF : right_left = right_left; break;
          default : right_left = 0; break;
          }
      switch(right_left){
          case 0 : car_back(); break;
          case 1 : car_left(); break;
          case 2 : car_right(); break;
          default : car_back(); break;   //default back
          }
      break;
    
    default : car_Stop(); mode = 0; break;    
  }
}


void Auto() {
  
  int upperWall = distensToWall();
  Serial.println (upperWall);
  stepRight();
  int rightWall = distensToWall();
  Serial.println (rightWall);
  stepRight();
  int lowerWall = distensToWall();
  Serial.println (lowerWall);
  stepRight();
  int leftWall = distensToWall();
  Serial.println (leftWall);

  int stepcounterColumn = 1;
  while (leftWall > stepcounterColumn){
    stepForward();
    stepcounterColumn += 1;
  }
  stepRight();
  int stepcounterRow = 1;
  while (upperWall > stepcounterRow){
    stepForward();
    stepcounterRow += 1;
  }
  stepRight();

  stepcounterColumn = 0;
  stepcounterRow = 0;
  int turncounter = 0;
/*
  while (stepcounterRow < (upperWall + lowerWall - 2)){
    
    if (turncounter = 1){
      stepRight();
      stepForward();
      stepRight();
      turncounter = 2;
    }
    else if (turncounter = 2){
      stepLeft();
      stepForward();
      stepLeft();
      turncounter = 1;
    }
    else{
      turncounter = 1;
    }
    
    while (stepcounterColumn < (rightWall + leftWall - 2)){
      stepForward();
      stepcounterColumn += 1;
    }
    
    stepcounterRow += 1;
  }
*/
}



int distensToWall() {
  int wall;
  delay(250);
  int interval = distensFront();
  Serial.println(interval);
  delay(250);
  if (interval < 200 ) {
    wall = ((interval/40) + 1);
  } else{
    wall = 5;
  }
  Serial.println(wall);
  return wall;
}


void car_front(){   //car goes forward
/*  Serial.println("Forward"); */
    digitalWrite(ML_Ctrl,HIGH);//set direction control pin of B motor to HIGH 
    analogWrite(ML_PWM,200);//Set PWM control speed of B motor to 20
    digitalWrite(MR_Ctrl,HIGH);//set direction control pin of A motor to HIGH 
    analogWrite(MR_PWM,200);//Set PWM control speed of A motor to 20
}
void car_back(){   //car goes back
/*  Serial.println("Back"); */
    digitalWrite(ML_Ctrl,LOW);//set direction control pin of B motor to LOW
    analogWrite(ML_PWM,200);//set PWM control speed of B motor to 200
    digitalWrite(MR_Ctrl,LOW);//set direction control pin of A motor to LOW
    analogWrite(MR_PWM,200);//set PWM control speed of A motor to 200
}
void car_left_turn(){   //car turns left
/*  Serial.println("Left"); */
   //one side gos back and other gos front in the same speed so ti rurns on the spot
    digitalWrite(ML_Ctrl,LOW);//set direction control pin of B motor to LOW
    analogWrite(ML_PWM,200);//set PWM control speed of B motor to 200
    digitalWrite(MR_Ctrl,HIGH);//set direction control pin of A motor to HIGH 
    analogWrite(MR_PWM,200);//set PWM control speed of A motor to 200
    delay(100);
}
void car_right_turn(){   //car turns right
/*  Serial.println("Right"); */
   //one side gos back and other gos front in the same speed so ti rurns on the spot
    digitalWrite(ML_Ctrl,HIGH);//set direction control pin of B motor to HIGH 
    analogWrite(ML_PWM,200);//set PWM control speed of B motor to 200
    digitalWrite(MR_Ctrl,LOW);//set direction control pin of A motor to LOW
    analogWrite(MR_PWM,200);//set PWM control speed of A motor to 200
    delay(100);
}
void car_left(){   //car turns left
   //lowers the motor speed of side so it turns left
    analogWrite(MR_PWM,255);
    analogWrite(ML_PWM,70);
    delay(100);
}
void car_right(){   //car turns right
     //lowers the motor speed of side so it turns right
    analogWrite(MR_PWM,70);
    analogWrite(ML_PWM,255);
    delay(100);
}
void car_Stop(){   //car stops
/*  Serial.println("Stop"); */
    analogWrite(ML_PWM,0);//set PWM control speed of B motor to 0
    analogWrite(MR_PWM,0);//set PWM control speed of A motor to 0
}
void BRUSH_M(){
   //checks if brush is 1 or 0 and chenge it to the other one 
    if(brush == 0){
    brush = 1;
    digitalWrite(MB_Ctrl,HIGH);   //sets the output of pin 3 to high
    delay(100);   //delays the the program os no other eror signals interrupt the control of bruch motors
  }
  else{
    brush = 0;
    digitalWrite(MB_Ctrl,LOW);   //sets the output of pin 3 to low
    delay(100);
  }
}
void stepForward(){
  digitalWrite(ML_Ctrl,HIGH);//set direction control pin of B motor to HIGH 
  analogWrite(ML_PWM,120);//Set PWM control speed of B motor to 20
  digitalWrite(MR_Ctrl,HIGH);//set direction control pin of A motor to HIGH 
  analogWrite(MR_PWM,120);//Set PWM control speed of A motor to 20
  delay(1170);
  analogWrite(ML_PWM,0);//set PWM control speed of B motor to 200
  analogWrite(MR_PWM,0);//set PWM control speed of A motor to 200
  delay(300);
}
void stepRight(){
  digitalWrite(ML_Ctrl,HIGH);//set direction control pin of B motor to LOW
  analogWrite(ML_PWM,160);//set PWM control speed of B motor to 200
  digitalWrite(MR_Ctrl,LOW);//set direction control pin of A motor to HIGH 
  analogWrite(MR_PWM,160);//set PWM control speed of A motor to 200
  delay(950);
  analogWrite(ML_PWM,0);//set PWM control speed of B motor to 200
  analogWrite(MR_PWM,0);//set PWM control speed of A motor to 200
  delay(300);
}
void stepLeft(){
  digitalWrite(ML_Ctrl,LOW);//set direction control pin of B motor to LOW
  analogWrite(ML_PWM,150);//set PWM control speed of B motor to 200
  digitalWrite(MR_Ctrl,HIGH);//set direction control pin of A motor to HIGH 
  analogWrite(MR_PWM,150);//set PWM control speed of A motor to 200
  
  Serial.println ("Front");
  distensForward = distensFront();  
  distensToTheRight = distensRight();
  Serial.println (distensForward);
  dif = 200;//abs(distensForward - distensRight());
  while (dif > 4){
//    Serial.println (distensRight());
    analogWrite(ML_PWM,150);//set PWM control speed of B motor to 200
    analogWrite(MR_PWM,150);//set PWM control speed of A motor to 200
    delay(100);     
//    Serial.println (distensForward);  
    analogWrite(ML_PWM,0);//set PWM control speed of B motor to 200
    analogWrite(MR_PWM,0);//set PWM control speed of A motor to 200
    
    Serial.println ("***");
    Serial.println (distensForward);
    distensToTheRight = distensRight();
    Serial.println (distensToTheRight);
    dif = abs((distensForward - distensToTheRight));    
    Serial.println (dif);  
  }
  //Serial.println (distensForward);  
  analogWrite(ML_PWM,0);//set PWM control speed of B motor to 200
  analogWrite(MR_PWM,0);//set PWM control speed of A motor to 200
  delay(300); 
}



long distensFront() {
  long durationFront;
  digitalWrite(trigPinFront, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPinFront, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinFront, LOW);
 
  pinMode(echoPinFront, INPUT);
  durationFront = pulseIn(echoPinFront, HIGH);
 
  return (durationFront/2) * 0.0343; 
}

long distensRight() {
  long durationRight;
  digitalWrite(trigPinRight, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPinRight, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinRight, LOW);
 
  pinMode(echoPinRight, INPUT);
  durationRight = pulseIn(echoPinRight, HIGH);
 
  return (durationRight/2) * 0.0343; 
}
