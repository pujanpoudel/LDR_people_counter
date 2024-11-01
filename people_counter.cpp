#include <Wire.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int sensorPin = A0;
const int resetPin = 7;
const int EEPROM_ADDR = 0;
int sensorValue = 0;
int personCount = 0;
bool personDetected = false;
bool lastResetState = HIGH;

void setup() {
  Serial.begin(9600);
  
  // Initialize reset pin as input with internal pull-up
  pinMode(resetPin, INPUT_PULLUP);
  
  // Read the stored count from EEPROM
  EEPROM.get(EEPROM_ADDR, personCount);
  
  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  
  // Initial LCD display
  updateDisplay();
}

void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Person Entered");
  lcd.setCursor(0, 1);
  lcd.print("Count: ");
  lcd.print(personCount);
}

void checkResetButton() {
  // Read the reset button state
  bool currentResetState = digitalRead(resetPin);
  
  // Check for button press (LOW due to pull-up)
  if (currentResetState == LOW && lastResetState == HIGH) {
    delay(50);  // Simple debounce
    if (digitalRead(resetPin) == LOW) {
      // Reset counter
      personCount = 0;
      // Store new value in EEPROM
      EEPROM.put(EEPROM_ADDR, personCount);
      // Update display
      updateDisplay();
      Serial.println("Counter Reset to 0");
    }
  }
  lastResetState = currentResetState;
}

void loop() {
  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);
  
  // Check for person detection
  if (sensorValue < 50 && !personDetected) {
    personCount++;
    personDetected = true;
    
    // Store new count in EEPROM
    EEPROM.put(EEPROM_ADDR, personCount);
    
    // Update display
    updateDisplay();
    
    Serial.print("Person Count: ");
    Serial.println(personCount);
  }
  
  // Reset detection flag when no person is detected
  if (sensorValue >= 50) {
    personDetected = false;
  }
  
  // Check reset button
  checkResetButton();
  
  delay(100);
}
