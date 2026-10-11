#include <Arduino.h>

// --- Pin Configurations (ESP32 Dev Board) ---
const int MOTOR_IN1 = 12;   // Connects to DRV8833 IN1
const int MOTOR_IN2 = 13;   // Connects to DRV8833 IN2
const int ENCODER_A = 18;   // Interrupt-capable pin for Channel A
const int ENCODER_B = 19;   // Channel B pin for direction tracking

// --- ESP32 PWM Channel Configurations ---
const int PWM_FREQ = 5000;       // 5 kHz frequency
const int PWM_RESOLUTION = 8;    // 8-bit resolution (0-255 values)
const int IN1_CHANNEL = 0;       // LEDC channel 0
const int IN2_CHANNEL = 1;       // LEDC channel 1

// --- Motor & Encoder Calculations ---
// 7 PPR base encoder * 50:1 Gearbox Ratio = 350 ticks per single output revolution
const float COUNTS_PER_REV = 350.0; 

const float TARGET_FWD_REVS = 30.0;
const float TARGET_BWD_REVS = 15.0;

const long TARGET_COUNTS_FWD = TARGET_FWD_REVS * COUNTS_PER_REV;
const long TARGET_COUNTS_BWD = TARGET_BWD_REVS * COUNTS_PER_REV;

// --- Global Tracking Variables ---
volatile long encoderTicks = 0;
long startTicks = 0;

// --- ISR (Interrupt Service Routine) ---
// Placed in IRAM for high-speed execution safety on ESP32
void IRAM_ATTR updateEncoderISR() {
  if (digitalRead(ENCODER_A) == digitalRead(ENCODER_B)) {
    encoderTicks++;
  } else {
    encoderTicks--;
  }
}

// --- Motor Driving Functions ---
void moveForward(int speed) {
  ledcWrite(IN1_CHANNEL, speed);
  ledcWrite(IN2_CHANNEL, 0);
}

void moveBackward(int speed) {
  ledcWrite(IN1_CHANNEL, 0);
  ledcWrite(IN2_CHANNEL, speed);
}

void stopMotor() {
  // Pull both inputs HIGH for active braking configuration on the DRV8833
  ledcWrite(IN1_CHANNEL, 255);
  ledcWrite(IN2_CHANNEL, 255);
}

void setup() {
  Serial.begin(115200); // Standard ESP32 serial communication rate

  // Configure ESP32 LEDC PWM Outputs
  ledcSetup(IN1_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(IN2_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  
  ledcAttachPin(MOTOR_IN1, IN1_CHANNEL);
  ledcAttachPin(MOTOR_IN2, IN2_CHANNEL);

  // Configure Encoders with Internal Pull-Ups
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);

  // Attach Hardware Interrupt to watch both RISING and FALLING edge shifts
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), updateEncoderISR, CHANGE);

  delay(3000); // Pause to allow safe serial monitor attachment
  Serial.println("System Initialized. Initiating Swarm Sequence...");

  // --- Step 1: Move Forward 30 Rotations ---
  Serial.print("Target forward ticks calculation: ");
  Serial.println(TARGET_COUNTS_FWD);
  
  startTicks = encoderTicks;
  moveForward(200); // Run at ~80% power duty cycle

  while (abs(encoderTicks - startTicks) < TARGET_COUNTS_FWD) {
    // Keep driving until full count quota is completed
    yield(); // Keeps background ESP32 core watchdog systems happy
  }
  stopMotor();
  Serial.println("Forward Milestone Complete.");
  delay(1500); 

  // --- Step 2: Move Backward 15 Rotations ---
  Serial.print("Target backward ticks calculation: ");
  Serial.println(TARGET_COUNTS_BWD);

  startTicks = encoderTicks;
  moveBackward(200); 

  while (abs(encoderTicks - startTicks) < TARGET_COUNTS_BWD) {
    yield();
  }
  stopMotor();
  Serial.println("Backward Milestone Complete. Motion sequence resting.");
}

void loop() {
  // Sequence operates entirely inside setup execution.
}
