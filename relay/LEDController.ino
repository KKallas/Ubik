// LEDController.ino - LED control and status indication
// Part of Ubik ESP32 Framework
//
// NOTE: All #include statements are in relay.ino (main file)
//       Arduino IDE concatenates all .ino files before compiling

// LED configuration - make these configurable for different hardware
#ifndef LED_PIN
#define LED_PIN 27  // Default for M5 Atom
#endif

#ifndef LED_TYPE
#define LED_TYPE SK6812
#endif

#ifndef LED_COLOR_ORDER
#define LED_COLOR_ORDER GRB
#endif

#ifndef LED_BRIGHTNESS
#define LED_BRIGHTNESS 10  // 0-255, keep low for power efficiency
#endif

// Global LED instance
CRGB mainLED;

// LED colors for states (can be overridden by API)
CRGB colorStateOff = CRGB::Green;   // Default color when state is OFF
CRGB colorStateOn = CRGB::Red;      // Default color when state is ON

// Track current state for LED updates
extern bool relayState;  // Defined in main file or API handlers

// Initialize LED controller
void initLED() {
  Serial.println("Initializing LED controller...");

  FastLED.addLeds<LED_TYPE, LED_PIN, LED_COLOR_ORDER>(&mainLED, 1);
  FastLED.setBrightness(LED_BRIGHTNESS);

  // Set initial LED color based on state
  updateLED();

  Serial.println("LED controller initialized");
}

// Update LED color based on current state
void updateLED() {
  mainLED = relayState ? colorStateOn : colorStateOff;
  FastLED.show();
}

// Set LED to specific color (used by API)
void setLEDColor(uint8_t r, uint8_t g, uint8_t b) {
  mainLED = CRGB(r, g, b);
  FastLED.show();
}

// Save color for current state
void saveColorForState(uint8_t r, uint8_t g, uint8_t b) {
  CRGB newColor = CRGB(r, g, b);
  if (relayState) {
    colorStateOn = newColor;
  } else {
    colorStateOff = newColor;
  }
  setLEDColor(r, g, b);
}

// Get current LED color as hex string
String getLEDColorHex() {
  char colorHex[8];
  sprintf(colorHex, "#%02X%02X%02X", mainLED.r, mainLED.g, mainLED.b);
  return String(colorHex);
}
