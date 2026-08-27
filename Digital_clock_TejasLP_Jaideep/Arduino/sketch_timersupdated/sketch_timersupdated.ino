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

  
  pinMode(btnEdit,  INPUT_PULLUP);
  pinMode(btnNext,  INPUT_PULLUP);
  pinMode(btnInc,   INPUT_PULLUP);
  pinMode(btnDec,   INPUT_PULLUP);
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
    clockSec++;
    if (clockSec >= 60) {
      clockSec = 0;
      clockMin++;
      if (clockMin >= 60) {
        clockMin = 0;
        clockHr++;
        if (clockHr >= 24) clockHr = 0;
      }
    }
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
      initTimerSec = timerSec;
      initTimerMin = timerMin;
      initTimerHr  = timerHr;
    }
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
      timerSec = initTimerSec;
      timerMin = initTimerMin;
      timerHr  = initTimerHr;
    }
  }

  lastEditState  = currEdit;
  lastNextState  = currNext;
  lastIncState   = currInc;
  lastDecState   = currDec;
  lastTimerState = currTimer;
}

void adjustDigit(int dir) {
  int &s = (currentMode == MODE_CLOCK) ? clockSec : timerSec;
  int &m = (currentMode == MODE_CLOCK) ? clockMin : timerMin;
  int &h = (currentMode == MODE_CLOCK) ? clockHr  : timerHr;

  int secU = s % 10, secT = s / 10;
  int minU = m % 10, minT = m / 10;
  int hrU  = h % 10, hrT  = h / 10;

  switch (selectedDigit) {
    case 0: secU = (secU + dir + 10) % 10; break;
    case 1: secT = (secT + dir + 6) % 6; break;
    case 2: minU = (minU + dir + 10) % 10; break;
    case 3: minT = (minT + dir + 6) % 6; break;
    case 4: 
      hrU += dir;
      if (hrU > 9) hrU = 0; if (hrU < 0) hrU = 9;
      if (hrT * 10 + hrU > 23) hrU = (dir > 0) ? 0 : 3;
      break;
    case 5: 
      hrT += dir;
      if (hrT > 2) hrT = 0; if (hrT < 0) hrT = 2;
      if (hrT * 10 + hrU > 23) hrT = (dir > 0) ? 0 : 2;
      break;
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

  int secUnits = activeSec % 10;
  int secTens  = activeSec / 10;
  int minUnits = activeMin % 10;
  int minTens  = activeMin / 10;
  int hrUnits  = activeHr  % 10;
  int hrTens   = activeHr  / 10;

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