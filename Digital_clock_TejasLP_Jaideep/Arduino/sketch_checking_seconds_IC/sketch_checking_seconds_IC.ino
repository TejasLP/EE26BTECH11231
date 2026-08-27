const int pinA = 2; 
const int pinB = 3; 
const int pinC = 4; 
const int pinD = 5; 

const int fixTailA = 6; 
const int fixTailD = 7; 

void setup() {
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);

  
  pinMode(fixTailA, INPUT);
  pinMode(fixTailD, INPUT);
}

void loop() {
  for (int digit = 0; digit <= 9; digit++) {
    displayDigit(digit);
    delay(1000);
  }
}

void displayDigit(int number) {
  
  for (int bit = 0; bit < 4; bit++) {
    int bitValue = (number >> bit) & 1;
    digitalWrite(2 + bit, bitValue);
  }

  
  if (number == 6) {
    pinMode(fixTailA, OUTPUT);
    digitalWrite(fixTailA, LOW); 
  } else {
    pinMode(fixTailA, INPUT);    
  }

  
  if (number == 9) {
    pinMode(fixTailD, OUTPUT);
    digitalWrite(fixTailD, LOW); 
  } else {
    pinMode(fixTailD, INPUT);    
  }
}