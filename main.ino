#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// RFID Setup
MFRC522DriverPinSimple ss_pin(15);
MFRC522DriverSPI driver{ss_pin};
MFRC522 mfrc522{driver};

// WiFi credentials
const char *ssid = "Zaidan";
const char *password = "99389324";

// Google Apps Script URL (without RFID parameter)
const char *scriptBaseURL = "https://script.google.com/macros/s/AKfycbw58OY-XikOdz5wCASy4eUlt4DgW9lHKP55Ay5lqaao2Ex7NOmOlWOQCF-zeuP6vjGcvQ/exec?rfid=";

// System icons
static const unsigned char PROGMEM wifi_icon[] = {
    0x00, 0x00, 0x0f, 0xf0, 0x30, 0x0c, 0x40, 0x02, 0x8f, 0xf1, 0x90, 0x09,
    0xa0, 0x05, 0xa3, 0xc5, 0xa4, 0x25, 0xa4, 0x25, 0xa0, 0x05, 0x90, 0x09,
    0x8f, 0xf1, 0x40, 0x02, 0x30, 0x0c, 0x0f, 0xf0};

static const unsigned char PROGMEM rfid_icon[] = {
    0x00, 0x00, 0x07, 0xe0, 0x18, 0x18, 0x20, 0x04, 0x47, 0xe2, 0x88, 0x11,
    0x90, 0x09, 0x90, 0x09, 0x90, 0x09, 0x90, 0x09, 0x88, 0x11, 0x47, 0xe2,
    0x20, 0x04, 0x18, 0x18, 0x07, 0xe0, 0x00, 0x00};

static const unsigned char PROGMEM check_icon[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x18,
    0x00, 0x30, 0x00, 0x60, 0x80, 0xc0, 0xc1, 0x80, 0x63, 0x00, 0x36, 0x00,
    0x1c, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00};

static const unsigned char PROGMEM error_icon[] = {
    0x00, 0x00, 0x81, 0x81, 0xc3, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x18,
    0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0x81, 0x81, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void setup()
{
  Serial.begin(115200);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println("OLED failed!");
    while (1)
      ;
  }

  // Initialize RFID
  mfrc522.PCD_Init();
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);

  showMessage("Starting...", "Connecting WiFi");

  // Connect WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  showMessageWithIcon("WiFi Connected!", WiFi.localIP().toString(), wifi_icon);
  delay(2000);

  Serial.println("System ready - Scan RFID card");
}

void loop()
{
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED)
  {
    showMessage("WiFi Error", "Reconnecting...");
    WiFi.reconnect();
    delay(5000);
    return;
  }

  // Show waiting message
  showWaitingForCard();

  // Check for RFID card
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    delay(100);
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial())
  {
    delay(100);
    return;
  }

  // Read card UID
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    if (mfrc522.uid.uidByte[i] < 0x10)
    {
      uidString += "0";
    }
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }

  Serial.println("Card UID: " + uidString);

  // Show card detected
  showMessage("Cartao Lido!", uidString);
  delay(1000);

  // Make API request
  fetchAndDisplayData(uidString);

  // Wait before next scan
  delay(3000);
}

void showWaitingForCard()
{
  display.clearDisplay();

  // Draw RFID icon
  display.drawBitmap(48, 5, rfid_icon, 16, 16, SSD1306_WHITE);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Center the text
  display.setCursor(8, 30);
  display.println("Aproxime o cartao");
  display.setCursor(25, 45);
  display.println("para ser lido");

  display.display();
}

void fetchAndDisplayData(String rfidCode)
{
  showMessage("Verificando...", "Aguarde");

  String fullURL = String(scriptBaseURL) + rfidCode;
  String response = makeRequestWithRedirects(fullURL, 5);

  if (response.length() > 0)
  {
    processResponse(response);
  }
  else
  {
    showAccessDenied();
  }
}

String makeRequestWithRedirects(String url, int maxRedirects)
{
  WiFiClientSecure client;
  client.setInsecure();

  for (int i = 0; i < maxRedirects; i++)
  {
    HTTPClient http;
    http.begin(client, url);
    http.setTimeout(15000);
    http.addHeader("User-Agent", "ESP8266");

    Serial.println("Request " + String(i + 1) + ": " + url);

    int httpCode = http.GET();
    Serial.println("HTTP Code: " + String(httpCode));

    if (httpCode == 200)
    {
      String response = http.getString();
      http.end();
      Serial.println("Success! Response length: " + String(response.length()));
      return response;
    }
    else if (httpCode == 301 || httpCode == 302 || httpCode == 307 || httpCode == 308)
    {
      String newUrl = http.getLocation();
      http.end();

      if (newUrl.length() > 0)
      {
        Serial.println("Redirecting to: " + newUrl);
        url = newUrl;
        delay(500);
        continue;
      }
      else
      {
        Serial.println("Redirect without location header");
        return "";
      }
    }
    else
    {
      Serial.println("HTTP Error: " + String(httpCode));
      http.end();
      return "";
    }
  }

  Serial.println("Too many redirects");
  return "";
}

void processResponse(String jsonData)
{
  DynamicJsonDocument doc(8192);

  if (deserializeJson(doc, jsonData))
  {
    showAccessDenied();
    Serial.println("JSON Parse Error");
    return;
  }

  // Check if guest was found
  String name = doc["Nome do convidado"] | "";
  if (name == "" || name == "Convidado não encontrado")
  {
    showAccessDenied();
    return;
  }

  // Extract data
  String timestamp = doc["Timestamp"] | "";
  String entrada = doc["Entrada"] | "";
  String saida = doc["Saida"] | "";
  String bitmapData = doc["ImagemBitmap"] | "";

  Serial.println("Guest: " + name);
  Serial.println("Bitmap length: " + String(bitmapData.length()));

  // Show access granted
  showAccessGranted();
  delay(2000);

  // Display guest info
  displayGuestInfo(name, timestamp, entrada, saida);
  delay(4000);

  // Display bitmap if available
  if (bitmapData.length() > 10)
  {
    Serial.println("Displaying bitmap...");
    displayBitmapImage(bitmapData);
    delay(6000);
  }
  else
  {
    showMessage("Sem Imagem", "Disponivel");
    delay(2000);
  }
}

void showAccessGranted()
{
  display.clearDisplay();

  // Draw check icon
  display.drawBitmap(56, 5, check_icon, 16, 16, SSD1306_WHITE);

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 30);
  display.println("ACESSO");
  display.setCursor(8, 50);
  display.println("LIBERADO");

  display.display();
}

void showAccessDenied()
{
  display.clearDisplay();

  // Draw error icon
  display.drawBitmap(56, 5, error_icon, 16, 16, SSD1306_WHITE);

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 30);
  display.println("ACESSO");
  display.setCursor(25, 50);
  display.println("NEGADO");

  display.display();
  delay(3000);
}

void displayGuestInfo(String name, String timestamp, String entrada, String saida)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("=== INFO CONVIDADO ===");
  display.println();
  display.println("Nome: " + name);
  display.println();

  if (entrada.length() > 0)
  {
    display.println("Entrada: " + entrada.substring(11, 19));
  }

  if (saida.length() > 0)
  {
    display.println("Saida:   " + saida.substring(11, 19));
  }

  display.display();
}

void displayBitmapImage(String bitmapStr)
{
  display.clearDisplay();
  Serial.println("Processing bitmap data...");

  // Remove unwanted characters
  bitmapStr.replace("{", "");
  bitmapStr.replace("}", "");
  bitmapStr.replace(" ", "");
  bitmapStr.replace("\n", "");
  bitmapStr.replace("\r", "");

  // Create array to store bitmap data
  uint8_t bitmapData[1024]; // 128x64 / 8 = 1024 bytes
  int dataIndex = 0;

  // Parse the hex values
  int pos = 0;
  while (pos < bitmapStr.length() && dataIndex < 1024)
  {
    // Look for "0x" pattern
    int hexStart = bitmapStr.indexOf("0x", pos);
    if (hexStart == -1)
      break;

    // Extract hex value (2 characters after "0x")
    String hexValue = bitmapStr.substring(hexStart + 2, hexStart + 4);

    if (hexValue.length() == 2)
    {
      // Convert hex string to byte
      bitmapData[dataIndex] = (uint8_t)strtol(hexValue.c_str(), NULL, 16);
      dataIndex++;
    }

    pos = hexStart + 4;
  }

  Serial.println("Parsed " + String(dataIndex) + " bytes");

  // Display the bitmap using drawBitmap function
  if (dataIndex > 0)
  {
    display.drawBitmap(0, 0, bitmapData, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    display.display();
    Serial.println("Bitmap displayed successfully");
  }
  else
  {
    showMessage("Erro", "Dados bitmap");
    Serial.println("No valid bitmap data found");
  }
}

void showMessage(String title, String message)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Center title
  int titleWidth = title.length() * 6;
  int titleX = (SCREEN_WIDTH - titleWidth) / 2;
  display.setCursor(titleX, 10);
  display.println(title);

  // Center message
  int messageWidth = message.length() * 6;
  int messageX = (SCREEN_WIDTH - messageWidth) / 2;
  display.setCursor(messageX, 35);
  display.println(message);

  display.display();
}

void showMessageWithIcon(String title, String message, const unsigned char *icon)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Draw icon
  display.drawBitmap(56, 5, icon, 16, 16, SSD1306_WHITE);

  // Center title
  int titleWidth = title.length() * 6;
  int titleX = (SCREEN_WIDTH - titleWidth) / 2;
  display.setCursor(titleX, 25);
  display.println(title);

  // Center message
  int messageWidth = message.length() * 6;
  int messageX = (SCREEN_WIDTH - messageWidth) / 2;
  display.setCursor(messageX, 40);
  display.println(message);

  display.display();
}