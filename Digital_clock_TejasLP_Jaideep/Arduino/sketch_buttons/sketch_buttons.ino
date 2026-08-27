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
const int enableHrUnits  = 12; 
const int enableHrTens   = A0; 


const int btnEdit = A1; 
const int btnNext = A2; 
const int btnInc  = A3; 
const int btnDec  = A4; 


const int TAIL_A_ON_TIME = 1200; 
const int TAIL_D_ON_TIME = 400;  


int seconds = 0;
int minutes = 0;
int hours   = 0;
unsigned long lastSecondTime = 0;


bool editMode = false;
int selectedDigit = 0; 
unsigned long lastBlinkTime = 0;
bool blinkState = true;


bool lastEditState = HIGH;
bool lastNextState = HIGH;
bool lastIncState  = HIGH;
bool lastDecState  = HIGH;

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
  pinMode(enableHrUnits, OUTPUT);
  pinMode(enableHrTens, OUTPUT);

  
  pinMode(btnEdit, INPUT_PULLUP);
  pinMode(btnNext, INPUT_PULLUP);
  pinMode(btnInc,  INPUT_PULLUP);
  pinMode(btnDec,  INPUT_PULLUP);

  
  blankAllDisplays();
}

void loop() {
  handleButtons();

  
  if (!editMode) {
    if (millis() - lastSecondTime >= 1000) {
      lastSecondTime = millis();
      seconds++;
      if (seconds >= 60) {
        seconds = 0;
        minutes++;
        if (minutes >= 60) {
          minutes = 0;
          hours++;
          if (hours >= 24) {
            hours = 0;
          }
        }
      }
    }
  } else {
    
    lastSecondTime = millis();
    
    
    if (millis() - lastBlinkTime >= 200) {
      lastBlinkTime = millis();
      blinkState = !blinkState;
    }
  }

  refreshDisplay();
}

void handleButtons() {
  bool currEdit = digitalRead(btnEdit);
  bool currNext = digitalRead(btnNext);
  bool currInc  = digitalRead(btnInc);
  bool currDec  = digitalRead(btnDec);

  
  if (lastEditState == HIGH && currEdit == LOW) {
    delay(50); 
    editMode = !editMode;
    selectedDigit = 0; 
    blinkState = true;
  }

  
  if (editMode) {
    
    if (lastNextState == HIGH && currNext == LOW) {
      delay(50);
      selectedDigit = (selectedDigit + 1) % 6;
      blinkState = true;
    }

    
    if (lastIncState == HIGH && currInc == LOW) {
      delay(50);
      adjustDigit(1);
    }

    
    if (lastDecState == HIGH && currDec == LOW) {
      delay(50);
      adjustDigit(-1);
    }
  }

  lastEditState = currEdit;
  lastNextState = currNext;
  lastIncState  = currInc;
  lastDecState  = currDec;
}

void adjustDigit(int dir) {
  int secU = seconds % 10, secT = seconds / 10;
  int minU = minutes % 10, minT = minutes / 10;
  int hrU  = hours % 10,   hrT  = hours / 10;

  switch (selectedDigit) {
    case 0: 
      secU = (secU + dir + 10) % 10;
      break;
    case 1: 
      secT = (secT + dir + 6) % 6;
      break;
    case 2: 
      minU = (minU + dir + 10) % 10;
      break;
    case 3: 
      minT = (minT + dir + 6) % 6;
      break;
    case 4: 
      hrU += dir;
      if (hrU > 9) hrU = 0;
      if (hrU < 0) hrU = 9;
      if (hrT * 10 + hrU > 23) hrU = (dir > 0) ? 0 : 3;
      break;
    case 5: 
      hrT += dir;
      if (hrT > 2) hrT = 0;
      if (hrT < 0) hrT = 2;
      if (hrT * 10 + hrU > 23) hrT = (dir > 0) ? 0 : 2;
      break;
  }

  seconds = secT * 10 + secU;
  minutes = minT * 10 + minU;
  hours   = hrT  * 10 + hrU;
}

void refreshDisplay() {
  int secUnits = seconds % 10;
  int secTens  = seconds / 10;
  int minUnits = minutes % 10;
  int minTens  = minutes / 10;
  int hrUnits  = hours % 10;
  int hrTens   = hours / 10;

  
  if (!(editMode && selectedDigit == 5 && !blinkState)) showDigit(hrTens, enableHrTens);
  if (!(editMode && selectedDigit == 4 && !blinkState)) showDigit(hrUnits, enableHrUnits);
  if (!(editMode && selectedDigit == 3 && !blinkState)) showDigit(minTens, enableMinTens);
  if (!(editMode && selectedDigit == 2 && !blinkState)) showDigit(minUnits, enableMinUnits);
  if (!(editMode && selectedDigit == 1 && !blinkState)) showDigit(secTens, enableSecTens);
  if (!(editMode && selectedDigit == 0 && !blinkState)) showDigit(secUnits, enableSecUnits);
}

void showDigit(int digitValue, int enablePin) {
  blankAllDisplays();

  
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

void blankAllDisplays() {
  digitalWrite(enableSecUnits, LOW);
  digitalWrite(enableSecTens, LOW);
  digitalWrite(enableMinUnits, LOW);
  digitalWrite(enableMinTens, LOW);
  digitalWrite(enableHrUnits, LOW);
  digitalWrite(enableHrTens, LOW);
  
  pinMode(fixTailA, INPUT);
  pinMode(fixTailD, INPUT);
}