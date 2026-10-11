#define MotFwd  3  // Motor Forward pin
#define MotRev  4 // Motor Reverse pin

int encoderPin1 = 2; // Encoder Output A
int encoderPin2 = 3; // Encoder Output B
volatile int lastEncoded = 0;
volatile long encoderValue = 0;

void setup() {
  pinMode(MotFwd, OUTPUT);
  pinMode(MotRev, OUTPUT);
  Serial.begin(9600);

  pinMode(encoderPin1, INPUT_PULLUP);
  pinMode(encoderPin2, INPUT_PULLUP);

  digitalWrite(encoderPin1, HIGH);
  digitalWrite(encoderPin2, HIGH);

  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);
}

void loop() {
  for (int i = 0; i <= 500; i++) {
    digitalWrite(MotFwd, LOW);
    digitalWrite(MotRev, HIGH);
    Serial.print("Forward  ");
    Serial.println(encoderValue);
  }

  delay(1000);

  for (int i = 0; i <= 500; i++) {
    digitalWrite(MotFwd, HIGH);
    digitalWrite(MotRev, LOW);
    Serial.print("Reverse  ");
    Serial.println(encoderValue);
  }

  delay(1000);
}

void updateEncoder() {
  int MSB = digitalRead(encoderPin1);
  int LSB = digitalRead(encoderPin2);

  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue--;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue++;

  lastEncoded = encoded;
}
