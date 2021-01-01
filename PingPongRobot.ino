#include<Servo.h>

Servo pan;
Servo tilt;
Servo trigger;

const int launchButtonPin = 2;
const int autoButtonPin = 3;
const int inRangeIndicator = 7;
const int outOfRangeIndicator = 8;
const int autoIndicator = 4;

const int trigPin = 10;
const int echoPin = 11;

int launchState = 0;
int autoState = 0;

boolean autoStage = false;
boolean hasLaunched = false;
boolean inRange = false;

float duration;
float distance;

int maxShot = 450; //cm
int minShot = 190;

int panPosition = 0;
boolean panPositive = true;

void setup() {
  pan.attach(9);
  tilt.attach(12);
  trigger.attach(6);
  Serial.begin(9600);
  Serial.setTimeout(10);

  pinMode(launchButtonPin, INPUT);
  pinMode(autoButtonPin, INPUT);
  pinMode(inRangeIndicator, OUTPUT);
  pinMode(outOfRangeIndicator, OUTPUT);
  pinMode(autoIndicator, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  trigger.write(0);
  delay(50);

  pan.write(0);
}

void loop() {

  distance = getDistance();
  delay(50);

  //Pan back and forth
  if (panPosition > 137)
    panPositive = false;

  if (panPosition == 0)
    panPositive = true;

  if (panPositive && panPosition < 138)
  {
    panPosition += 6;
    pan.write(panPosition);
  }
  if (panPositive == false && panPosition > 0)
  {
    panPosition -= 6;
    pan.write(panPosition);
  }
  delay(120);

  //Set Auto LED
  if (autoStage)
    digitalWrite(autoIndicator, HIGH);
  else
    digitalWrite(autoIndicator, LOW);

  //Check if in range
  if (distance < maxShot && distance > minShot && distance > 220)
  {
    inRange = true;

    tilt.write(180 - (distance - minShot) / (maxShot - minShot) * 180);
    delay(1500);
  }
  else
  {
    inRange = false;
  }

  //Get button states
  launchState = digitalRead(launchButtonPin);
  autoState = digitalRead(autoButtonPin);

  //Set in-range LEDs
  if (inRange)
  {
    digitalWrite(inRangeIndicator, HIGH);
    digitalWrite(outOfRangeIndicator, LOW);
  }
  else
  {
    digitalWrite(inRangeIndicator, LOW);
    digitalWrite(outOfRangeIndicator, HIGH);
  }

  //Manual launch
  if (launchState == HIGH && autoStage == false && inRange && autoState == LOW)
    launch();

  //Auto launch
  if (autoStage && inRange && hasLaunched == false)
  {
    launch();
    hasLaunched = true;
  }

  //Re-enable auto firing
  if (autoStage && launchState == HIGH && autoState == LOW)
    hasLaunched = false;

  //Toggle auto mode
  if (autoState == HIGH && launchState == LOW)
  {
    delay(50);
    if (autoStage)
      autoStage = false;
    else
      autoStage = true;
  }

  //Max launch
  if (launchState == HIGH && autoState == HIGH)
  {
    tilt.write(0);
    //pan.write(75);
    delay(700);
    launch();
  }

}

float getDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * .0343) / 2;
  
  return distance;
}

void launch()
{
  digitalWrite(inRangeIndicator, LOW);
  digitalWrite(outOfRangeIndicator, LOW);
  digitalWrite(autoIndicator, LOW);

  //Flash LEDs
  delay(500);
  digitalWrite(inRangeIndicator, HIGH);
  delay(500);
  digitalWrite(outOfRangeIndicator, HIGH);
  delay(500);
  digitalWrite(autoIndicator, HIGH);

  delay(500);
  digitalWrite(inRangeIndicator, LOW);
  digitalWrite(outOfRangeIndicator, LOW);
  digitalWrite(autoIndicator, LOW);

  delay(500);
  digitalWrite(inRangeIndicator, HIGH);
  digitalWrite(outOfRangeIndicator, HIGH);
  digitalWrite(autoIndicator, HIGH);

  //Launch
  trigger.write(35);

  //Flash Again
  delay(1500);
  digitalWrite(autoIndicator, LOW);
  delay(1000);
  digitalWrite(outOfRangeIndicator, LOW);
  delay(1000);
  digitalWrite(inRangeIndicator, LOW);

  //Reset trigger
  trigger.write(0);
  delay(500);
}
