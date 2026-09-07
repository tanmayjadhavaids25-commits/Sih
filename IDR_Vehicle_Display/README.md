# IDR Vehicle Display (Arduino UNO + 3.5" TFT Shield)

Professional automotive-style dashboard for **SIH 2026 AI-Driven Intelligent Dead Reckoning (IDR)**.

This project is for:
- **Arduino UNO** (display controller only)
- **3.5-inch TFT Arduino Shield** (vehicle display)
- Future backend:
  - Raspberry Pi = IDR engine (AI/EKF/NHC/map matching)
  - ESP32 = GNSS + IMU source to Raspberry Pi

Arduino role is strictly: **RECEIVE -> PARSE -> DISPLAY**.

---

## 1) Required Arduino libraries

Install these from Arduino Library Manager:

1. **MCUFRIEND_kbv** (by David Prentice)
2. **Adafruit GFX Library**
3. **TouchScreen** (only needed if `ENABLE_TOUCH` is true)

---

## 2) Exact installation steps

1. Open Arduino IDE.
2. Go to **Sketch -> Include Library -> Manage Libraries**.
3. Search and install:
   - `MCUFRIEND_kbv`
   - `Adafruit GFX Library`
   - `TouchScreen` (optional)
4. Open `/home/runner/work/Sih/Sih/IDR_Vehicle_Display/IDR_Vehicle_Display.ino`.
5. Ensure all `.h/.cpp` files in this folder are visible in the IDE tab list.

---

## 3) TFT configuration instructions

All central settings are in `/home/runner/work/Sih/Sih/IDR_Vehicle_Display/config.h`.

### Controller setup
- `TFT_DRIVER_AUTO true` -> uses `tft.readID()` auto detection.
- If white/blank or wrong colors, set:
  - `TFT_DRIVER_AUTO false`
  - `TFT_DRIVER_MANUAL_ID 0x9486` (change to your detected controller)

### Resolution setup
- Width/height are detected from `tft.width()` and `tft.height()` after init.
- No repeated hard-coded resolution assumptions in logic.

### Touch setup
- `ENABLE_TOUCH false` by default.
- Set true only if your shield touch controller is working and calibrated.

---

## 4) Arduino UNO wiring guidance

### Typical UNO 3.5" shield
- Plug the shield directly on UNO headers.
- Control lines commonly mapped as in `config.h`:
  - `TFT_CS A3`
  - `TFT_CD A2`
  - `TFT_WR A1`
  - `TFT_RD A0`
  - `TFT_RST A4`

### External serial backend wiring (future Raspberry Pi)
- UNO UART is 5V TTL; Raspberry Pi UART is 3.3V.
- Use level shifting for Pi RX line.
- Shared ground required.

⚠ Do **not** directly drive TFT from ESP32 GPIO unless voltage compatibility is verified.

---

## 5) Serial protocol documentation

### Packet format
One line per frame:

`$IDR,SPEED=42.5,HEADING=127.2,GNSS=LOST,MODE=DR,CONF=87,LAT=52.4102,LON=-1.5603*`

### Supported fields
- `SPEED` float (km/h)
- `HEADING` float (degrees)
- `GNSS` one of: `AVAILABLE`, `LOST`, `RESTORED`, `NOFIX`
- `MODE` one of: `GNSS_INS`, `DR`, `RESTORING`, `DEMO`
- `CONF` integer 0..100
- `LAT` float
- `LON` float
- `ACC` float accuracy
- `IMU`, `AI`, `EKF`, `NHC`, `MAP` = `1`/`0` or `ACTIVE`
- `BATT`, `TEMP`, `RANGE` optional telemetry

### Command mode (serial text)
Send single-letter line commands:
- `N` -> Navigation page
- `T` -> Telemetry page
- `D` -> Diagnostics page
- `A` -> About page
- `S` -> Start demo
- `P` -> Pause/resume demo
- `R` -> Reset demo

---

## 6) How to run DEMO MODE

1. In `config.h`, keep `MOCK_DATA_MODE true`.
2. Upload sketch.
3. Dashboard automatically starts simulated route with visible label **DEMO / SIMULATED**.
4. Demo cycle:
   - GNSS AVAILABLE
   - GNSS SIGNAL LOST
   - DEAD RECKONING movement continues
   - GNSS RESTORED
   - RE-SYNCHRONIZING
   - back to normal
5. Control via serial monitor: `S`, `P`, `R`.

---

## 7) How Raspberry Pi will connect later

1. Set `MOCK_DATA_MODE false`.
2. Pi IDR engine sends `$IDR,...*` frames at ~10 Hz over UART.
3. Arduino parses packets in `serial_protocol.cpp` and updates `NavigationState`.
4. Arduino performs display only; no AI/EKF/NHC/map-matching computations.

---

## 8) Complete upload instructions

1. Connect UNO + shield via USB.
2. In Arduino IDE:
   - **Tools -> Board -> Arduino UNO**
   - **Tools -> Port -> your COM/tty port**
3. Open `IDR_Vehicle_Display.ino`.
4. Click **Verify** then **Upload**.
5. Open Serial Monitor at **115200** baud for commands and debug text.

---

## 9) Troubleshooting blank/white TFT

1. Confirm shield is fully seated on UNO headers.
2. Run with `TFT_DRIVER_AUTO true` first.
3. If still white screen, set manual controller ID in `config.h`.
4. Check rotation/color by adjusting `tft.setRotation(1)` in `display.cpp`.
5. Ensure `MCUFRIEND_kbv` and `Adafruit_GFX` are installed and up to date.
6. For touch-only issues, keep `ENABLE_TOUCH false` and use serial commands.

---

## 10) Memory/CPU optimization notes

Implemented for UNO constraints:
- Fixed-size `char` buffers for serial parser.
- No heavy bitmap assets.
- No full-screen redraw every loop.
- `millis()` scheduler at `UI_UPDATE_HZ` (default 10 Hz).
- Static page elements drawn once per page switch.
- Dynamic regions updated in-place (speed, heading, confidence, GNSS, marker).
- Avoids dynamic `String` allocations in core update path.

---

## Project files

- `IDR_Vehicle_Display.ino` - main loop and orchestration
- `config.h` - hardware/theme/runtime configuration
- `navigation.h/.cpp` - navigation state and mode logic
- `serial_protocol.h/.cpp` - serial frame parser and command handling
- `demo.h/.cpp` - GNSS outage simulation and mock route
- `display.h/.cpp` - TFT rendering and page UI
