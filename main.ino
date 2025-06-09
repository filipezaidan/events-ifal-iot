
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

// Display imports
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// Bitmaps de ícones do sistema
static const unsigned char PROGMEM wifi_icon[] = {
  0x00, 0x00, 0x0f, 0xf0, 0x30, 0x0c, 0x40, 0x02, 0x8f, 0xf1, 0x90, 0x09,
  0xa0, 0x05, 0xa3, 0xc5, 0xa4, 0x25, 0xa4, 0x25, 0xa0, 0x05, 0x90, 0x09,
  0x8f, 0xf1, 0x40, 0x02, 0x30, 0x0c, 0x0f, 0xf0
};

static const unsigned char PROGMEM rfid_icon[] = {
  0x00, 0x00, 0x07, 0xe0, 0x18, 0x18, 0x20, 0x04, 0x47, 0xe2, 0x88, 0x11,
  0x90, 0x09, 0x90, 0x09, 0x90, 0x09, 0x90, 0x09, 0x88, 0x11, 0x47, 0xe2,
  0x20, 0x04, 0x18, 0x18, 0x07, 0xe0, 0x00, 0x00
};

static const unsigned char PROGMEM check_icon[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x18,
  0x00, 0x30, 0x00, 0x60, 0x80, 0xc0, 0xc1, 0x80, 0x63, 0x00, 0x36, 0x00,
  0x1c, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM error_icon[] = {
  0x00, 0x00, 0x81, 0x81, 0xc3, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x18,
  0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0x81, 0x81, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


MFRC522DriverPinSimple ss_pin(15);
MFRC522DriverSPI driver{ss_pin}; // Create SPI driver
MFRC522 mfrc522{driver};         // Create MFRC522 instance

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();

  mfrc522.PCD_Init();    // Init MFRC522 board.
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);	// Show details of PCD - MFRC522 Card Reader details.
	Serial.println(F("Scan PICC to see UID"));
}

void loop() {
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if (!mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards.
	if (!mfrc522.PICC_ReadCardSerial()) {
		return;
	}
  display.clearDisplay();
  display.display();
  

  Serial.print("Card UID: ");
  MFRC522Debug::PrintUID(Serial, (mfrc522.uid));
  Serial.println();


  // Save the UID on a String variable
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uidString += "0"; 
    }
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
  //Serial.println(PROGMEM wifi_icon);

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println(uidString)
  display.display();
  delay(1000);
}
