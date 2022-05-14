//A lot of unused functions used while testing to see the most effective sotware organization
/*Components
 * Servo Motor = Tail
 * Motors (Use an H bridge to make forward and backwards
 * Ultrasonic sensors to see infront of her
 * Audio output
 * Photosensor --> Physically attach to the servo motor
 * LEDS
 
 * Connect push buttons for start and stop functionality?
 * Connect a DC motor 
 */

 /*
  * TO DO
  * 1. ULTRASONIC
  *   a. GLUE ultrasonic                    (D)
  *   b. TEST ultrasonic data               (D)
  *   c. TEST ultrasonic with program       (D)
  *   
  * 2. Buzzer, LED, and Servo
  *   a. CONNECT buzzer                     (D)
  *   b. CONNECT LED                        (D)
  *   c. GLUE Servo                         (D)
  *   b. TEST components with program       (D)
  *   
  * 3. Side Dispenser
  *   a. BUILD the attachment               (D)
  *   b. PHYSICALLY attach it to robot      (D)
  *   c. CONEECT to motor                   (ND)
  *   d. TEST with program                  (ND)
  */
#include <Servo.h>

/*Global Variables*/
const int distanceThreshold = 100; //cm
const double LDRThreshold = 10;
const double reverseTimeThreshold = 50;
double distance = 0;
int iterations = 0;
int defaultSpeed = 100;

/*Pin Assignments*/

//Servo Motor
Servo tail;
const int tailPin = 9;

const int echoPin = 8; //Ultrasonic Sensor
const int trigPin = 2; //Ultrasonic Sensor

const int LDRPin = A0; //LDR analog pin

const int audioPin = 12; //Audio Output

const int leftMotorB = 5; //Left Motor Backwards
const int leftMotorF = 6; //Left Motor Forwards
const int rightMotorB = 3; //Right Motor Backwards
const int rightMotorF = 11; //Right Motor Forwards



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  tail.attach(tailPin); //Servo Motor
  
  //Ultrasonic Sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //Motors
  pinMode(leftMotorF, OUTPUT);
  pinMode(leftMotorB, OUTPUT);
  pinMode(rightMotorF, OUTPUT);
  pinMode(rightMotorB, OUTPUT);

  pinMode(LDRPin, INPUT); //LDR

  pinMode(audioPin, OUTPUT); //Speaker
}

void loop() {
  // put your main code here, to run repeatedly:
  drive('f', defaultSpeed, defaultSpeed);
  /*Serial.println(distance);
  findDistance();
  
  if(distance < distanceThreshold) //There is something in front
  {
    while(analogRead(LDRPin) < 650)
    {
      Serial.println(analogRead(LDRPin));
      drive('b', defaultSpeed, defaultSpeed);
    }
    //checkDistance();
    stopProgram();
  }*/
 checkInFront();
}

void drive(char dir, int dutyCycleR, int dutyCycleL)
{
  switch(dir) 
  {
    case 'f':
      analogWrite(leftMotorF, dutyCycleL);
      digitalWrite(leftMotorB, LOW);
      analogWrite(rightMotorF, dutyCycleR);
      digitalWrite(rightMotorB, LOW);
      break;
    case 'b':
      digitalWrite(leftMotorF, LOW);
      analogWrite(leftMotorB, dutyCycleL);
      digitalWrite(rightMotorF, LOW);
      analogWrite(rightMotorB, dutyCycleR);
      break;
    case 'r':
      analogWrite(leftMotorF, dutyCycleL);
      digitalWrite(leftMotorB, LOW);
      digitalWrite(rightMotorF, LOW);
      analogWrite(rightMotorB, dutyCycleR);
      break;
    case 'l':
      digitalWrite(leftMotorF, LOW);
      analogWrite(leftMotorB, dutyCycleL);
      analogWrite(rightMotorF, dutyCycleR);
      digitalWrite(rightMotorB, LOW);
      break;
    case 's':
      digitalWrite(leftMotorF, LOW);
      digitalWrite(leftMotorB, LOW);
      digitalWrite(rightMotorF, LOW);
      digitalWrite(rightMotorB, LOW);
      break;
  }
  
}

void stopProgram()
{
  drive('s', 0, 0);
  tone(audioPin, 275);
  for(int i = 0; i < 180; i++) 
  {
    tail.write(i);
    delay(10);
  }
  for(int i = 180; i > 0; i--) 
  {
    tail.write(i);
    delay(10);
  }
  noTone(audioPin);
  exit(0);
}

void checkInfront()
{
  findDistance();
  if(distance < distanceThreshold)//This means that there is something infront of her
  {
    drive('s', 0, 0); //Stop the motors from running
    delay(500);
    if(canTurn('l') == false)//if there is something in front of her while turning left
    {
      delay(250);
      if(canTurn('r') == false)//if there is something in front of her while turning right
      {
        delay(250);
        goBack();
      }
    }
  }
}
void goBack()
{
  int timeElapsed = 0;
  Serial.println("REVERSE");
  drive('b', 100, 100);
  while(analogRead(LDRPin) < 650 || timeElapsed <= reverseTimeThreshold) //op amp = logic low and
  {
    timeElapsed++;
  }
  drive('s', 0, 0); //Stop
  if(analogRead(LDRPin) > 650 ) stuck(); //If the LDR exited the loop because it was shaded in the back while reversing then she is stuck
  else
  {
    drive('r', 100, 100); //turn 180 degrees to the right
    delay(1000);
    drive('s', 0, 0); //Stop
  }
}
bool canTurn(char dir)
{
  bool ret = true;
  Serial.println(dir);
  drive(dir, 100, 100);//Turn left
  delay(250);
  drive('s', 0, 0); //Stop
  findDistance();
  if(distance < distanceThreshold) //there is something in front of her
  {
    ret = false;
  }
  return ret;
}
void stuck()
{
  Serial.println("STUCK");
  //Make a beep, wag tail
  tone(audioPin, 275);
  
  for(int i = 0; i < 180; i++) 
  {
    tail.write(i);
    delay(10);
  }
  for(int i = 180; i > 0; i--) 
  {
    tail.write(i);
    delay(10);
  }
  noTone(audioPin);
  exit(0); //Shut Down
}
void findDistance()
{
  //Reset trigPin
  digitalWrite(trigPin, LOW);
  delay(0.005);

  //Set the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delay(0.001);
  digitalWrite(trigPin, LOW);

  // Read echoPin, returns the sound wave travel time in microseconds
  double duration = pulseIn(echoPin, HIGH);

  //Calculate distance: Speed of sound wave divided by 2 (go and back)
  distance = duration * 0.034 / 2;
}




/*void checkDistance(int iteration)
{
  int timeElapsed = 0;
  findDistance();
  Serial.println(distance);
  if(distance < distanceThreshold)//This means that there is something infront of her
  {
    drive('s', 0, 0); //Stop the motors from running
    Serial.println("Stopped");
    if(iteration == 1) //Look to the left to see if there is an obstacle there
    {
      Serial.println("Turning Left");
      drive('l', 250, 250);//Turn left
      delay(250);
      drive('s', 0, 0); //Stop
      checkDistance(2); //Check to see if there is an obstacle at new location
    }
    else if(iteration == 2) //Look to the right to see if there is an obstacle there
    {
      Serial.println("Turning RIGHT");
      drive('r', 250, 250); //Turn right
      delay(500);
      drive('s', 0, 0); //Stop
      //checkDistance(3); //Check to see if there is an obstacle at new location
    }
    else if(iteration == 3) //Since there are obstacles on both sides, reverse
    {
      drive('l', 250, 250); //Get back to inital position
      delay(250);
      drive('s', 0, 0);
      drive('b', 255, 255); //Drive backwards
      
      //Reverse for a specific time as long as LDR is not shaded: loop will exit when there is something in front of the LDR OR she has reversed for an alloted time
      while(analogRead(LDRPin) < 650 || timeElapsed <= reverseTimeThreshold) //op amp = logic low
      {
        timeElapsed++;
      }
      
      drive('s', 0, 0); //Stop
      
      if(analogRead(LDRPin) > 650 )stuck(); //If the LDR is shaded, then she is stuck, op amp = logic high
      else
      {
        drive('r', 255, 255); //turn 180 degrees to the right
        delay(1000); 
        drive('s', 0, 0); //Stop
      }
    }
  }
}*/
/*bool LDRShaded()
{
  bool ret = false;
  if(analogRead(LDRPin) > LDRThreshold)ret = true; //If the LDR reading is exceeds the defined threshold, then there is something behind her
  return ret;
}*/
