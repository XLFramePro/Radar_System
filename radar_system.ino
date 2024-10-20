#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define TRIG_PIN 2
#define ECHO_PIN 3
#define SERVO_PIN 6
#define DISTANCE_THRESHOLD 60

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Servo myServo;

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  myServo.attach(SERVO_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Error OLED Screen"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Radar System");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  for (int angle = 0; angle <= 180; angle++) {
    updateDisplay(angle);
  }
  
  for (int angle = 180; angle >= 0; angle--) {
    updateDisplay(angle);
  }
}

void updateDisplay(int angle) {
  myServo.write(angle);
  int distance = readUltrasonicDistance();
  display.clearDisplay();
  
  drawRadarFrame();
  drawRadarNeedle(angle);
  if (distance < DISTANCE_THRESHOLD) {
    drawDetectedObject(angle, distance);
  }

  display.display();
  delay(5);
}

int readUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

void drawRadarFrame() {
  for (int radius = 15; radius <= 60; radius += 15) {
    for (int i = 0; i <= 180; i++) {
      int x = 64 + radius * cos(radians(i));
      int y = 64 - radius * sin(radians(i));
      display.drawPixel(x, y, WHITE);
    }
  }
}

void drawRadarNeedle(int angle) {
  int x = 64 + (SCREEN_HEIGHT - 1) * cos(radians(angle));
  int y = 64 - (SCREEN_HEIGHT - 1) * sin(radians(angle));
  display.drawLine(64, 64, x, y, WHITE);
}

void drawDetectedObject(int angle, int distance) {
  int xTarget = 64 + distance * cos(radians(angle));
  int yTarget = 64 - distance * sin(radians(angle));
  display.fillCircle(xTarget, yTarget, 2, WHITE);
}
