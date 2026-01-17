# Quick Start Guide

## File Structure

```
relay/
├── relay.ino            # Main file: config + includes + setup() + loop()
├── APIHandlers.ino      # REST API endpoints
├── LEDController.ino    # LED control functions
├── WebServer.ino        # HTTP server functions
├── WiFiManager.ino      # WiFi functions
└── data/
    └── index.html       # Web interface
```

## Customizing Hardware

Edit [relay.ino](../relay/relay.ino) at the top:

```cpp
// Pin Assignments
#define RELAY_PIN 26              // Change GPIO pins
#define BUTTON_PIN 39
#define LED_PIN 27

// LED Configuration
#define LED_TYPE SK6812           // Change LED type
#define LED_COLOR_ORDER GRB
#define LED_BRIGHTNESS 10

// Hardware Features (comment out to disable)
#define HAS_RELAY                 // Comment out if no relay
#define HAS_LED                   // Comment out if no LED
#define HAS_BUTTON                // Comment out if no button
```

## Compilation

Arduino IDE concatenates files alphabetically:
1. `APIHandlers.ino`
2. `LEDController.ino`
3. `relay.ino` ← Has all #includes and forward declarations
4. `WebServer.ino`
5. `WiFiManager.ino`

**Important:** Only `relay.ino` has:
- `#include` statements
- Forward declarations
- Hardware configuration
- setup() and loop()

Other files have ONLY function definitions.

## Common Issues

### "Function not declared"
✅ **Fixed:** Forward declarations added to `relay.ino`

### "Undefined #define"
✅ **Fixed:** All defines are in `relay.ino` which compiles with other files

### IDE shows errors
✅ **Normal:** VS Code doesn't understand Arduino's file concatenation. If Arduino IDE compiles successfully, it works!

## Upload Steps

1. **Open in Arduino IDE:** File → Open → relay/relay.ino
2. **Select Board:** M5Stack Atom
3. **Upload Sketch:** Click Upload button
4. **Upload LittleFS:** Tools → ESP32 LittleFS Data Upload
5. **Done!** Connect to WiFi (SSID = MAC address), browse to 192.168.4.1

## API Endpoints

Once running, access:

- `http://192.168.4.1/` - Web interface
- `http://192.168.4.1/api/status` - Get relay state & LED color
- `http://192.168.4.1/api/relay?state=on` - Turn relay on
- `http://192.168.4.1/api/relay?state=off` - Turn relay off
- `http://192.168.4.1/api/color?color=#ff0000` - Set LED color

## Customization Examples

### Disable Button
```cpp
// #define HAS_BUTTON    // Comment out this line
```

### Change LED Type to WS2812
```cpp
#define LED_TYPE WS2812
```

### Add New API Endpoint
Edit `APIHandlers.ino`:
```cpp
void handleAPICustom() {
  // Your code
  server.send(200, "application/json", "{\"success\":true}");
}

void registerAPIEndpoints() {
  // ... existing endpoints
  server.on("/api/custom", HTTP_GET, handleAPICustom);
}
```

## Serial Monitor

Baud rate: `115200`

You'll see:
```
========================================
Ubik ESP32 Framework
M5 Atom Relay Controller
========================================

Initializing hardware...
✓ Relay initialized (OFF)
✓ Button initialized on GPIO39
...
System Ready!
Connect to WiFi and navigate to:
http://192.168.4.1
========================================
```

## Next Steps

- See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed documentation
- See [ACTION_PLAN.md](../ACTION_PLAN.md) for roadmap
- See [README.md](../README.md) for project overview
