# Ubik Framework Architecture

## Overview

The Ubik framework uses a modular architecture to promote code reusability, maintainability, and easy customization. All code is organized into logical modules that can be easily adapted for different hardware configurations.

## Directory Structure

```
relay/
├── relay.ino                # Main sketch file
├── relay.ino       # Hardware-specific configuration
├── WiFiManager.ino          # WiFi management
├── LEDController.ino        # LED control
├── WebServer.ino            # HTTP server
├── APIHandlers.ino          # REST API endpoints
└── data/                    # LittleFS files (HTML, CSS, JS)
    └── index.html           # Web interface
```

**Note**: All `.ino` files must be in the same folder (the sketch folder). Arduino IDE automatically compiles them together in alphabetical order before compiling to C++.

## Module Descriptions

### Main File: relay.ino

The main sketch file that:
- Defines global state variables
- Initializes all modules
- Contains the `setup()` and `loop()` functions
- Handles button input
- Coordinates between modules

**Key Responsibilities:**
- Hardware initialization
- Module coordination
- Main event loop

### Configuration Module: relay.ino

Hardware-specific configuration and pin definitions.

**Supported Hardware:**
- M5Stack Atom Relay (default)
- M5Stack Atom Lite
- M5Stack Atom Matrix
- Custom hardware (user-defined)

**Configuration Options:**
- Pin assignments (relay, button, LED)
- LED type and color order
- Hardware capabilities flags
- WiFi and server settings
- Serial baud rate

**Customization:**
Edit this file to adapt the framework to your specific hardware. This file is compiled first (alphabetically), so all defines are available to other modules.

### Core Modules

All modules are `.ino` files in the relay sketch folder.

#### WiFiManager.ino

Handles all WiFi-related functionality.

**Functions:**
- `readMacAddress()` - Get device MAC address
- `initWiFiAP()` - Initialize Access Point mode

**Future Features:**
- WiFi Client mode
- Dual mode (AP + Client)
- WiFi configuration via serial
- Captive portal

#### LEDController.ino

Manages LED control and status indication.

**Functions:**
- `initLED()` - Initialize FastLED
- `updateLED()` - Update LED based on state
- `setLEDColor(r, g, b)` - Set specific color
- `saveColorForState(r, g, b)` - Save color for current state
- `getLEDColorHex()` - Get current color as hex string

**Features:**
- State-based color management
- Configurable LED type and brightness
- Color persistence per state

#### WebServer.ino

HTTP server configuration and file serving.

**Functions:**
- `initFileSystem()` - Initialize LittleFS
- `initWebServer()` - Configure and start HTTP server
- `handleRoot()` - Serve index.html
- `handleNotFound()` - Serve files from LittleFS
- `handleWebServer()` - Process incoming requests (call in loop)

**Features:**
- Automatic MIME type detection
- LittleFS integration
- Static file serving
- API endpoint registration

### API Module: APIHandlers.ino

REST API endpoint implementations.

**Endpoints:**
- `GET /api/status` - Get current state (relay + LED color)
- `GET /api/relay?state=on|off` - Control relay
- `GET /api/color?color=#RRGGBB` - Set LED color

**Functions:**
- `handleAPIStatus()` - Status endpoint handler
- `handleAPIRelay()` - Relay control handler
- `handleAPIColor()` - LED color handler
- `registerAPIEndpoints()` - Register all endpoints with server

**Features:**
- JSON responses
- Error handling
- Input validation
- Extensible design

## Data Flow

### Initialization Flow

```
relay.ino setup()
    ↓
initHardware()
    ↓
initWiFiAP() → WiFiManager.ino
    ↓
initLED() → LEDController.ino
    ↓
initWebServer() → WebServer.ino
    ↓
    registerAPIEndpoints() → APIHandlers.ino
```

### Request Handling Flow

```
Client Request
    ↓
handleWebServer() → WebServer.ino
    ↓
    ├── / → handleRoot() → serve index.html
    ├── /api/status → handleAPIStatus() → APIHandlers.ino
    ├── /api/relay → handleAPIRelay() → APIHandlers.ino
    ├── /api/color → handleAPIColor() → APIHandlers.ino
    └── /other → handleNotFound() → serve from LittleFS
```

### State Management Flow

```
Button Press OR API Call
    ↓
relayState updated (global variable)
    ↓
updateLED() → LEDController.ino
    ↓
FastLED.show()
```

## Arduino IDE Compilation

Arduino IDE automatically compiles all `.ino` files in the sketch folder in alphabetical order:

1. `relay.ino` (configuration - compiled first!)
2. `APIHandlers.ino`
3. `LEDController.ino`
4. `relay.ino` (main sketch)
5. `WebServer.ino`
6. `WiFiManager.ino`

All files are concatenated together before compilation, which is why:
- No `#include` statements are needed between `.ino` files
- All functions and variables are accessible across files
- Configuration defines in `relay.ino` are available everywhere
- You may see IDE warnings about undefined identifiers, but these resolve during compilation

## Customization Guide

### Adding a New Hardware Configuration

1. Edit [relay.ino](../relay/relay.ino)
2. Add a new `#define HARDWARE_YOUR_DEVICE`
3. Define pin assignments and capabilities
4. Uncomment your hardware selection

### Adding a New API Endpoint

1. Edit [APIHandlers.ino](../relay/APIHandlers.ino)
2. Create handler function: `void handleAPIYourEndpoint() { ... }`
3. Register in `registerAPIEndpoints()`: `server.on("/api/your-endpoint", HTTP_GET, handleAPIYourEndpoint);`

### Modifying the Web Interface

1. Edit [data/index.html](../relay/data/index.html)
2. Use Arduino IDE: Tools → ESP32 LittleFS Data Upload
3. Or use esptool to upload filesystem image

### Adding WiFi Client Mode

1. Edit [WiFiManager.ino](../relay/WiFiManager.ino)
2. Implement `initWiFiClient()` function
3. Add credentials storage (Preferences library)
4. Update main sketch to call appropriate init function

## Global Variables

The following global variables are shared across modules:

- `relayState` (bool) - Current relay state
- `mainLED` (CRGB) - LED color object
- `colorStateOff` (CRGB) - LED color when state is OFF
- `colorStateOn` (CRGB) - LED color when state is ON
- `server` (WebServer) - HTTP server instance

These are declared with `extern` in modules that need to access them.

## Preprocessor Defines

Configuration is controlled through preprocessor defines in `relay.ino`:

- `RELAY_PIN` - GPIO pin for relay control
- `BUTTON_PIN` - GPIO pin for button input
- `LED_PIN` - GPIO pin for LED data
- `LED_TYPE` - FastLED type (WS2812, SK6812, etc.)
- `LED_COLOR_ORDER` - RGB, GRB, BGR, etc.
- `LED_BRIGHTNESS` - 0-255
- `HAS_RELAY` - true/false
- `HAS_LED` - true/false
- `HAS_BUTTON` - true/false
- `BUTTON_DEBOUNCE_DELAY` - milliseconds
- `SERIAL_BAUD_RATE` - baud rate

## Best Practices

### Module Design
- Keep modules focused on a single responsibility
- Use descriptive function names
- Document all public functions
- Minimize global state

### Hardware Abstraction
- Use preprocessor defines for pin assignments
- Check capability flags before accessing hardware
- Support graceful degradation

### Error Handling
- Log errors to Serial
- Return meaningful error messages in API responses
- Handle missing files gracefully

### Memory Management
- Avoid String concatenation in loops
- Use F() macro for string literals
- Stream large responses instead of buffering

## Future Enhancements

### Phase 2: WiFi Configuration
- Client mode support
- Serial configuration interface
- Captive portal
- Web-based WiFi setup

### Phase 3: Security
- Asymmetric key signing
- Request authentication
- HTTPS/TLS support

### Phase 4: Advanced Features
- OTA updates
- mDNS/Bonjour
- MQTT support
- Multi-device management

## Troubleshooting

### Compilation Errors
- Ensure all modules are in the correct directories
- Check that relay.ino has a hardware selected
- Verify library dependencies are installed

### Runtime Issues
- Check Serial output for initialization messages
- Verify pin assignments match your hardware
- Ensure LittleFS data is uploaded

### Network Issues
- Verify WiFi AP is created (check SSID = MAC address)
- Confirm IP address (default: 192.168.4.1)
- Check firewall settings on client device

## Contributing

When adding new modules:
1. Follow the existing naming conventions
2. Document all functions
3. Use preprocessor guards for hardware-specific code
4. Test with multiple hardware configurations
5. Update this documentation

## See Also

- [README.md](../README.md) - Project overview
- [ACTION_PLAN.md](../ACTION_PLAN.md) - Development roadmap
- [relay.ino](../relay/relay.ino) - Hardware configuration
