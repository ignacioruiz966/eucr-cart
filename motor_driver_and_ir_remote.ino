#include <IRremote.hpp>


// Ultrasonic Pins
const int trigPin = 9;
const int echoPin = 10;

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



// Ultrasonic measurements
long duration;
int distance;


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


void setup() {
  Serial.begin(9600);

  // start the receiver
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);

  motorStartup();
  
}

void loop() {
  /*
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance >= 50) {
    digitalWrite(IN1, HIGH);
  }
  else
  {
    digitalWrite(IN1, LOW);
  }

  delay(500);
  */

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
      
    }
     else if (IrInput == LEFT) {
      
    }
    motorBrake();

    IrReceiver.resume();
  }

  /*

  motorForward();
  delay(2000);

  // Stop motor
  motorBrake();
  delay(1000);

  // Turn motor reverse at half speed
  motorBackward();
  delay(2000);
  */

}


