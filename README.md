# Ubik

**ESP32-based scaffolding system for IoT device control with transparent local/remote operation**

Ubik is a framework for building web-controlled ESP32 devices (sensors, actuators, relays) that work seamlessly both locally and remotely through an optional relay server architecture.

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                        Ubik Architecture                        │
└─────────────────────────────────────────────────────────────────┘

Local Development & Testing:
┌──────────┐                ┌─────────────────────────────┐
│  Browser │ ─── HTTP ───>  │  ESP32 (192.168.4.1)        │
│          │                │  - Web Server               │
│          │                │  - HTML Interface           │
│          │                │  - API Endpoints            │
│          │ <── Response ─ │  - I2C/SPI/UART Hardware    │
└──────────┘                └─────────────────────────────┘

Remote Operation (via Relay Server):
┌──────────┐           ┌──────────────────┐          ┌────────────────┐
│  Browser │ ─ HTTP ─> │  Relay Server    │ ─ VPN ─> │  ESP32 (Ubik)  │
│          │           │  (Digital Ocean) │          │  - Same HTML   │
│          │           │  Relays to:      │          │  - Same APIs   │
│          │           │  /api/{ubik_id}/ │          │  - Hardware    │
│          │ <──────── │                  │ <─────── │                │
└──────────┘           └──────────────────┘          └────────────────┘
```

### Communication Flow

```
Server (HTTP) ─[VPN]─> ESP32 (Ubik) ─[I2C/SPI/UART]─> Sensors/Actuators
                          │
                          └─ Serves HTML interface
```

## Key Features

### 🔄 Transparent Local/Remote Operation
- **Same HTML interface** works locally (AP mode) and remotely (via relay server)
- **Same API endpoints** - no code changes needed
- **Edit once, deploy everywhere** - test locally, works remotely automatically

### 🌐 Relay Server Architecture
The relay server:
1. Serves the ESP32's HTML interface
2. Proxies API requests to specific ESP32s via VPN
3. Routes requests using device ID: `/api/{ubik_id}/api?...`
4. Enables remote access without exposing ESP32s to the internet

### 🔌 Hardware Agnostic
- Works with any **I2C**, **SPI**, or **UART** peripherals
- Tested on M5Stack Atom series (Lite, Matrix, Relay)
- Adaptable to any ESP32-based hardware

### 🛡️ Secure by Design
- VPN tunnel between relay server and ESP32 devices
- Optional asymmetric key signing for critical operations
- No direct internet exposure of ESP32 devices

### 📡 Dual WiFi Modes
- **AP Mode**: Creates WiFi access point (SSID = MAC address)
- **Client Mode**: Connects to existing WiFi network
- **Captive Portal**: Auto-configuration on first connection

## How It Works

### Local Operation
1. ESP32 boots in AP mode with SSID = its MAC address
2. Connect to the ESP32's WiFi network
3. Browse to `192.168.4.1`
4. Control device through web interface
5. HTML/CSS/JS served from ESP32's LittleFS
6. API calls go directly to ESP32

### Remote Operation (via Relay Server)
1. ESP32 connects to relay server via VPN
2. Relay server knows all connected ESP32 devices
3. User accesses relay server: `https://relay.example.com`
4. Relay serves the **same HTML** from ESP32
5. API calls are proxied: `/api/{ubik_id}/relay?state=on`
6. Relay forwards request through VPN to specific ESP32
7. Response returns through same path

### The Magic: No Code Changes Needed

When you edit and test your Ubik implementation locally, it automatically works remotely because:
- The HTML interface is **shared** to the relay server
- The relay server **proxies** all `/api/*` requests to the ESP32
- From the browser's perspective, it's just talking to one server
- From the ESP32's perspective, it's just receiving normal HTTP requests

## Example: M5 Atom Relay Controller

The included example demonstrates a relay controller with:
- ✅ LED color control
- ✅ Relay on/off switching
- ✅ Real-time status polling
- ✅ Physical button control
- ✅ Web-based remote control

### API Endpoints

```
GET /api/status          - Get relay state and LED color
GET /api/relay?state=on  - Turn relay on/off
GET /api/color?color=#ff0000 - Set LED color
```

### Relay Server Routing

When accessed through relay server, requests are automatically proxied:

```
Browser → https://relay.example.com/api/AABBCCDDEEFF/relay?state=on
              ↓
Relay Server recognizes AABBCCDDEEFF and forwards via VPN
              ↓
ESP32 (MAC: AABBCCDDEEFF) receives: GET /api/relay?state=on
              ↓
Response flows back through same path
```

## Project Structure

```
Ubik/
├── relay/                    # M5 Atom Relay example implementation
│   ├── relay.ino            # ESP32 firmware
│   └── data/
│       └── index.html       # Web interface (served locally & shared to relay)
├── relay-server/            # [Coming Soon] Relay server implementation
│   ├── server.js            # Node.js/Go relay server
│   ├── vpn-setup.md         # VPN configuration guide
│   └── digital-ocean.md     # Digital Ocean deployment guide
├── ACTION_PLAN.md           # Development roadmap
├── LICENSE
└── README.md               # This file
```

## Quick Start

### 1. Flash ESP32

```bash
# Using Arduino IDE 2.x
1. Open relay/relay.ino
2. Select board: M5Stack Atom
3. Upload sketch
4. Upload LittleFS data (Tools → ESP32 LittleFS Data Upload)
```

### 2. Connect & Configure

```bash
# ESP32 creates AP with SSID = MAC address (e.g., "AABBCCDDEEFF")
1. Connect to ESP32's WiFi network
2. Navigate to 192.168.4.1
3. Control relay and LED color
```

### 3. Deploy Relay Server (Optional)

```bash
# Set up relay server for remote access
1. Deploy relay server to Digital Ocean
2. Configure VPN between server and ESP32s
3. Access devices remotely via relay server
```

Detailed relay server setup instructions coming soon in `/relay-server/` directory.

## Development Workflow

### Testing Locally
```bash
1. Edit relay/data/index.html      # Modify web interface
2. Upload LittleFS to ESP32        # Flash changes
3. Test at 192.168.4.1            # Verify locally
```

### Deploying Remotely
```bash
1. No changes needed!              # Same HTML works remotely
2. Relay server fetches HTML from ESP32
3. Routes API calls via VPN
4. Works immediately
```

## Hardware Requirements

### Minimum Requirements
- ESP32-based microcontroller
- WiFi connectivity
- I2C/SPI/UART peripherals (sensors, actuators, etc.)

### Tested Hardware
- M5Stack Atom Lite
- M5Stack Atom Matrix
- M5Stack Atom Relay

### Relay Server Requirements
- VPS (tested on Digital Ocean)
- VPN server (WireGuard/OpenVPN)
- Node.js or Go runtime

## Roadmap

See [ACTION_PLAN.md](ACTION_PLAN.md) for detailed development roadmap.

### ✅ Phase 1 - Core Framework (Current)
- [x] ESP32 web server with LittleFS
- [x] WiFi AP mode with MAC address SSID
- [x] LED and relay control
- [x] RESTful API endpoints
- [x] Real-time status polling

### 🚧 Phase 2 - WiFi Client Mode & Configuration
- [ ] Dual-mode WiFi (AP + Client)
- [ ] Serial configuration interface
- [ ] Captive portal for easy setup
- [ ] Web-based WiFi configuration
- [ ] QR code generation for device info

### 📋 Phase 3 - Relay Server
- [ ] Relay server implementation
- [ ] VPN configuration guide
- [ ] Device registration system
- [ ] Request proxying with device ID routing
- [ ] HTML interface sharing

### 🔐 Phase 4 - Security
- [ ] Asymmetric key signing (Ed25519/ECDSA)
- [ ] Request authentication
- [ ] Replay attack prevention
- [ ] HTTPS/TLS support

### 📚 Phase 5 - Documentation & Examples
- [ ] Comprehensive API documentation
- [ ] Multiple hardware examples
- [ ] Relay server deployment guide
- [ ] Security best practices

## Use Cases

### Industrial IoT
- Remote sensor monitoring
- Actuator control
- Factory automation
- Process control

### Smart Home
- Custom home automation
- DIY smart devices
- Multi-zone control
- Integration with existing systems

### Prototyping
- Rapid IoT prototyping
- Hardware testing
- API development
- Educational projects

## Security Considerations

### Current Security
- WiFi AP mode (no password by default)
- Local network only
- No authentication

### Planned Security Features
- VPN tunnel for relay communication
- Asymmetric key signing for critical operations
- HTTPS support
- Rate limiting
- Audit logging

**Note**: Current implementation is suitable for development and trusted networks. Production deployments should implement authentication and encryption.

## Contributing

Contributions welcome! Areas of interest:
- Relay server implementation
- Additional hardware examples
- Security improvements
- Documentation
- Testing

## License

MIT License - see [LICENSE](LICENSE)

## Repository Structure

This repository will contain:
1. **Ubik Framework** - Core ESP32 firmware and examples
2. **Relay Server Software** - Server-side relay implementation
3. **Documentation** - Setup guides, API docs, deployment guides

## Related Repositories

- /relay-server - Relay server implementation *(Coming Soon)*
- /ubik-examples - Additional hardware examples *(Coming Soon)*

## Community

- **Issues**: Report bugs and request features via GitHub Issues
- **Discussions**: General questions and architecture discussions

## Credits

Built with:
- [Arduino Core for ESP32](https://github.com/espressif/arduino-esp32)
- [M5Stack Atom](https://shop.m5stack.com/collections/atom-series)

---

**Made with ❤️ for the community**
