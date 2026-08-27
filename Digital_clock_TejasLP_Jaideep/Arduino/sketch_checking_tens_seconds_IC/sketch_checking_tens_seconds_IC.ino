const int pinA = 2; 
const int pinB = 3;
const int pinC = 4;
const int pinD = 5; 


const int fixTailA = 6;
const int fixTailD = 7;


const int enableUnits = 8; 
const int enableTens  = 9; 

int seconds = 0;
unsigned long lastSecondTime = 0;

void setup() {
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);

  pinMode(fixTailA, INPUT);
  pinMode(fixTailD, INPUT);

  pinMode(enableUnits, OUTPUT);
  pinMode(enableTens, OUTPUT);

  
  digitalWrite(enableUnits, LOW);
  digitalWrite(enableTens, LOW);
}

void loop() {
  
  if (millis() - lastSecondTime >= 1000) {
    lastSecondTime = millis();
    seconds++;
    if (seconds >= 60) {
      seconds = 0; 
    }
  }

  
  refreshDisplay();
}

void refreshDisplay() {
  int tens = seconds / 10;
  int units = seconds % 10;

  
  showDigit(tens, enableTens);
  delay(3); 

  
  showDigit(units, enableUnits);
  delay(3); 
}

void showDigit(int digitValue, int enablePin) {
  
  digitalWrite(enableUnits, LOW);
  digitalWrite(enableTens, LOW);

  
  for (int bit = 0; bit < 4; bit++) {
    digitalWrite(2 + bit, (digitValue >> bit) & 1);
  }

  
  if (digitValue == 6) {
    pinMode(fixTailA, OUTPUT);
    digitalWrite(fixTailA, LOW);
  } else {
    pinMode(fixTailA, INPUT);
  }

  if (digitValue == 9) {
    pinMode(fixTailD, OUTPUT);
    digitalWrite(fixTailD, LOW);
  } else {
    pinMode(fixTailD, INPUT);
  }

  
  digitalWrite(enablePin, HIGH);
}