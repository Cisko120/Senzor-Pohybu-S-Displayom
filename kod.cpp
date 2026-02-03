#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define PIR_PIN 27

bool motionDetected = false;
unsigned long lastMotionTime = 0;
const unsigned long holdTime = 2500; // 5s

int oldMotion = -1;

void showStatus(int motion, bool confirmed) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  if (confirmed) display.println("POHYB DETEKOVANY!");
  else display.println("Cakam na pohyb...");

  display.display();
}

void setup() {
  delay(500);
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true) delay(10);
  }

  pinMode(PIR_PIN, INPUT);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Pripravujem sa...");
  display.display();

  Serial.println("Pripravujem sa... Cakam 5 sekund.");
  delay(5000);

  showStatus(digitalRead(PIR_PIN), false);
  Serial.println("System spusteny. Cakam na detekciu pohybu.");
}

void loop() {
  int motion = digitalRead(PIR_PIN);

  // event: HIGH = pohyb detekovany (nedavno)
  if (motion == HIGH) {
    lastMotionTime = millis();
    if (!motionDetected) {
      motionDetected = true;
      Serial.println("Pohyb detekovany!");
    }
  }

  // zisti ci presli 2-3 sekundy
  if (motionDetected && (millis() - lastMotionTime > holdTime)) {
    motionDetected = false;
    Serial.println("Pohyb skoncil.");
  }

  // uprav obrazovku ak sa zmenil status
  static bool oldDetected = false;
  if (motion != oldMotion || motionDetected != oldDetected) {
    oldMotion = motion;
    oldDetected = motionDetected;
    showStatus(motion, motionDetected);
  }

  delay(50); // debounce
}
