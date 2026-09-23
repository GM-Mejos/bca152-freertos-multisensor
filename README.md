# BCA152 FreeRTOS Multisensor — Real-Time Room Monitoring System

A real-time, multitasking room-monitoring system built on the ESP32 using **FreeRTOS** (via ESP-IDF, no Arduino framework) and simulated in **Wokwi**. The system continuously monitors temperature, humidity, ambient light, and motion, displays live readings on an OLED screen navigable via a rotary encoder, and raises an acoustic buzzer alarm when temperature leaves a safe range.

Developed for **BCA152 Microcontrollers**, Mindanao State University – Iligan Institute of Technology.

---

## Features

- **Temperature & humidity monitoring** via single-bus protocol on a DHT22 sensor
- **Ambient light sensing** via a photoresistor (LDR) sampled through ADC1 Channel 6
- **Motion detection** via a digital PIR sensor, governing an automatic ACTIVE/INACTIVE power-saving state machine
- **OLED display** (SSD1306) cycling through Temperature, Humidity, Light, and Motion telemetry pages
- **Rotary encoder navigation** (KY-040) to manually cycle display pages via quadrature edge detection
- **Temperature alarm** — acoustic buzzer activates automatically outside the 18.0 °C–30.0 °C nominal range
- Built on **5 concurrent FreeRTOS tasks**, each with explicit priorities, communicating via dedicated queues, an event group, and a mutual exclusion semaphore
- **13 unit tests** (Unity framework) covering alarm boundaries, display navigation cycles, and state machine transitions — all passing
- Static code analysis performed and verified clean via Cppcheck

---

## Hardware & Simulated Wiring Pinout

| Component | Interface / Type | ESP32 Pin(s) | Description |
|---|---|---|---|
| **DHT22** | Single-Bus Digital | GPIO 4 | Ambient temperature & relative humidity |
| **Photoresistor (LDR)** | Analog Input | GPIO 34 (ADC1_CH6) | Ambient illuminance level |
| **PIR Motion Sensor** | Digital Input | GPIO 33 | Motion presence & wake trigger |
| **Rotary Encoder (KY-040)** | Quadrature Digital | CLK: GPIO 25, DT: GPIO 26, SW: GPIO 27 | Page navigation & display wake-up |
| **SSD1306 OLED** | I2C (400 kHz) | SDA: GPIO 21, SCL: GPIO 22 | Multi-page telemetry display |
| **Piezo Buzzer** | Digital Output / PWM | GPIO 14 | Acoustic threshold warning alarm |

---

## System Architecture

![System Architecture Diagram](docs/images/architecture-diagram.jpg)

---

## FreeRTOS Task Communication & IPC

![FreeRTOS Task Communication Diagram](docs/images/task-communication-diagram.jpg)

**Synchronization & IPC Primitives:**
- **`displayQueue`** — Mailbox queue (length 1, overwrite) transferring latest `SensorData` from `SensorTask` to `DisplayTask`.
- **`alarmQueue`** — FIFO queue passing telemetry to `AlarmTask`, ensuring no excursion readings are skipped.
- **`navQueue`** — Queue transferring `NavDirection` (NEXT / PREVIOUS) tokens from `InputTask` to `DisplayTask`.
- **`g_systemEvents`** — Event group broadcasting `EVENT_ACTIVE`, `EVENT_MOTION`, and `EVENT_ALARM` flags across tasks.
- **`serialMutex`** — Mutual exclusion semaphore protecting UART/`printf` operations from mid-line character interleaving.

---

## System State Machine

![System State Machine Diagram](docs/images/state-machine-diagram.jpg)

The system boots in the **ACTIVE** state. `MotionTask` monitors the PIR sensor; if no motion is detected for a continuous 15-second window, the system invokes `evaluateSystemState()` and transitions to **INACTIVE** (blanks OLED to conserve power). Any motion detected on the PIR sensor or interaction with the rotary encoder immediately returns the system to **ACTIVE**.

---

## FreeRTOS Task Summary

| Task | Priority | Period / Trigger | IPC Primitive | Blocked Condition | Primary Responsibility |
|---|---|---|---|---|---|
| **`MotionTask`** | 3 (High) | 100 ms periodic | Event Group | Delay | Monitor PIR, evaluate inactivity timeouts |
| **`InputTask`** | 3 (High) | 10 ms periodic | `navQueue` | Delay | Debounce encoder, dispatch navigation events |
| **`SensorTask`** | 2 (Medium) | 2 s (`vTaskDelayUntil`) | `displayQueue`, `alarmQueue` | Delay | Sample DHT22 + LDR, broadcast telemetry |
| **`AlarmTask`** | 2 (Medium) | Queue-Driven | `alarmQueue`, `g_systemEvents` | Waiting for Data | Block on queue, evaluate limits, drive buzzer |
| **`DisplayTask`** | 1 (Low) | 100 ms periodic | `displayQueue`, `navQueue` | Delay | Render current telemetry page to SSD1306 |

---

## Project Structure

```
bca152-freertos-multisensor/
├── include/
│   ├── alarm.h
│   ├── display.h
│   ├── input.h
│   ├── motion.h
│   ├── rtos_objects.h
│   ├── sensors.h
│   └── system_state.h
├── src/
│   ├── alarm.cpp
│   ├── display.cpp
│   ├── input.cpp
│   ├── main.cpp
│   ├── motion.cpp
│   ├── rtos_objects.cpp
│   ├── sensors.cpp
│   └── system_state.cpp
├── test/
│   └── test_logic/
│       └── test_logic.cpp
├── components/
│   └── ssd1306/
├── docs/
│   ├── laboratory-report.md
│   ├── laboratory-report.pdf
│   └── images/
│       ├── architecture-diagram.jpg
│       ├── task-communication-diagram.jpg
│       └── state-machine-diagram.jpg
├── diagram.json
├── wokwi.toml
└── platformio.ini
```

---

## Building, Testing, and Simulation

### Prerequisites
- [PlatformIO Core / IDE](https://platformio.org/)
- [Wokwi Simulator](https://wokwi.com/) (VS Code Extension)

### 1. Build Firmware
```bash
pio run
```

### 2. Execute Host-Native Unit Tests
```bash
pio test -e native
```
All 13 pure logic test cases compile and run via the Unity framework on the host x86 runner with zero hardware dependencies.

### 3. Run Wokwi Simulation
1. Open `diagram.json` in VS Code.
2. Press `F1`, select **`Wokwi: Start Simulator`**, and press Enter.
3. Turn the rotary encoder dial to cycle pages; drag the DHT22 temperature slider above 30 °C or below 18 °C to trigger the buzzer alarm.

---

## Verification Summary

- **Unit Tests:** 13/13 passing (temperature boundaries, navigation cyclic wraparound, state machine timeout transitions).
- **Static Analysis:** Executed using Cppcheck with zero defects across all source files.
- **Functional Tests:** FT-01 through FT-10 verified inside Wokwi simulation.
- **Fault Injections:** Verified watchdog trigger on unblocked tasks, rotary lag under priority demotion, and UART interleaving upon mutex removal.

---

## Author

**Guido Manfred G. Mejos**  
Department of Computer Applications — College of Computer Studies  
Mindanao State University – Iligan Institute of Technology