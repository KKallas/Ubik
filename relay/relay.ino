#include <WiFi.h>
#include <esp_wifi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <FastLED.h>

// M5 Atom Relay pin definitions
#define RELAY_PIN 26  // GPIO26 controls the relay on M5 Atom Relay
#define BUTTON_PIN 39  // GPIO39 button on M5 Atom

CRGB mainLED;
WebServer server(80);
bool relayState = false;  // Track relay state (off by default)

// LED colors for relay states (user can override these)
CRGB colorRelayOff = CRGB::Green;   // Green when relay is OFF (default)
CRGB colorRelayOn = CRGB::Red;      // Red when relay is ON (default)

// Button debounce variables
bool lastButtonState = HIGH;
bool buttonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;  // 50ms debounce

// Update LED color based on relay state
void updateLED() {
  mainLED = relayState ? colorRelayOn : colorRelayOff;
  FastLED.show();
}

// Read MAC address directly from ESP32 WiFi hardware
// WiFi.macAddress() returns "00:00:00:00:00:00" in AP mode before STA is initialized
// This function uses esp_wifi_get_mac() to get the real MAC address and formats it without colons
String readMacAddress() {
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    char macStr[13];
    sprintf(macStr, "%02X%02X%02X%02X%02X%02X",
            baseMac[0], baseMac[1], baseMac[2],
            baseMac[3], baseMac[4], baseMac[5]);
    return String(macStr);
  } else {
    Serial.println("Failed to read MAC address");
    return String("000000000000");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("\n\nM5 Atom Relay Web Server Starting...");

  // Initialize relay pin (off by default)
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  relayState = false;
  Serial.println("Relay initialized (OFF)");

  // Initialize button pin with internal pullup
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Button initialized on GPIO39");

  // Initialize FastLED for LED control
  FastLED.addLeds<SK6812, 27, GRB>(&mainLED, 1);
  FastLED.setBrightness(10);

  Serial.println("FastLED initialized");

  // Set WiFi mode before getting MAC
  WiFi.mode(WIFI_AP);
  delay(100);

  // Get MAC address without colons using esp_wifi
  String mac = readMacAddress();

  Serial.println("Starting Access Point...");
  Serial.print("SSID: ");
  Serial.println(mac);

  // Start WiFi AP with MAC address as SSID (no password)
  WiFi.softAP(mac.c_str());
  delay(100);

  // Initialize LittleFS
  // Shift + Command + P "Upload little FS..."
  // https://randomnerdtutorials.com/arduino-ide-2-install-esp32-littlefs/#installing-MAC
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS Mount Failed");
    return;
  }
  Serial.println("LittleFS mounted successfully");

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Route for root / web page
  server.on("/", HTTP_GET, []() {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
      server.send(404, "text/plain", "File Not Found");
      return;
    }
    server.streamFile(file, "text/html");
    file.close();
  });

  // API endpoint to set LED color
  server.on("/api/color", HTTP_GET, []() {
    Serial.println("API /api/color called");

    if (server.hasArg("color")) {
      String colorHex = server.arg("color");
      Serial.print("Received color parameter: ");
      Serial.println(colorHex);

      // Remove # if present
      if (colorHex.startsWith("#")) {
        colorHex = colorHex.substring(1);
      }

      // Parse hex color to RGB
      long colorValue = strtol(colorHex.c_str(), NULL, 16);
      uint8_t r = (colorValue >> 16) & 0xFF;
      uint8_t g = (colorValue >> 8) & 0xFF;
      uint8_t b = colorValue & 0xFF;

      Serial.print("Setting LED - R:");
      Serial.print(r);
      Serial.print(" G:");
      Serial.print(g);
      Serial.print(" B:");
      Serial.println(b);

      // Save this color for the current relay state
      CRGB newColor = CRGB(r, g, b);
      if (relayState) {
        colorRelayOn = newColor;
      } else {
        colorRelayOff = newColor;
      }

      // Update LED with new color
      mainLED = newColor;
      FastLED.show();
      Serial.println("LED color overridden for current state");

      server.send(200, "application/json", "{\"success\":true,\"color\":\"#" + colorHex + "\"}");
    } else {
      Serial.println("ERROR: Missing color parameter");
      server.send(400, "application/json", "{\"success\":false,\"error\":\"Missing color parameter\"}");
    }
  });

  // API endpoint to control relay
  server.on("/api/relay", HTTP_GET, []() {
    Serial.println("API /api/relay called");

    if (server.hasArg("state")) {
      String stateParam = server.arg("state");
      Serial.print("Received state parameter: ");
      Serial.println(stateParam);

      if (stateParam == "on" || stateParam == "1" || stateParam == "true") {
        digitalWrite(RELAY_PIN, HIGH);
        relayState = true;
        updateLED();
        Serial.println("Relay turned ON");
        server.send(200, "application/json", "{\"success\":true,\"relay\":true}");
      } else if (stateParam == "off" || stateParam == "0" || stateParam == "false") {
        digitalWrite(RELAY_PIN, LOW);
        relayState = false;
        updateLED();
        Serial.println("Relay turned OFF");
        server.send(200, "application/json", "{\"success\":true,\"relay\":false}");
      } else {
        server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid state parameter\"}");
      }
    } else {
      server.send(400, "application/json", "{\"success\":false,\"error\":\"Missing state parameter\"}");
    }
  });

  // API endpoint to get status (LED color + relay state)
  server.on("/api/status", HTTP_GET, []() {
    Serial.println("API /api/status called");

    // Get current LED color
    uint8_t r = mainLED.r;
    uint8_t g = mainLED.g;
    uint8_t b = mainLED.b;
    char colorHex[8];
    sprintf(colorHex, "#%02X%02X%02X", r, g, b);

    // Build JSON response
    String json = "{\"success\":true,\"color\":\"";
    json += colorHex;
    json += "\",\"relay\":";
    json += relayState ? "true" : "false";
    json += "}";

    server.send(200, "application/json", json);
  });

  // Handle all other requests by checking if file exists in LittleFS
  server.onNotFound([]() {
    String path = server.uri();
    File file = LittleFS.open(path, "r");
    if (!file) {
      server.send(404, "text/plain", "File Not Found");
      return;
    }

    // Determine content type based on file extension
    String contentType = "text/plain";
    if (path.endsWith(".html")) contentType = "text/html";
    else if (path.endsWith(".css")) contentType = "text/css";
    else if (path.endsWith(".js")) contentType = "application/javascript";
    else if (path.endsWith(".json")) contentType = "application/json";
    else if (path.endsWith(".png")) contentType = "image/png";
    else if (path.endsWith(".jpg") || path.endsWith(".jpeg")) contentType = "image/jpeg";
    else if (path.endsWith(".gif")) contentType = "image/gif";
    else if (path.endsWith(".ico")) contentType = "image/x-icon";

    server.streamFile(file, contentType);
    file.close();
  });

  server.begin();
  Serial.println("HTTP server started");

  // Set LED to green to indicate ready
  mainLED = CRGB::Green;
  FastLED.show();
  Serial.println("LED set to green - ready!");
}

void loop() {
  server.handleClient();

  // Read button state with debounce
  bool reading = digitalRead(BUTTON_PIN);

  // If button reading changed, reset debounce timer
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  // Check if enough time has passed to consider it a stable state
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the button state has changed (after debounce)
    if (reading != buttonState) {
      buttonState = reading;

      // Only toggle on button press (transition from HIGH to LOW)
      if (buttonState == LOW) {
        // Toggle relay
        relayState = !relayState;
        digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
        updateLED();

        Serial.print("Button pressed - Relay toggled to: ");
        Serial.println(relayState ? "ON" : "OFF");
      }
    }
  }

  lastButtonState = reading;
  delay(10);
}
