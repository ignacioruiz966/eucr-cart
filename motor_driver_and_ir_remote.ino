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

// Motor PWM pins
const int ENA = 5;
const int ENB = 3;

// Motor directional pins
const int IN1 = 9;
const int IN2 = 10;
const int IN3 = 11;
const int IN4 = 12;

// SERVO 
Servo myservo;
const int SERVO_PIN = 4;
int pos = 90;
int servoStep = 15; // Direction of servo sweep (+15 or -15)

bool autoMode = false;

// Non-blocking timer for servo sweep
unsigned long previousMillis = 0;
const long servoInterval = 500; // Interval between servo steps (ms)

void motorStartup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void motorForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 255);
  analogWrite(ENB, 255);
}

void motorBrake() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void motorBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 255);
  analogWrite(ENB, 255); 
}

void motorLeft() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 255);
  analogWrite(ENB, 255); 
}

void motorRight() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 255);
  analogWrite(ENB, 255); 
}

float getFilteredDistance() {
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
    } else {
      samples[i] = duration;
    }
    delay(2); // Short internal delay for sensor stability
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
  myservo.write(pos);

  // Starts the receiver
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);

  motorStartup();
}

void loop() {
  // 1. Instantly check for IR remote commands on every cycle
  if (IrReceiver.decode()) {
    unsigned long IrInput = IrReceiver.decodedIRData.decodedRawData;
    
if (IrInput == UP) {
      // Toggle autonomous mode on/off, or drive forward manually
      autoMode = !autoMode; 
      Serial.print("Autonomous Mode: ");
      Serial.println(autoMode ? "ON" : "OFF");
      if (!autoMode) motorBrake();
      delay(300); // Debounce press
    }
    else if (IrInput == DOWN) {
      autoMode = false; // Override to manual back-up
      motorBackward();
      delay(250);
      motorBrake();
    }
    else if (IrInput == LEFT) {
      autoMode = false; // Override to manual turn left
      motorLeft();
      delay(200);
      motorBrake();
    }
    else if (IrInput == RIGHT) {
      autoMode = false; // Override to manual turn right
      motorRight();
      delay(200);
      motorBrake();
    }
    IrReceiver.resume();
  }

  // 2. Non-blocking Servo Sweep & Distance Logging
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= servoInterval) {
    previousMillis = currentMillis;

    // Move servo position
    pos += servoStep;
    
    // Reverse direction at limits
    if (pos >= 180 || pos <= 0) {
      servoStep = -servoStep;
    }
    
    myservo.write(pos);

    // Read and print distance
    float currentDistance = getFilteredDistance();
    if (currentDistance < 0) {
      Serial.println("Error: Out of range or timed out");
    } else {
      Serial.print("Filtered Distance: ");
      Serial.print(currentDistance);
      Serial.println(" cm");
    }

    if (autoMode) {
    float distance = getFilteredDistance();
    Serial.print("Autonomous Distance: ");
    Serial.println(distance);

    if (distance > 0 && distance < 25) {
      // Obstacle detected! Back up and turn
      motorBrake();
      delay(100);
      motorBackward();
      delay(300);
      motorRight();
      delay(300);
      motorBrake();
    } else {
      // Path is clear, drive forward
      motorForward();
    }

  }
  }
}