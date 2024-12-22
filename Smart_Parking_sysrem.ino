#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Initialize I2C display
LiquidCrystal_I2C lcd(0x27, 16, 2); // Address 0x27, 16 columns, 2 rows
SoftwareSerial bluetooth(2, 3);    // RX, TX for Bluetooth   

// Pins for ultrasonic sensors
const int trigPins[] = {9, 7}; // Trig pins for sensors
const int echoPins[] = {8, 6}; // Echo pins for sensors

// Other components
const int buzzerPin = 10;
const int ledPins[] = {11, 12}; // LEDs for parking spaces

// Variables
long durations[2];
int distances[2];
const int parkingThreshold = 10; // Distance in cm to detect vehicle presence
bool isOccupied[] = {false, false}; // Occupancy status for both parking spaces

void setup() {
  // Initialize components
  lcd.begin(16, 2);       // Initialize the LCD with 16 columns and 2 rows
  lcd.backlight();        // Turn on the LCD backlight
  bluetooth.begin(9600);  // Initialize Bluetooth communication

  // Set up pins
  for (int i = 0; i < 2; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(buzzerPin, OUTPUT);

  // Display initialization message
  lcd.setCursor(0, 0);
  lcd.print("Smart Parking");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
}

void loop() {
  for (int i = 0; i < 2; i++) {
    // Measure distance using the ultrasonic sensor
    digitalWrite(trigPins[i], LOW);
    delayMicroseconds(2);
    digitalWrite(trigPins[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPins[i], LOW);

    durations[i] = pulseIn(echoPins[i], HIGH);
    distances[i] = durations[i] * 0.034 / 2; // Convert to cm

    // Display distance on LCD
    lcd.setCursor(0, i); // Update the corresponding line for each parking spot
    lcd.print("P");
    lcd.print(i + 1);
    lcd.print(":");
    lcd.print(distances[i]);
    lcd.print("cm ");

    // Check if parking space is occupied
    if (distances[i] < parkingThreshold) {
      if (!isOccupied[i]) {
        isOccupied[i] = true;

        // Update LCD and send alert via Bluetooth
        lcd.setCursor(0, i);
        lcd.print("P");
        lcd.print(i + 1);
        lcd.print(": Occupied");
        bluetooth.print("Parking ");
        bluetooth.print(i + 1);
        bluetooth.println(": Occupied");

        // Turn on LED and activate buzzer
        digitalWrite(ledPins[i], HIGH);
        for (int j = 0; j < 3; j++) {
          digitalWrite(buzzerPin, HIGH);
          delay(200);
          digitalWrite(buzzerPin, LOW);
          delay(200);
        }
      }
    } else {
      if (isOccupied[i]) {
        isOccupied[i] = false;

        // Update LCD and send alert via Bluetooth
        lcd.setCursor(0, i);
        lcd.print("P");
        lcd.print(i + 1);
        lcd.print(": Free     ");
        bluetooth.print("Parking ");
        bluetooth.print(i + 1);
        bluetooth.println(": Free");

        // Turn off LED
        digitalWrite(ledPins[i], LOW);
      }
    }
  }

  delay(500); // Update every 0.5 seconds
}
