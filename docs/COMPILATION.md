# Arduino Compilation Guide

## How Arduino IDE Compiles Multiple .ino Files

Arduino IDE has a special compilation process for sketches with multiple `.ino` files:

### Step 1: File Concatenation
Arduino IDE concatenates all `.ino` files in **alphabetical order** into a single file before compilation:

```
00_HardwareConfig.ino   ← First (all #defines available to everything)
APIHandlers.ino
LEDController.ino
relay.ino              ← Main sketch
WebServer.ino
WiFiManager.ino         ← Last
```

### Step 2: Include Processing
Only the **first file** (by alphabetical order) should have `#include` statements. In our case:
- ✅ `relay.ino` has all `#include <...>` statements
- ❌ No other `.ino` files should have `#include` statements

### Step 3: Compilation
After concatenation, the entire sketch is compiled as a single C++ file.

## File Structure Rules

### relay.ino (Main File)
**Must contain:**
- All `#include` statements for libraries
- Global variable declarations
- `setup()` function
- `loop()` function

**Example:**
```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <FastLED.h>

bool relayState = false;

void setup() {
  // initialization
}

void loop() {
  // main loop
}
```

### 00_HardwareConfig.ino (Configuration)
**Must contain:**
- Only `#define` preprocessor directives
- No `#include` statements
- No function definitions

**Example:**
```cpp
#define RELAY_PIN 26
#define LED_PIN 27
#define SERIAL_BAUD_RATE 115200
```

### Other Module Files (WiFiManager.ino, LEDController.ino, etc.)
**Must contain:**
- Function definitions only
- `extern` declarations for variables from other files
- No `#include` statements

**Example:**
```cpp
// WiFiManager.ino
extern WebServer server;  // Declared in relay.ino

void initWiFiAP() {
  // function implementation
}
```

## Why This Structure?

### Problem: Duplicate Includes
If multiple `.ino` files have the same `#include`, you get:
```cpp
// After concatenation:
#include <WiFi.h>    // from file 1
#include <WiFi.h>    // from file 2 - DUPLICATE!
```

### Solution: Single Include Point
Only `relay.ino` has includes:
```cpp
// After concatenation:
#include <WiFi.h>    // Only once!
// ... rest of concatenated code
```

## Common Issues

### Issue 1: "undefined reference to function"
**Cause:** Function called before it's defined (alphabetical order issue)

**Solution:**
- Move function to earlier file alphabetically, OR
- Add forward declaration in earlier file

### Issue 2: "'CONSTANT' was not declared in this scope"
**Cause:** Constant defined in file that comes later alphabetically

**Solution:**
- Rename file with `00_` prefix to force early compilation
- Example: `HardwareConfig.ino` → `00_HardwareConfig.ino`

### Issue 3: IDE shows errors but Arduino compiles fine
**Cause:** VS Code/IDE doesn't understand Arduino's concatenation

**Solution:**
- Ignore IDE warnings for "undefined" identifiers
- If it compiles in Arduino IDE, it's correct
- IDE errors are cosmetic only

## Compilation Order in This Project

When you compile in Arduino IDE, files are processed in this order:

```
1. 00_HardwareConfig.ino  → All #defines available
   ↓
2. APIHandlers.ino        → API endpoint handlers
   ↓
3. LEDController.ino      → LED control functions
   ↓
4. relay.ino              → Main sketch with #includes, setup(), loop()
   ↓
5. WebServer.ino          → HTTP server functions
   ↓
6. WiFiManager.ino        → WiFi functions
   ↓
7. Concatenated file is compiled as single C++ file
```

## Best Practices

### DO:
- ✅ Put all `#include` statements in main `.ino` file
- ✅ Use `00_` prefix for configuration files that need early compilation
- ✅ Use `extern` declarations in modules that reference global variables
- ✅ Keep function definitions in separate module files
- ✅ Use descriptive filenames (e.g., `WiFiManager.ino`, not `wifi.ino`)

### DON'T:
- ❌ Add `#include` statements to module files
- ❌ Define global variables in multiple files
- ❌ Rely on specific compilation order without using filename prefixes
- ❌ Mix configuration defines with function definitions

## Testing Your Structure

To verify correct structure:

```bash
# Check that only main file has includes
grep -n "^#include" *.ino

# Should only show: relay.ino with #include statements
```

## VS Code Warnings

You may see these warnings in VS Code - **they are normal and can be ignored**:

```
- "identifier 'CONSTANT' is undefined"
- "cannot open source file"
- "identifier 'function' is undefined"
```

These warnings appear because VS Code analyzes each file individually, but Arduino IDE concatenates them all together before compiling.

**If Arduino IDE compiles successfully, your code is correct!**

## Summary

The key principle: **Arduino concatenates all .ino files alphabetically into one file before compiling.**

This means:
- Only one set of `#include` statements needed (in main file)
- All functions are globally accessible (no imports needed)
- File naming controls compilation order
- IDE warnings may not reflect actual compilation behavior

For more details on the architecture, see [ARCHITECTURE.md](ARCHITECTURE.md).
