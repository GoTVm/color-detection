#include <MD_TCS230.h>
#include <FreqCount.h>
#include <LiquidCrystal.h>

#define  S2_OUT  12
#define  S3_OUT  13
#define  OE_OUT   8
#define cal_button 2
#define det_button 3
#define LCD_VO 11

int Contrast = 75;
bool calibrated = false;

MD_TCS230  CS(S2_OUT, S3_OUT, OE_OUT);
LiquidCrystal lcd(1, 4, 6, 7, 9, 10); //states the pins RS, E, D4, D5, D6, D7 are connected to


void setup() {
  pinMode(cal_button, INPUT_PULLUP);
  pinMode(det_button, INPUT_PULLUP);
  analogWrite(LCD_VO, 75);

  delay(30);

  CS.begin();
  lcd.begin(16, 2);

  delay(50);
  lcd.clear();
  delay(10);
  lcd.setCursor(0, 0);
  lcd.print("G: Calibration");
  delay(10);
  lcd.setCursor(0, 1);
  lcd.print("B: Detect colors");
}

void calibrateSensor() {
  if (calibrated) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calibration");
    lcd.setCursor(0, 1);
    lcd.print("already done");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("G: Calibration");
    lcd.setCursor(0, 1);
    lcd.print("B: Detect colors");
  }
  else {
    sensorData sd;
    delay(300);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Black");
    while (digitalRead(cal_button) == HIGH);

    if (digitalRead(cal_button) == LOW) {
      CS.read();
      while (CS.available() == 0);
      CS.getRaw(&sd);
      CS.setDarkCal(&sd);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("White");
    while (digitalRead(cal_button) == HIGH);

    if (digitalRead(cal_button) == LOW) {
      CS.read();
      while (CS.available() == 0);
      CS.getRaw(&sd);
      CS.setWhiteCal(&sd);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calibration");
    lcd.setCursor(0, 1);
    lcd.print("done");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("G: Calibration");
    lcd.setCursor(0, 1);
    lcd.print("B: Detect colors");

    calibrated = true;
  }
}

void detectColor() {
  bool detected;
  if (calibrated) {
    colorData rgb;
    detected = true;
    lcd.clear();
    CS.read();
    while (CS.available() == 0);
    CS.getRGB(&rgb);
    lcd.setCursor(0, 0);
    lcd.print("R: " + String(rgb.value[TCS230_RGB_R]));
    lcd.setCursor(7, 0);
    lcd.print("G: " + String(rgb.value[TCS230_RGB_G]));
    lcd.setCursor(0, 1);
    lcd.print("B: " + String(rgb.value[TCS230_RGB_B]));
    unsigned long currentMillis = millis();

    while (millis() - currentMillis < 10000);

    if (millis() - currentMillis >= 5000 && detected) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("G: Calibration");
      lcd.setCursor(0, 1);
      lcd.print("B: Detect colors");
      detected = false;
    }
  }
  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Pending");
    lcd.setCursor(0, 1);
    lcd.print("calibration");
  }
}

void loop() {
  if (digitalRead(cal_button) == LOW) {
    calibrateSensor();
  }

  else if (digitalRead(det_button) == LOW) {
    detectColor();
  }
}
