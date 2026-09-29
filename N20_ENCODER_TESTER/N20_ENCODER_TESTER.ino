#define ENC_A 2
#define ENC_B 3

volatile long encoderTicks = 0;

const long COUNTS_PER_REV = 600;  // Change to your encoder's actual value

long revolutionCount = 0;

void setup() {
  Serial.begin(115200);

  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);

  Serial.println("N20 Encoder Test");
  Serial.println("Rotate the motor...");
}

void loop() {
  static int lastA = LOW;

  int A = digitalRead(ENC_A);
  int B = digitalRead(ENC_B);

  // Detect A rising edge
  if (A == HIGH && lastA == LOW) {

    if (B == LOW) {
      encoderTicks++;
    } 
    else {
      encoderTicks--;
    }
  }

  lastA = A;

  // Convert ticks to complete revolutions
  long newRevolutionCount = encoderTicks / COUNTS_PER_REV;

  if (newRevolutionCount != revolutionCount) {
    revolutionCount = newRevolutionCount;

    Serial.print("Revolutions: ");
    Serial.println(revolutionCount);
  }
}