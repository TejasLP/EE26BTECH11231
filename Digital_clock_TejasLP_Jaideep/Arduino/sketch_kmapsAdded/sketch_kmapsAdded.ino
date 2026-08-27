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


const int btnEdit  = A1; 
const int btnNext  = A2; 
const int btnInc   = A3; 
const int btnDec   = A4; 
const int btnTimer = A5; 


const int TAIL_A_ON_TIME = 1200; 
const int TAIL_D_ON_TIME = 1400;  


enum DisplayMode { MODE_CLOCK, MODE_TIMER };
DisplayMode currentMode = MODE_CLOCK;


int clockSec = 0, clockMin = 0, clockHr = 0;
unsigned long lastClockTick = 0;


int timerSec = 0, timerMin = 0, timerHr = 0;
int initTimerSec = 0, initTimerMin = 0, initTimerHr = 0;
bool timerRunning = false;
bool timerAlarm = false;
unsigned long lastTimerTick = 0;


bool editMode = false;
int selectedDigit = 0; 
unsigned long lastBlinkTime = 0;
bool blinkState = true;


bool lastEditState  = HIGH;
bool lastNextState  = HIGH;
bool lastIncState   = HIGH;
bool lastDecState   = HIGH;
bool lastTimerState = HIGH;





int next_mod10(int val) {
  bool W = (val >> 0) & 1; bool X = (val >> 1) & 1; 
  bool Y = (val >> 2) & 1; bool Z = (val >> 3) & 1;
  bool A = !W;
  bool B = (W && !X && !Z) || (!W && X);
  bool C = (!X && Y) || (!W && Y) || (W && X && !Y);
  bool D = (!W && Z) || (W && X && Y);
  return (D << 3) | (C << 2) | (B << 1) | A;
}

int next_mod6(int val) {
  bool W = (val >> 0) & 1; bool X = (val >> 1) & 1; bool Y = (val >> 2) & 1;
  bool A = !W;
  bool B = (W && !X && !Y) || (!W && X);
  bool C = (W && X) || (!W && !X && Y);
  return (C << 2) | (B << 1) | A;
}

int next_mod4(int val) {
  bool W = (val >> 0) & 1; bool X = (val >> 1) & 1;
  bool A = !W;
  bool B = (W && !X) || (!W && X);
  return (B << 1) | A;
}

int next_mod3(int val) {
  bool W = (val >> 0) & 1; bool X = (val >> 1) & 1;
  bool A = !W && !X;
  bool B = W && !X;
  return (B << 1) | A;
}





int prev_mod10(int val) {
  bool W = (val >> 0) & 1; bool X = (val >> 1) & 1; 
  bool Y = (val >> 2) & 1; bool Z = (val >> 3) & 1;
  bool A = !W;
  bool B = (!X && !W && ((!Z && Y) || (Z && !Y))) || (!Z && W && X);
  bool C = (!Z && Y && (X || W)) || (Z && !X && !W && !Y);
  bool D = (!X && !Y && ((Z && W) || (!Z && !W)));
  return (D << 3) | (C << 2) | (B << 1) | A;
}

int prev_mod6(int val) {
  bool W = (val >> 0) & 1; bool X = (val >> 1) & 1; bool Y = (val >> 2) & 1;
  bool A = !W;
  bool B = (Y && !X && !W) || (!Y && X && W);
  bool C = !X && ((Y && W) || (!Y && !W));
  return (C << 2) | (B << 1) | A;
}

int prev_mod4(int val) {
  bool W = (val >> 0) & 1; bool X = (val >> 1) & 1;
  bool A = !W;
  bool B = (X && W) || (!X && !W);
  return (B << 1) | A;
}

int prev_mod3(int val) {
  bool W = (val >> 0) & 1; bool X = (val >> 1) & 1;
  bool A = X && !W;
  bool B = !X && !W;
  return (B << 1) | A;
}





void setup() {
  pinMode(pinA, OUTPUT); pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT); pinMode(pinD, OUTPUT);

  pinMode(fixTailA, INPUT); pinMode(fixTailD, INPUT);

  pinMode(enableSecUnits, OUTPUT); pinMode(enableSecTens, OUTPUT);
  pinMode(enableMinUnits, OUTPUT); pinMode(enableMinTens, OUTPUT);
  pinMode(enableHrUnits, OUTPUT);  pinMode(enableHrTens, OUTPUT);

  pinMode(btnEdit,  INPUT_PULLUP); pinMode(btnNext,  INPUT_PULLUP);
  pinMode(btnInc,   INPUT_PULLUP); pinMode(btnDec,   INPUT_PULLUP);
  pinMode(btnTimer, INPUT_PULLUP);

  blankAllDisplays();
}

void loop() {
  handleButtons();
  updateClock();
  updateTimer();

  if (millis() - lastBlinkTime >= (timerAlarm ? 100 : 200)) {
    lastBlinkTime = millis();
    blinkState = !blinkState;
  }

  refreshDisplay();
}





void updateClock() {
  if (editMode && currentMode == MODE_CLOCK) {
    lastClockTick = millis(); 
    return;
  }

  if (millis() - lastClockTick >= 1000) {
    lastClockTick = millis();

    int secU = clockSec % 10; int secT = clockSec / 10;
    int minU = clockMin % 10; int minT = clockMin / 10;
    int hrU  = clockHr  % 10; int hrT  = clockHr  / 10;

    secU = next_mod10(secU); 
    if (secU == 0) {
      secT = next_mod6(secT);
      if (secT == 0) {
        minU = next_mod10(minU);
        if (minU == 0) {
          minT = next_mod6(minT);
          if (minT == 0) {
            if (hrT == 2) { 
              hrU = next_mod4(hrU);
              if (hrU == 0) hrT = next_mod3(hrT);
            } else {        
              hrU = next_mod10(hrU);
              if (hrU == 0) hrT = next_mod3(hrT);
            }
          }
        }
      }
    }

    clockSec = secT * 10 + secU;
    clockMin = minT * 10 + minU;
    clockHr  = hrT  * 10 + hrU;
  }
}





void updateTimer() {
  if (timerRunning) {
    if (millis() - lastTimerTick >= 1000) {
      lastTimerTick = millis();

      if (timerSec > 0) {
        timerSec--;
      } else {
        if (timerMin > 0) {
          timerMin--;
          timerSec = 59;
        } else if (timerHr > 0) {
          timerHr--;
          timerMin = 59;
          timerSec = 59;
        } else {
          timerRunning = false;
          timerAlarm = true;
        }
      }
    }
  }
}





void handleButtons() {
  bool currEdit  = digitalRead(btnEdit);
  bool currNext  = digitalRead(btnNext);
  bool currInc   = digitalRead(btnInc);
  bool currDec   = digitalRead(btnDec);
  bool currTimer = digitalRead(btnTimer);

  if (timerAlarm && (currEdit == LOW || currNext == LOW || currInc == LOW || currDec == LOW || currTimer == LOW)) {
    timerAlarm = false;
    delay(100);
    return;
  }

  if (lastTimerState == HIGH && currTimer == LOW) {
    delay(50);
    currentMode = (currentMode == MODE_CLOCK) ? MODE_TIMER : MODE_CLOCK;
    editMode = false;
  }

  if (lastEditState == HIGH && currEdit == LOW) {
    delay(50);
    editMode = !editMode;
    selectedDigit = 0;
    blinkState = true;

    if (!editMode && currentMode == MODE_TIMER) {
      initTimerSec = timerSec; initTimerMin = timerMin; initTimerHr  = timerHr;
    }
  }

  if (editMode) {
    if (lastNextState == HIGH && currNext == LOW) {
      delay(50);
      selectedDigit = (selectedDigit + 1) % 6;
      blinkState = true;
    }
    if (lastIncState == HIGH && currInc == LOW) {
      delay(50); adjustDigit(1);
    }
    if (lastDecState == HIGH && currDec == LOW) {
      delay(50); adjustDigit(-1);
    }
  } 
  else if (currentMode == MODE_TIMER) {
    if (lastIncState == HIGH && currInc == LOW) {
      delay(50);
      if (timerSec > 0 || timerMin > 0 || timerHr > 0) {
        timerRunning = !timerRunning;
        lastTimerTick = millis();
      }
    }
    if (lastDecState == HIGH && currDec == LOW) {
      delay(50);
      timerRunning = false;
      timerSec = initTimerSec; timerMin = initTimerMin; timerHr  = initTimerHr;
    }
  }

  lastEditState = currEdit; lastNextState = currNext;
  lastIncState  = currInc;  lastDecState  = currDec;
  lastTimerState = currTimer;
}





void adjustDigit(int dir) {
  int &s = (currentMode == MODE_CLOCK) ? clockSec : timerSec;
  int &m = (currentMode == MODE_CLOCK) ? clockMin : timerMin;
  int &h = (currentMode == MODE_CLOCK) ? clockHr  : timerHr;

  int secU = s % 10, secT = s / 10;
  int minU = m % 10, minT = m / 10;
  int hrU  = h % 10, hrT  = h / 10;

  if (dir > 0) { 
    
    switch (selectedDigit) {
      case 0: secU = next_mod10(secU); break;
      case 1: secT = next_mod6(secT); break;
      case 2: minU = next_mod10(minU); break;
      case 3: minT = next_mod6(minT); break;
      case 4: hrU = (hrT == 2) ? next_mod4(hrU) : next_mod10(hrU); break;
      case 5: 
        hrT = next_mod3(hrT);
        if (hrT == 2 && hrU > 3) hrU = 3; 
        break;
    }
  } else { 
    
    switch (selectedDigit) {
      case 0: secU = prev_mod10(secU); break;
      case 1: secT = prev_mod6(secT); break;
      case 2: minU = prev_mod10(minU); break;
      case 3: minT = prev_mod6(minT); break;
      case 4: hrU = (hrT == 2) ? prev_mod4(hrU) : prev_mod10(hrU); break;
      case 5: 
        hrT = prev_mod3(hrT);
        if (hrT == 2 && hrU > 3) hrU = 3; 
        break;
    }
  }

  s = secT * 10 + secU;
  m = minT * 10 + minU;
  h = hrT  * 10 + hrU;
}





void refreshDisplay() {
  if (timerAlarm && !blinkState) {
    blankAllDisplays();
    return;
  }

  int activeSec = (currentMode == MODE_CLOCK) ? clockSec : timerSec;
  int activeMin = (currentMode == MODE_CLOCK) ? clockMin : timerMin;
  int activeHr  = (currentMode == MODE_CLOCK) ? clockHr  : timerHr;

  int secUnits = activeSec % 10; int secTens  = activeSec / 10;
  int minUnits = activeMin % 10; int minTens  = activeMin / 10;
  int hrUnits  = activeHr  % 10; int hrTens   = activeHr  / 10;

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
  digitalWrite(enableSecUnits, LOW); digitalWrite(enableSecTens, LOW);
  digitalWrite(enableMinUnits, LOW); digitalWrite(enableMinTens, LOW);
  digitalWrite(enableHrUnits, LOW);  digitalWrite(enableHrTens, LOW);

  pinMode(fixTailA, INPUT);
  pinMode(fixTailD, INPUT);
}