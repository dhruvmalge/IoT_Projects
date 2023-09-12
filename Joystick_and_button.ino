#include <Servo.h>
Servo servoX;
Servo servoY;

#define servoXPin 2
#define servoYPin 3
#define joystickButton 5
#define VRXPin A0
#define VRYPin A1

//xAxis
const int leftThreshold = 545;
const int rightThreshold = 500;
//y Axis
const int upThreshold = 565;
const int downThreshold = 500;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(joystickButton, INPUT_PULLUP);
  servoX.attach(servoXPin);
  servoY.attach(servoYPin); 

}

void loop() 
{
  // put your main code here, to run repeatedly:
  Serial.begin(9600);
  int xPin = analogRead(VRXPin);
  int yPin = analogRead(VRYPin);
  int joystickbuttonvalue = digitalRead(joystickButton);

  Serial.println(" ");
  Serial.print("X Axis Value : ");
  Serial.print(xPin);
  Serial.print(", Y Axis Value : ");
  Serial.print(yPin);
  delay(100);


  int defaultPosition = 90;
  if (joystickbuttonvalue == HIGH)
  {
    Serial.print("JoystickButton is HIGH");
  }
  if(joystickbuttonvalue == LOW)
  {
    Serial.print("JoystickButton is LOW");
  }
  

  if (xPin <= 545)
  {
    servoX.write(defaultPosition--);
  }
  if (xPin >= 545)
  {
    servoX.write(defaultPosition++);
  }
  if (yPin <= 515)
  {
    servoY.write(defaultPosition++);
  }
  if (yPin >= 515)
  {
    servoY.write(defaultPosition--);
  }
  else
  {
    servoX.write(defaultPosition);
    servoY.write(defaultPosition);
  }
  
}
