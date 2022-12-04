#include <LiquidCrystal.h>

#define DECR_BTN 9
#define INCR_BTN 6
#define MODE_BTN 7
#define SAVE_BTN 8
#define MOTOR 10

enum Function {EDIT_HR, EDIT_MIN};

struct Time {
  int hr = 12;
  int min = 0;
};

struct FeedEvent {
  Time time;
  int duration;
};

LiquidCrystal lcd(11, 12, 2, 3, 4, 5);

Function function = EDIT_HR;

FeedEvent feedEvents[2];

bool timeSaved, durationSaved;

/*************************
 *    REMOVE THIS AREA   *
 ************************/
int get_hr() {
  return 12;
}

int get_min() {
  return 0;
}
/*************************
 *    REMOVE THIS AREA   *
 ************************/

int get_hr(Time time) {
  char timeStr[16];

  unsigned long elapsedMillis = millis(), period = 200;
  int hr = time.hr, min = time.min;

  bool isVisible;
  bool incrPressed = false, decrPressed = false, modePressed = false, savePressed = false;
  bool incrPressedPrev = false, decrPressedPrev = false, modePressedPrev = false;

  lcd.setCursor(0, 1);
  sprintf(timeStr, "%02d:%02d", hr, min);
  lcd.print(timeStr);

  while (function == EDIT_HR) {
    while (millis() < elapsedMillis + period) {
      incrPressed = digitalRead(INCR_BTN);
      decrPressed = digitalRead(DECR_BTN);
      modePressed = digitalRead(MODE_BTN);
      savePressed = digitalRead(SAVE_BTN);

      if (incrPressed && incrPressed != incrPressedPrev && hr < 24) {
        hr++;
      }

      if (decrPressed && decrPressed != decrPressedPrev && hr > 0) {
        hr--;  
      }

      if (modePressed && modePressed != modePressedPrev) {
        function = EDIT_MIN;
        while(digitalRead(MODE_BTN));
        break;
      }

      if (savePressed) {
        timeSaved = true;
        while (digitalRead(SAVE_BTN));
        return hr;
      }

      decrPressedPrev = decrPressed;
      incrPressedPrev = incrPressed;
      modePressedPrev = modePressed;
    }

    if (!isVisible) {
      lcd.setCursor(0, 1);
      lcd.print("  ");

      isVisible = true; 
    } else {
      sprintf(timeStr, "%02d", hr);
      lcd.setCursor(0, 1);
      lcd.print(timeStr);

      isVisible = false;
    }
    elapsedMillis = millis();
  }
  return hr;
};

int get_min(Time time) {
  char timeStr[16];

  unsigned long elapsedMillis = millis(), period = 200;
  int hr = time.hr, min = time.min;

  bool isVisible;
  bool incrPressed = false, decrPressed = false, modePressed = false, savePressed = false;
  bool incrPressedPrev = false, decrPressedPrev = false, modePressedPrev = false;

  lcd.setCursor(0, 1);
  sprintf(timeStr, "%02d:%02d", hr, min);
  lcd.print(timeStr);

  while ((function == EDIT_MIN)) {
    while (millis() < elapsedMillis + period) {
      incrPressed = digitalRead(INCR_BTN);
      decrPressed = digitalRead(DECR_BTN);
      modePressed = digitalRead(MODE_BTN);
      savePressed = digitalRead(SAVE_BTN);

      if (incrPressed && incrPressed != incrPressedPrev && min < 55) {
        min += 5;
      }

      if (decrPressed && decrPressed != decrPressedPrev && min > 0) {
        min -= 5;  
      }

      if (modePressed && modePressed != modePressedPrev) {
        function = EDIT_HR;
        while(digitalRead(MODE_BTN));
        break;
      }

      if (savePressed) {
        timeSaved = true;
        while (digitalRead(SAVE_BTN));
        return min;
      }

      decrPressedPrev = decrPressed;
      incrPressedPrev = incrPressed;
      modePressedPrev = modePressed;
    }

    if (!isVisible) {
      lcd.setCursor(3, 1);
      lcd.print("  ");

      isVisible = true; 
    } else {
      sprintf(timeStr, "%02d", min);
      lcd.setCursor(3, 1);
      lcd.print(timeStr);

      isVisible = false;
    }
    elapsedMillis = millis();
  }
  return min;
};

int get_duration(void) {
  char durationStr[16];

  unsigned long elapsedMillis = millis(), period = 200;
  int duration = 0;

  bool isVisible;
  bool savePressed = false, incrPressed, decrPressed;
  bool incrPressedPrev, decrPressedPrev;

  lcd.setCursor(0, 1);
  lcd.print("     ");
  lcd.setCursor(0, 1);
  lcd.print(duration);
  lcd.setCursor(0, 1);

  while (!savePressed) {
    while (millis() < elapsedMillis + period) {
      savePressed = digitalRead(SAVE_BTN);
      incrPressed = digitalRead(INCR_BTN);
      decrPressed = digitalRead(DECR_BTN);

      if (incrPressed && incrPressed != incrPressedPrev) {
        duration++;
      }

      if (decrPressed && decrPressed != decrPressedPrev && duration > 0)
        duration--;

      if (savePressed) {
        while (digitalRead(SAVE_BTN));

        return duration;
      }

      incrPressedPrev = incrPressed;
      decrPressedPrev = decrPressed;
    }

    if (!isVisible) {
      lcd.setCursor(0, 1);
      lcd.print("   ");

      isVisible = true; 
    } else {
      sprintf(durationStr, "%02d", duration);
      lcd.setCursor(0, 1);
      lcd.print(durationStr);

      isVisible = false;
    }
    elapsedMillis = millis();
  }
};

void setup() {
  // Initialize buttons
  pinMode(DECR_BTN, INPUT);  
  pinMode(INCR_BTN, INPUT);
  pinMode(MODE_BTN, INPUT);
  pinMode(SAVE_BTN, INPUT);

  pinMode(MOTOR, OUTPUT);

  digitalWrite(MOTOR, HIGH);


  // Begin serial for debug
  Serial.begin(9600);
  
  // Initialize lcd
  lcd.begin(16, 2);

  // Loop through times and get input
  char prompt[20];
  int arrSize = sizeof feedEvents / sizeof feedEvents[0];

  for (int i = 0; i < arrSize; i++) {
    FeedEvent* feedEvent = &feedEvents[i];

    sprintf(prompt, "Enter time %d", (i + 1));
    Serial.println(prompt);

    lcd.setCursor(0, 0);
    lcd.print(prompt);

    while (!timeSaved) {
      if (function == EDIT_HR)
        feedEvent->time.hr = get_hr(feedEvent->time);
      else
        feedEvent->time.min = get_min(feedEvent->time);
    }

    lcd.setCursor(0, 0);
    sprintf(prompt, "Enter duration %d", (i + 1));
    lcd.print(prompt);

    feedEvent->duration = get_duration();


    function = EDIT_HR;
    timeSaved = false;

    lcd.clear();
  }

  char timeStr[16];

  lcd.setCursor(0, 0);
  sprintf(timeStr, "%02d:%02d %02d", feedEvents[0].time.hr, feedEvents[0].time.min, feedEvents[0].duration);
  lcd.print(timeStr);

  lcd.setCursor(0, 1);
  sprintf(timeStr, "%02d:%02d %02d", feedEvents[1].time.hr, feedEvents[1].time.min, feedEvents[1].duration);
  lcd.print(timeStr);

}

void loop() {

}
