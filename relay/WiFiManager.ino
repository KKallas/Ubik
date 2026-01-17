// WiFiManager.ino - WiFi configuration and management
// Part of Ubik ESP32 Framework
//
// NOTE: All #include statements are in relay.ino (main file)
//       Arduino IDE concatenates all .ino files before compiling

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

// Initialize WiFi in Access Point mode with MAC address as SSID
void initWiFiAP() {
  Serial.println("Initializing WiFi Access Point...");

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

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
}

// TODO: Future implementation - WiFi Client Mode
// void initWiFiClient(const char* ssid, const char* password) {
//   Serial.println("Connecting to WiFi...");
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   // Add connection logic with timeout and retry
// }

// TODO: Future implementation - Dual Mode (AP + Client)
// void initWiFiDualMode() {
//   WiFi.mode(WIFI_AP_STA);
//   // Configure both AP and Client
// }
