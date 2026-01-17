// APIHandlers.ino - REST API endpoint handlers
// Part of Ubik ESP32 Framework
//
// NOTE: All #include statements are in relay.ino (main file)
//       Arduino IDE concatenates all .ino files before compiling

// External references
extern WebServer server;
extern bool relayState;

// Hardware-specific defines - override these in main file for different hardware
#ifndef RELAY_PIN
#define RELAY_PIN 26  // Default for M5 Atom Relay
#endif

// Forward declarations from LEDController
extern void saveColorForState(uint8_t r, uint8_t g, uint8_t b);
extern void updateLED();
extern String getLEDColorHex();

// API: Set LED color
// GET /api/color?color=#RRGGBB
void handleAPIColor() {
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
    saveColorForState(r, g, b);
    Serial.println("LED color overridden for current state");

    server.send(200, "application/json", "{\"success\":true,\"color\":\"#" + colorHex + "\"}");
  } else {
    Serial.println("ERROR: Missing color parameter");
    server.send(400, "application/json", "{\"success\":false,\"error\":\"Missing color parameter\"}");
  }
}

// API: Control relay state
// GET /api/relay?state=on|off|1|0|true|false
void handleAPIRelay() {
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
}

// API: Get current status (LED color + relay state)
// GET /api/status
void handleAPIStatus() {
  Serial.println("API /api/status called");

  // Get current LED color
  String colorHex = getLEDColorHex();

  // Build JSON response
  String json = "{\"success\":true,\"color\":\"";
  json += colorHex;
  json += "\",\"relay\":";
  json += relayState ? "true" : "false";
  json += "}";

  server.send(200, "application/json", json);
}

// Register all API endpoints
void registerAPIEndpoints() {
  Serial.println("Registering API endpoints...");

  server.on("/api/color", HTTP_GET, handleAPIColor);
  server.on("/api/relay", HTTP_GET, handleAPIRelay);
  server.on("/api/status", HTTP_GET, handleAPIStatus);

  // TODO: Add more API endpoints here
  // server.on("/api/config", HTTP_GET, handleAPIConfig);
  // server.on("/api/device-info", HTTP_GET, handleAPIDeviceInfo);

  Serial.println("API endpoints registered");
}
