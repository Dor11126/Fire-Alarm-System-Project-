#include <Adafruit_LiquidCrystal.h>

const int tempPin    = A0;  // Temperature sensor connected to A0
const int gasPin     = A1;  // Gas sensor connected to A1
const int ledPin     = 13;  // LED connected to digital pin 13
const int buzzerPin  = 7;   // Buzzer connected to digital pin 7

Adafruit_LiquidCrystal lcd(0);

// Threshold values
const int TEMP_THRESHOLD = 200;  // ADC threshold for high temperature
const int GAS_THRESHOLD  = 300;  // ADC threshold for gas detection

unsigned long previousMillis = 0;
const long interval = 50; // Blink speed (in milliseconds)

bool ledState = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Status: NORMAL");
  lcd.setCursor(0, 1);
  lcd.print("T:----C G:----");
}

void loop() {
  int rawTemp = analogRead(tempPin);
  int rawGas  = analogRead(gasPin);

  // Convert raw temperature to Celsius ( LM35 sensor)
  float voltage   = rawTemp * 5.0 / 1023.0;
  float tempC     = voltage * 100.0;

  Serial.print("Temperature: ");
  Serial.print(tempC, 1);
  Serial.print(" C | Gas ADC: ");
  Serial.println(rawGas);

  bool fireDetected = (rawTemp > TEMP_THRESHOLD) || (rawGas > GAS_THRESHOLD);

  // Update LCD display
  if (!fireDetected) {
    lcd.setCursor(0, 0);
    lcd.print("Status: NORMAL "); // 16 characters wide
  } else {
    lcd.setCursor(0, 0);
    lcd.print("!!! FIRE ALERT !!!");
  }

  // Second row: T:xx.xC G:yyy
  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(tempC, 1);
  lcd.print("C ");
  lcd.print("G:");
  lcd.print(rawGas);

  // Pad with spaces if needed to clear leftover characters
  int fill = 16 - (4 + String(tempC,1).length() + 2 + 1 + String(rawGas).length());
  while (fill-- > 0) lcd.print(' ');

  // LED and buzzer control
  if (fireDetected) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(ledPin, ledState ? HIGH : LOW);
      if (ledState) tone(buzzerPin, 1000);
      else          noTone(buzzerPin);
    }
  } else {
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
  }

  delay(100);
}
