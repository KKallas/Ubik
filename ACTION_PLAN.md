# M5Stack ESP32 Boilerplate Project - Action Plan

## Project Overview
Transform the current M5 Atom Relay project into a reusable boilerplate for M5Stack ESP32 products with WiFi connectivity, web interface, and security features.

---

## Phase 1: Project Restructuring & Boilerplate Setup

### 1.1 Repository Structure
- [ ] Create GitHub repository structure
- [ ] Add `.gitignore` for Arduino projects
- [ ] Create `README.md` with setup instructions
- [ ] Create `LICENSE` file
- [ ] Organize code into modular structure:
  ```
  /src
    /core          - Core WiFi, server, LED functionality
    /config        - Configuration management
    /security      - Authentication and signing
    /api           - API endpoint handlers
  /data            - LittleFS web files
  /examples        - Example implementations
  /docs            - Documentation
  ```

### 1.2 Modularize Code
- [ ] Extract WiFi management into separate module (`WiFiManager.ino`)
- [ ] Extract web server setup into module (`WebServer.ino`)
- [ ] Extract API handlers into module (`APIHandlers.ino`)
- [ ] Extract LED control into module (`LEDController.ino`)
- [ ] Create main template file that includes all modules

### 1.3 Create Boilerplate Template
- [ ] Make hardware-specific defines configurable (relay pin, LED pin, etc.)
- [ ] Create placeholder API endpoints that can be easily customized
- [ ] Template HTML/CSS/JS structure for easy customization
- [ ] Add comments explaining where to add custom functionality

---

## Phase 2: WiFi Client Mode & Serial Configuration

### 2.1 WiFi Modes Implementation
- [ ] Implement dual-mode WiFi (AP + Client)
- [ ] Add WiFi mode selection (AP only, Client only, or Both)
- [ ] Store WiFi credentials in NVS (Non-Volatile Storage)
- [ ] Add automatic AP fallback if client connection fails
- [ ] Implement WiFi reconnection logic with exponential backoff

### 2.2 Serial Configuration Interface
- [ ] Create serial command parser
- [ ] Implement commands:
  - [ ] `wifi-scan` - Scan and list available networks
  - [ ] `wifi-set <ssid> <password>` - Set WiFi credentials
  - [ ] `wifi-mode <ap|client|both>` - Set WiFi mode
  - [ ] `wifi-status` - Show current WiFi status
  - [ ] `wifi-reset` - Clear saved credentials
  - [ ] `config-show` - Display current configuration
  - [ ] `config-reset` - Reset to factory defaults
- [ ] Add command help system (`help` command)
- [ ] Implement input validation and error handling
- [ ] Add command history/autocomplete (optional, nice-to-have)

### 2.3 Captive Portal for AP Mode
- [ ] Implement DNS server to redirect all requests to device IP
  - Use `DNSServer` library for ESP32
- [ ] Configure captive portal detection responses:
  - [ ] Handle Apple CaptiveNetwork detection (iOS/macOS)
    - Respond to `http://captive.apple.com/hotspot-detect.html`
  - [ ] Handle Android captive portal detection
    - Respond to `http://connectivitycheck.gstatic.com/generate_204`
  - [ ] Handle Windows NCSI (Network Connectivity Status Indicator)
    - Respond to `http://www.msftconnecttest.com/connecttest.txt`
- [ ] Auto-redirect all HTTP requests to setup page
- [ ] Show WiFi configuration interface immediately on connection
- [ ] Test captive portal on iOS, Android, Windows, macOS
- [ ] Handle HTTPS redirect gracefully (show instructions if HTTPS fails)

### 2.4 QR Code Page (Fully Customizable HTML in LittleFS)
- [ ] Create example `/qr-code.html` page in LittleFS data folder
  - Customers can edit this file completely to their needs
  - Uses client-side JavaScript for QR code rendering (no ESP32 processing)
  - Works with any QR/barcode library user wants to include
- [ ] Default template includes:
  - [ ] Client-side QR code generation using lightweight JS library (e.g., qrcodejs)
  - [ ] Fetches device info from `/api/device-info` endpoint
  - [ ] Displays WiFi SSID (MAC address) in QR code format
  - [ ] Print-friendly CSS styling
  - [ ] Customizable branding area (logo, company name, etc.)
- [ ] Create `/api/device-info` endpoint that returns JSON:
  ```json
  {
    "mac": "AABBCCDDEEFF",
    "ip": "192.168.4.1",
    "model": "M5Atom-Relay",
    "version": "1.0.0"
  }
  ```
- [ ] Benefits of this approach:
  - [ ] Users can completely redesign the page without touching Arduino code
  - [ ] Can switch between QR codes, barcodes, or any format
  - [ ] Can add custom fields, instructions, multiple languages
  - [ ] Can include external libraries via CDN or local files
  - [ ] Just edit HTML/CSS/JS and re-upload to LittleFS
- [ ] Documentation:
  - [ ] Provide multiple template examples (QR code, barcode, NFC data)
  - [ ] Show how to customize for different use cases
  - [ ] Explain WiFi QR code format: `WIFI:T:nopass;S:<SSID>;P:;;`

### 2.5 Web-Based WiFi Configuration
- [ ] Create WiFi setup page (`/setup`)
- [ ] Add WiFi network scanner in web interface
- [ ] Allow WiFi configuration through web UI
- [ ] Show connection status in real-time
- [ ] Add "Forget Network" functionality
- [ ] Support both AP and Client mode configuration
- [ ] Show signal strength for available networks

### 2.6 Configuration Persistence
- [ ] Use ESP32 Preferences library for NVS storage
- [ ] Store WiFi credentials securely
- [ ] Store WiFi mode preference
- [ ] Store custom configuration (relay states, colors, etc.)
- [ ] Add factory reset functionality
- [ ] Implement config backup/restore via serial

---

## Phase 3: Asymmetric Key Signing & Security

### 3.1 Cryptographic Setup
- [ ] Research and select crypto library (mbedTLS, wolfSSL, or ESP32's built-in)
- [ ] Implement Ed25519 or ECDSA (secp256r1) for signing
  - Rationale: Small signatures, fast verification, good for embedded
- [ ] Generate and store device private key in secure storage
- [ ] Create public key distribution mechanism

### 3.2 Message Signing Implementation
- [ ] Create signing module (`MessageSigner.h/cpp`)
- [ ] Implement functions:
  - [ ] `signMessage(data)` - Sign arbitrary data
  - [ ] `verifySignature(data, signature, publicKey)` - Verify signature
  - [ ] `getPublicKey()` - Export public key
- [ ] Define critical endpoints that require signing:
  - `/api/relay` - Relay control
  - `/api/color` - LED control (optional)
  - `/setup` - Configuration changes
  - Custom critical endpoints

### 3.3 Request Authentication Flow
- [ ] Add signature field to API requests
- [ ] Implement middleware to verify signatures
- [ ] Add timestamp/nonce to prevent replay attacks
- [ ] Create challenge-response mechanism for initial pairing
- [ ] Handle signature verification failures gracefully

### 3.4 Key Management
- [ ] Generate device keypair on first boot
- [ ] Store private key in ESP32 secure storage (if available) or encrypted NVS
- [ ] Create key rotation mechanism (optional, advanced)
- [ ] Implement key export/import for backup
- [ ] Add command to view public key via serial: `security-pubkey`

### 3.5 Client-Side Implementation
- [ ] Create JavaScript crypto library integration (SubtleCrypto API)
- [ ] Store trusted public key in web app
- [ ] Implement request signing in JavaScript
- [ ] Add signature to API calls automatically
- [ ] Handle authentication errors in UI

### 3.6 Pairing/Trust Establishment
- [ ] Create initial pairing flow:
  1. Device generates QR code with public key (displayed on serial or LED blink pattern)
  2. User scans QR code or enters key manually
  3. Web app stores public key for future requests
- [ ] Alternative: Show public key on first web access and require confirmation
- [ ] Implement trust revocation mechanism

---

## Phase 4: Enhanced Security Features

### 4.1 Additional Security Measures
- [ ] Add rate limiting to prevent brute force
- [ ] Implement CORS headers for cross-origin protection
- [ ] Add HTTPS/TLS support (optional, but recommended)
  - Generate self-signed certificate or use Let's Encrypt
- [ ] Implement session tokens for web interface
- [ ] Add IP whitelisting (optional)

### 4.2 Audit & Logging
- [ ] Log all critical operations (relay changes, config updates)
- [ ] Add serial logging of authentication attempts
- [ ] Store last N operations in circular buffer
- [ ] Create `/api/audit-log` endpoint (protected)

---

## Phase 5: Documentation & Examples

### 5.1 Documentation
- [ ] Write comprehensive README with:
  - Features overview
  - Hardware requirements
  - Installation instructions
  - Configuration guide
  - API documentation
  - Security model explanation
- [ ] Create quick-start guide
- [ ] Document serial commands
- [ ] Create architecture diagrams
- [ ] Add code comments and inline documentation

### 5.2 Example Implementations
- [ ] Create example 1: Simple LED controller
- [ ] Create example 2: Relay controller (current project)
- [ ] Create example 3: Sensor dashboard
- [ ] Create example 4: Multi-device controller
- [ ] Add Postman collection for API testing

### 5.3 Testing & Validation
- [ ] Create test suite for API endpoints
- [ ] Test WiFi modes and failover
- [ ] Test security/signing implementation
- [ ] Test serial configuration commands
- [ ] Perform security audit

---

## Phase 6: Optional Enhancements

### 6.1 Advanced Features
- [ ] OTA (Over-The-Air) updates
- [ ] mDNS/Bonjour for device discovery
- [ ] MQTT support for IoT integration
- [ ] Home Assistant integration
- [ ] RESTful API with OpenAPI documentation
- [ ] WebSocket support for real-time updates (alternative to SSE)

### 6.2 Developer Experience
- [ ] Create CLI tool for device management
- [ ] Add VS Code extension/snippets
- [ ] Create web-based device manager
- [ ] Add debugging tools and utilities

---

## Implementation Priority

### High Priority (Must Have)
1. Phase 1: Project restructuring
2. Phase 2: WiFi client mode & serial config
3. Phase 5.1: Basic documentation

### Medium Priority (Should Have)
4. Phase 3: Asymmetric key signing
5. Phase 4.1: Additional security
6. Phase 5.2: Examples

### Low Priority (Nice to Have)
7. Phase 4.2: Audit logging
8. Phase 6: Optional enhancements

---

## Technology Stack

### Hardware
- M5Stack Atom Lite / Atom Matrix / Atom Relay
- ESP32-based devices

### Software
- **Arduino Framework** - Core development
- **FastLED** - LED control
- **WebServer** - HTTP server
- **DNSServer** - Captive portal DNS redirection
- **LittleFS** - File system
- **Preferences/NVS** - Configuration storage
- **mbedTLS** - Cryptography (built into ESP32)

### Web Stack
- **Vanilla JavaScript** - No frameworks for minimal size
- **SubtleCrypto API** - Client-side cryptography
- **Client-side QR/Barcode libraries** - User choice (e.g., qrcodejs, JsBarcode)
  - Included via CDN or stored in LittleFS
  - Fully customizable by end user
- **Responsive CSS** - Mobile-first design

---

## Security Considerations

### Threat Model
- **Unauthorized relay control** - Mitigated by signature verification
- **WiFi sniffing** - Mitigated by HTTPS (optional)
- **Replay attacks** - Mitigated by timestamp/nonce
- **Physical access** - Mitigated by secure key storage
- **Man-in-the-middle** - Mitigated by signature verification + HTTPS

### Security Best Practices
- Never transmit private keys
- Use secure random number generation
- Implement proper key storage
- Validate all inputs
- Use constant-time comparisons for signatures
- Implement rate limiting

---

## Timeline Estimate

- **Phase 1**: 1-2 days
- **Phase 2**: 2-3 days
- **Phase 3**: 3-4 days (crypto is complex)
- **Phase 4**: 1-2 days
- **Phase 5**: 1-2 days
- **Phase 6**: Variable (optional features)

**Total**: ~10-15 days of focused development

---

## Next Steps

1. Review and approve this plan
2. Set up GitHub repository
3. Begin Phase 1: Project restructuring
4. Implement phases in priority order
5. Test thoroughly at each phase
6. Document as you go

---

## Notes

- Keep backward compatibility in mind during refactoring
- Focus on code reusability and modularity
- Prioritize security without sacrificing usability
- Make configuration easy but secure
- Document everything for future developers

---

## Implementation Notes: Captive Portal & QR Codes

### Captive Portal Flow
1. Device boots in AP mode with SSID = MAC address (no password)
2. DNS server redirects ALL domains to device IP (192.168.4.1)
3. When phone/tablet connects, captive portal detection triggers:
   - iOS/macOS: Requests `http://captive.apple.com/hotspot-detect.html`
   - Android: Requests `http://connectivitycheck.gstatic.com/generate_204`
   - Windows: Requests `http://www.msftconnecttest.com/connecttest.txt`
4. Device responds with appropriate status codes to trigger portal popup
5. User sees WiFi configuration interface automatically
6. User configures home WiFi and device switches to client mode

### QR Code Benefits
- **Instant Connection**: Scan QR → Connect to WiFi → Portal opens
- **No Typing**: MAC address SSIDs are hard to type manually
- **Printable Labels**: Generate stickers for device deployment
- **Professional**: Clean, modern UX for IoT devices
- **Documentation**: QR code serves as visual device identifier

### QR Code Use Cases
1. **Initial Setup**: Print QR codes for new device deployments
2. **Field Installation**: Technicians scan to configure devices
3. **Customer Setup**: End users can self-configure devices
4. **Device Recovery**: Factory reset → scan QR → reconfigure
5. **Multi-Device**: Each device has unique QR code (based on MAC)

### Technical Implementation Tips
- DNSServer listens on port 53
- Respond to ALL DNS queries with device IP
- Handle both `/generate_204` and redirect requests
- Test on multiple platforms (iOS/Android behave differently)
- Keep captive portal page lightweight (< 50KB)
- Use inline CSS/JS to avoid extra requests
- Provide fallback instructions if portal doesn't auto-open

---

**Last Updated**: 2026-01-16
**Status**: Planning Phase
**Version**: 1.1
