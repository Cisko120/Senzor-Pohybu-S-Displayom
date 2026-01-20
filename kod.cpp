#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// PIR sensor pin
#define PIR_PIN 2

// detekovany pohyb
bool motionDetected = false;
// Timer for filtering false detections
unsigned long motionStartTime = 0;
const unsigned long motionThreshold = 1000; // 1000ms (1 second) threshold to confirm motion

void setup() {
	// Init Serial Monitor
	Serial.begin(115200);

	// definicia displeja
	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
		Serial.println(F("SSD1306 allocation failed"));
		for (;;);
	}

	// Set PIR pin as input
	pinMode(PIR_PIN, INPUT);
	
	// displej poèká 5 sekund a spustí sa
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	
	display.setCursor(0, 0);
	display.println("Pripravujem sa...");
	display.display();
	Serial.println("Pripravujem sa... Cakam 5 sekund.");
	delay(5000); // poèkaj 5 sekund a zaèni sníma
	
	// displej vypíše èakaciu správu
	display.clearDisplay();
	display.setCursor(0, 0);
	display.println("Cakam na pohyb...");
	display.display();
	
	Serial.println("System spusteny. Cakam na detekciu pohybu.");
}

void loop() {
	// zaznamenavanie pohybu
	int motion = digitalRead(PIR_PIN);
	
	// 1-0
	Serial.print("PIR hodnota: ");
	Serial.println(motion); // 1 = HIGH (motion), 0 = LOW (no motion)
	
	if (motion == HIGH) {
		if (motionStartTime == 0) {
			// spusti keï detekuje pohyb (1)
			motionStartTime = millis();
			Serial.println("Zaciatok casovania detekcie...");
		} else if (millis() - motionStartTime > motionThreshold && !motionDetected) {
			// Správa, keï detekuje pohyb
			motionDetected = true;
			Serial.println("Pohyb detekovany!");
			
			// správa ak detekuje pohyb
			display.clearDisplay();
			display.setCursor(0, 0);
			display.println("Detekovany pohyb");
			
			display.display();
			
			// èakacia doba medzi intervalmi
			delay(2000);
		}
	} else {
		// resetovanie èasovaèa
		motionStartTime = 0;
		if (motionDetected) {
			// koniec detekcie senzora
			motionDetected = false;
			Serial.println("Pohyb skoncil.");
			
			// Správa po obnovení
			display.clearDisplay();
			display.setCursor(0, 0);
			display.println("Cakam na pohyb...");
			display.display();
		}
	}
	
	// Pauza medzi opakovaniami
	delay(100);
}
