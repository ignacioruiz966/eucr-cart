#include <IRremote.hpp>
#include <Servo.h>

// Ultrasonic Pins
const int TRIG_PIN = 6;
const int ECHO_PIN = 7;

// Filtering US samples
const int FILTER_SAMPLES = 5;
const float SPEED_OF_SOUND = 0.0343;

// KY-022 signal pin
const int RECV_PIN = 8;

// IR Remote codes
const unsigned long UP = 0xE718FF00;
const unsigned long DOWN = 0xAD52FF00;
const unsigned long RIGHT = 0xA55AFF00;
const unsigned long LEFT = 0xF708FF00;

// motor PWM pins
const int ENA = 5;
const int ENB = 3;


// motor directional pins
const int IN1 = 9;
const int IN2 = 10;
const int IN3 = 11;
const int IN4 = 12;

// SERVO 
Servo myservo;
const int SERVO_PIN = 4;
int pos = 45;





void motorStartup(){
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void motorForward(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, 255);
  analogWrite(ENB, 255);
}

void motorBrake(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}


void motorBackward(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, 255);
  analogWrite(ENB, 255); 
}

void motorLeft(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(ENA, 255);
  analogWrite(ENB, 255); 
}

void motorRight(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, 255);
  analogWrite(ENB, 255); 
}

float getFilteredDistance() {

  // Sends pulses and records them
  long samples[FILTER_SAMPLES];

  for (int i = 0; i < FILTER_SAMPLES; i++) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    int duration = pulseIn(ECHO_PIN, HIGH, 26000);

    if (duration == 0) {
      samples[i] = 99999;
    }
    else {
      samples[i] = duration;
    }

    delay(10);

  }


  // Simple Bubble Sort
  for (int i = 0; i < FILTER_SAMPLES - 1; i++) {
    for (int j = 0; j < FILTER_SAMPLES - i - 1; j++) {
      if (samples[j] > samples[j + 1]) {
        long temp = samples[j];
        samples[j] = samples[j + 1];
        samples[j + 1] = temp;
      }
    }
  }


  // returns the median duration
  long medianDuration = samples[FILTER_SAMPLES / 2];

  if (medianDuration == 99999) {
    return -1.0;
  }

  return (medianDuration * SPEED_OF_SOUND) / 2.0;

}


void setup() {
  
  Serial.begin(9600);

  // US Setup
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  // Servo Setup
  myservo.attach(SERVO_PIN);

  // Starts the receiver
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);

  motorStartup();
  
}

void loop() {

  float currentDistance = getFilteredDistance();


  // WIP Servo Code
  for (pos = 45; pos <= 135; pos += 15) {
    myservo.write(pos);

    if (currentDistance < 0){
    Serial.println("Error: Out of range or timed out");
  } 
  else {
    Serial.println("Filtered Distance");
    Serial.println(currentDistance);
    Serial.print(" cm\n");

  }

  delay(500);

  }
  for (pos = 135; pos >= 45; pos -= 15) {
    myservo.write(pos);

    if (currentDistance < 0){
    Serial.println("Error: Out of range or timed out");
  } 
  else {
    Serial.println("Filtered Distance");
    Serial.println(currentDistance);
    Serial.print(" cm\n");

  }

  delay(500);
  
  }

  
  // Listens to IR remote & runs motor functions
  if (IrReceiver.decode()) {
    unsigned long IrInput = IrReceiver.decodedIRData.decodedRawData;
    if (IrInput == UP){
      motorForward();
      delay(2000);

    }
    else if (IrInput == DOWN) {
      motorBackward();
      delay(2000);

    }
     else if (IrInput == RIGHT) {
      motorRight();
      delay(2000);

    }
     else if (IrInput == LEFT) {
      motorLeft();
      delay(2000);
    }
    motorBrake();

    IrReceiver.resume();
  }
  
}


