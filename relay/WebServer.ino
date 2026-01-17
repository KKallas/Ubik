// WebServer.ino - HTTP server configuration and routing
// Part of Ubik ESP32 Framework
//
// NOTE: All #include statements are in relay.ino (main file)
//       Arduino IDE concatenates all .ino files before compiling

// Global web server instance
WebServer server(80);

// Forward declaration from APIHandlers
extern void registerAPIEndpoints();

// Initialize LittleFS filesystem
void initFileSystem() {
  Serial.println("Initializing LittleFS...");

  // Initialize LittleFS
  // Upload files using: Shift + Command + P "Upload LittleFS..."
  // Installation guide: https://randomnerdtutorials.com/arduino-ide-2-install-esp32-littlefs/
  if (!LittleFS.begin(true)) {
    Serial.println("ERROR: LittleFS Mount Failed");
    return;
  }

  Serial.println("LittleFS mounted successfully");
}

// Serve root page (index.html)
void handleRoot() {
  File file = LittleFS.open("/index.html", "r");
  if (!file) {
    server.send(404, "text/plain", "File Not Found");
    return;
  }
  server.streamFile(file, "text/html");
  file.close();
}

// Handle file requests from LittleFS
void handleNotFound() {
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
  else if (path.endsWith(".svg")) contentType = "image/svg+xml";

  server.streamFile(file, contentType);
  file.close();
}

// Initialize web server
void initWebServer() {
  Serial.println("Initializing web server...");

  // Initialize filesystem first
  initFileSystem();

  // Route for root / web page
  server.on("/", HTTP_GET, handleRoot);

  // Register API endpoints (from APIHandlers.ino)
  registerAPIEndpoints();

  // Handle all other requests by checking if file exists in LittleFS
  server.onNotFound(handleNotFound);

  // Start server
  server.begin();
  Serial.println("HTTP server started on port 80");
}

// Handle incoming HTTP requests (call this in loop())
void handleWebServer() {
  server.handleClient();
}
