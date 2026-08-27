const int pinA = 2; 
const int pinB = 3;
const int pinC = 4;
const int pinD = 5; 


const int fixTailA = 6; 
const int fixTailD = 7; 


const int enableSecUnits = 8;  
const int enableSecTens  = 9;  
const int enableMinUnits = 10; 
const int enableMinTens  = 11; 



const int TAIL_A_ON_TIME = 1200; 
const int TAIL_D_ON_TIME = 400;  

int seconds = 0;
int minutes = 0;
unsigned long lastSecondTime = 0;

void setup() {
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);

  pinMode(fixTailA, INPUT);
  pinMode(fixTailD, INPUT);

  pinMode(enableSecUnits, OUTPUT);
  pinMode(enableSecTens, OUTPUT);
  pinMode(enableMinUnits, OUTPUT);
  pinMode(enableMinTens, OUTPUT);

  digitalWrite(enableSecUnits, LOW);
  digitalWrite(enableSecTens, LOW);
  digitalWrite(enableMinUnits, LOW);
  digitalWrite(enableMinTens, LOW);
}

void loop() {
  
  if (millis() - lastSecondTime >= 100) {
    lastSecondTime = millis();
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0; 
      }
    }
  }

  refreshDisplay();
}

void refreshDisplay() {
  int secUnits = seconds % 10;
  int secTens  = seconds / 10;
  int minUnits = minutes % 10;
  int minTens  = minutes / 10;

  showDigit(minTens, enableMinTens);
  showDigit(minUnits, enableMinUnits);
  showDigit(secTens, enableSecTens);
  showDigit(secUnits, enableSecUnits);
}

void showDigit(int digitValue, int enablePin) {
  
  digitalWrite(enableSecUnits, LOW);
  digitalWrite(enableSecTens, LOW);
  digitalWrite(enableMinUnits, LOW);
  digitalWrite(enableMinTens, LOW);
  
  pinMode(fixTailA, INPUT);
  pinMode(fixTailD, INPUT);

  
  for (int bit = 0; bit < 4; bit++) {
    digitalWrite(2 + bit, (digitValue >> bit) & 1);
  }

  
  digitalWrite(enablePin, HIGH);

  
  if (digitValue == 6) {
    
    pinMode(fixTailA, OUTPUT);
    digitalWrite(fixTailA, LOW);
    delayMicroseconds(TAIL_A_ON_TIME);
    
    pinMode(fixTailA, INPUT); 
    delayMicroseconds(2000 - TAIL_A_ON_TIME);
  } 
  else if (digitValue == 9) {
    
    pinMode(fixTailD, OUTPUT);
    digitalWrite(fixTailD, LOW);
    delayMicroseconds(TAIL_D_ON_TIME); 
    
    pinMode(fixTailD, INPUT); 
    delayMicroseconds(2000 - TAIL_D_ON_TIME);
  } 
  else {
    delay(2); 
  }
}