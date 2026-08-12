#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ==================== OLED CONFIGURATION ====================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// ==================== PIN CONFIGURATION ====================
const int TRIG_PIN = 8;
const int ECHO_PIN = 9;
const int LED_PIN = 11;
const int BUZZER_PIN = 7;

// ==================== VARIABLES ====================
unsigned long echoTime;
int distanceCm;

// ==================== SETUP ====================
void setup() {

  // Configure pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Start Serial Monitor
  Serial.begin(9600);

  // Start I2C communication
  Wire.begin();
  Wire.setClock(100000);
  Wire.setWireTimeout(3000, true);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {

    Serial.println(
      "SSD1306 allocation failed! "
      "Check wiring or OLED address."
    );

    // Blink LED continuously if OLED initialization fails
    while (1) {

      digitalWrite(LED_PIN, HIGH);
      delay(200);

      digitalWrite(LED_PIN, LOW);
      delay(200);
    }
  }

  // Welcome message
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(15, 20);

  display.print("WELCOME");
  display.display();

  delay(1000);
}

// ==================== MAIN LOOP ====================
void loop() {

  // =========================================================
  // 1. ULTRASONIC SENSOR
  // =========================================================

  // Send ultrasonic trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  // Measure echo pulse duration
  // Timeout = 30 ms
  echoTime = pulseIn(ECHO_PIN, HIGH, 30000);

  // Check whether an echo was received
  if (echoTime == 0) {

    // No echo received
    distanceCm = 999;

  } else {

    // Calculate distance in centimeters
    distanceCm = echoTime * 0.0343 / 2;
  }

  // =========================================================
  // 2. SERIAL MONITOR
  // =========================================================

  Serial.print("Echo Time = ");
  Serial.print(echoTime);

  Serial.print(" us    Distance = ");
  Serial.print(distanceCm);

  Serial.println(" cm");

  // =========================================================
  // 3. BUZZER WARNING
  // =========================================================

  if (distanceCm <= 30) {

    // Object is very close
    digitalWrite(BUZZER_PIN, HIGH);
    delay(150);

    digitalWrite(BUZZER_PIN, LOW);
    delay(100);

  } else {

    digitalWrite(BUZZER_PIN, LOW);
  }

  // =========================================================
  // 4. LED ALERT
  // =========================================================

  if (distanceCm > 30 && distanceCm <= 60) {

    // Object is within alert range
    digitalWrite(LED_PIN, HIGH);

  } else {

    digitalWrite(LED_PIN, LOW);
  }

  // =========================================================
  // 5. OLED DISPLAY
  // =========================================================

  display.clearDisplay();

  // Display title
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  display.print("DISTANCE:");

  // Display distance
  display.setTextSize(3);
  display.setCursor(0, 20);

  if (distanceCm == 999) {

    // No valid distance
    display.print("---");

  } else {

    display.print(distanceCm);
    display.print("cm");
  }

  // =========================================================
  // 6. WARNING / ALERT MESSAGE
  // =========================================================

  if (distanceCm <= 30) {

    // Very close object
    display.setTextSize(1);
    display.setCursor(14, 50);

    display.print("[WARN]");

  } else if (distanceCm > 30 && distanceCm <= 60) {

    // Moderate distance
    display.setTextSize(1);
    display.setCursor(0, 50);

    display.print("ALERT");
  }

  // Send complete frame to OLED
  display.display();

  // Small delay before next measurement
  delay(50);
}

