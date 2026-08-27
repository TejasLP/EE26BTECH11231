const int pinA = 2;
const int pinB = 3;
const int pinC = 4;
const int pinD = 5;
const int pinE = 6;
const int pinF = 7;
const int pinG = 8;
const int pinDP = 9;

const int segmentPins[7] = {pinA, pinB, pinC, pinD, pinE, pinF, pinG};

const byte numbers[10][7] = {

  {0, 0, 0, 0, 0, 0, 1}, 
  {1, 0, 0, 1, 1, 1, 1}, 
  {0, 0, 1, 0, 0, 1, 0}, 
  {0, 0, 0, 0, 1, 1, 0}, 
  {1, 0, 0, 1, 1, 0, 0}, 
  {0, 1, 0, 0, 1, 0, 0}, 
  {0, 1, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 1, 1, 1, 1}, 
  {0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 1, 0, 0}  
};

void setup() {
  
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], HIGH); 
  }
  
  
  pinMode(pinDP, OUTPUT);
  digitalWrite(pinDP, HIGH); 
}

void loop() {
  
  for (int digit = 0; digit <= 9; digit++) {
    displayDigit(digit);
    delay(1000); 
  }
}


void displayDigit(int digit) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], numbers[digit][i]);
  }
}