# BCA152 FreeRTOS Multisensor — Real-Time Room Monitoring System[cite: 8, 9]

A real-time, multitasking room-monitoring and alert system built from scratch on the dual-core **Espressif ESP32** microcontroller using native **FreeRTOS** under the **ESP-IDF framework** (no Arduino abstractions) and simulated in **Wokwi**[cite: 8, 9].

Developed for **BCA152 Microcontrollers**, Department of Computer Applications, College of Computer Studies, Mindanao State University – Iligan Institute of Technology[cite: 8, 9].

---

## Project Overview

Modern ambient environment monitoring systems require deterministic response times, thread-safe communication, and strict reliability[cite: 9]. This project implements a concurrent multitasking room supervisor on the ESP32[cite: 8, 9]. The system samples ambient environmental metrics (temperature, humidity, and ambient light), monitors human occupancy via PIR detection, manages an automated power-saving display state machine, evaluates thermal safety limits against an acoustic buzzer alarm, and renders multi-screen telemetry to an I2C SSD1306 OLED display navigated via a debounced rotary encoder[cite: 8, 9].

---

## Features

- **Temperature & Humidity Monitoring:** High-precision periodic temperature and relative humidity acquisition via single-bus protocol on a DHT22 sensor[cite: 8, 9].
- **Ambient Light Sensing:** Raw analog illuminance tracking using ESP32 SAR ADC1 Channel 6, normalized to a percentage scale (0–100%)[cite: 8, 9].
- **Motion Detection & Power Management:** Digital PIR motion sensing governing an automatic ACTIVE/INACTIVE power-saving state machine that blanks the OLED after 15 seconds of inactivity[cite: 8, 9].
- **Interactive Multi-Page OLED Display:** SSD1306 128x64 display cycling across Temperature, Humidity, Light, and Motion telemetry pages[cite: 8, 9].
- **Rotary Encoder Navigation:** KY-040 mechanical encoder to manually cycle display pages via quadrature edge detection with bidirectional wraparound[cite: 8, 9].
- **Dual-Threshold Acoustic Alarm:** Active piezo buzzer activates automatically when ambient temperature leaves the nominal 18.0 °C to 30.0 °C safe operating range[cite: 8, 9].
- **Deterministic FreeRTOS Concurrency:** 5 dedicated tasks executing with explicit priorities, rate-monotonic timing (`vTaskDelayUntil`), inter-task queues, an event group, and a mutual exclusion semaphore[cite: 8, 9].
- **Decoupled Logic & Host-Native Unit Testing:** Pure algorithmic decision logic separated from silicon registers and verified using the Unity framework (13/13 passing tests)[cite: 8, 9].
- **Static Code Analysis:** Clean codebase verified with zero defects, zero warnings, and zero memory leaks using Cppcheck[cite: 8, 9].

---

## Learning Objectives

- Construct an ESP32 firmware project within PlatformIO using the native ESP-IDF framework without Arduino dependencies[cite: 9].
- Construct and simulate a multi-peripheral embedded circuit inside Wokwi[cite: 9].
- Interface digital single-bus, analog SAR ADC, and I2C peripherals with an ESP32[cite: 9].
- Structure modular embedded firmware across decoupled header and source files[cite: 9].
- Create and manage concurrent FreeRTOS tasks with justified priority assignments[cite: 9].
- Distinguish Running, Ready, and Blocked task states under preemptive scheduling[cite: 9].
- Implement thread-safe inter-task communication using FreeRTOS queues[cite: 9].
- Protect shared resources against race conditions and stdout data interleaving using a mutex[cite: 9].
- Coordinate multi-task system states using a FreeRTOS event group[cite: 9].
- Enforce periodic execution and eliminate cumulative timing drift using `vTaskDelayUntil()`[cite: 9].
- Decouple hardware-independent logic to execute automated host-native unit tests (`pio test -e native`)[cite: 9].
- Perform static code analysis using PlatformIO (`pio check`) to identify defect patterns[cite: 9].

---

## System Architecture

![System Architecture Diagram](docs/images/architecture-diagram.png)

The application architecture is decomposed into four modular layers[cite: 8, 9]:
1. **Sensory Ingestion Subsystem:** Low-level edge-timed and ADC sampling drivers interfacing the DHT22, LDR, and PIR sensor[cite: 8, 9].
2. **Core RTOS Processing Subsystem:** Preemptive FreeRTOS kernel managing queues, event groups, and mutual exclusion locks[cite: 8, 9].
3. **Supervisory State Subsystem:** Pure state evaluation functions handling dual-threshold thermal safety and 15-second inactivity timeout transitions[cite: 8, 9].
4. **Human-Machine Interface (HMI) Subsystem:** Quadrature rotary state decoder and SSD1306 I2C frame buffer rendering pipeline[cite: 8, 9].

---

## FreeRTOS Architecture

![FreeRTOS Task Communication Diagram](docs/images/task-communication-diagram.jpg)

The system prevents shared-memory race conditions by passing data strictly through thread-safe FreeRTOS primitives[cite: 8, 9]:

```
+---------------+        displayQueue (Overwrite)        +---------------+
|  SensorTask   |--------------------------------------->|  DisplayTask  |---> SSD1306 OLED
| (Priority 2)  |----+                                   | (Priority 1)  |
+---------------+    |   alarmQueue (FIFO Depth 5)       +---------------+
                     +---------------------------------->|   AlarmTask   |---> Piezo Buzzer
                                                         | (Priority 2)  |
+---------------+         navQueue (FIFO Depth 5)        +---------------+
|   InputTask   |----------------------------------------------^
| (Priority 3)  |
+---------------+
        |
        | Bit set/clear
        v
+------------------------------------------------------------------------+
|                      Event Group: g_systemEvents                       |
|   EVENT_ACTIVE (Bit 0)  |  EVENT_MOTION (Bit 1)  |  EVENT_ALARM (Bit 2)|
+------------------------------------------------------------------------+
        ^                                                      ^
        |                                                      |
+---------------+                                              |
|  MotionTask   |----------------------------------------------+
| (Priority 3)  |
+---------------+
```

---

## Hardware / Simulated Components

![Wokwi Circuit Layout](docs/images/circuit-diagram.png)

| Component | Hardware Model | Function in System |
| :--- | :--- | :--- |
| **ESP32 Microcontroller** | ESP32-WROOM-32 DevKit | Dual-core processing unit running the FreeRTOS kernel and application tasks[cite: 9]. |
| **Temperature & Humidity Sensor** | DHT22 (AM2302) | Measures ambient temperature and relative humidity over a single-bus digital line[cite: 8, 9]. |
| **Photoresistor** | LDR Analog Module | Captures ambient illuminance level via an analog voltage divider[cite: 8, 9]. |
| **PIR Motion Sensor** | HC-SR501 Digital Module | Senses room occupancy and triggers power state wake-up[cite: 8, 9]. |
| **Rotary Encoder** | KY-040 Mechanical Encoder | Provides manual clockwise and counter-clockwise display page navigation[cite: 8, 9]. |
| **OLED Display** | SSD1306 (128x64 Monochrome) | Renders telemetry screens and system alert notifications over I2C[cite: 8, 9]. |
| **Piezo Buzzer** | Active Piezo Transducer | Emits an acoustic alert tone when temperature limits are breached[cite: 8, 9]. |

---

## Pin Configuration

| Component Pin | ESP32 GPIO Pin | Electrical Interface | Configuration Details |
| :--- | :--- | :--- | :--- |
| **DHT22 Data (SDA)** | `GPIO 4` | Bidirectional Digital | Open-drain mode with internal pull-up[cite: 8, 9]. |
| **LDR Analog Out (AO)** | `GPIO 34` | Analog Input | ADC1 Channel 6 (Input-only pin, 0–4095 raw count)[cite: 8, 9]. |
| **PIR Sensor Out** | `GPIO 33` | Digital Input | Active-high digital presence signal[cite: 8, 9]. |
| **Rotary CLK (Phase A)** | `GPIO 18` | Digital Input | Internal pull-up enabled for quadrature decoding[cite: 9]. |
| **Rotary DT (Phase B)** | `GPIO 19` | Digital Input | Internal pull-up enabled for quadrature decoding[cite: 9]. |
| **Rotary SW (Button)** | `GPIO 5` | Digital Input | Internal pull-up enabled (momentary active-low)[cite: 9]. |
| **OLED SDA** | `GPIO 21` | I2C Data | 400 kHz Fast-Mode I2C Bus[cite: 8, 9]. |
| **OLED SCL** | `GPIO 22` | I2C Clock | 400 kHz Fast-Mode I2C Bus[cite: 8, 9]. |
| **Buzzer (+)** | `GPIO 14` | Digital Output | Push-pull digital drive for acoustic alerting[cite: 8, 9]. |

---

## Task Design

| Task Name | Priority | Period / Trigger | IPC Mechanism | Blocked Condition | Primary Responsibility |
| :--- | :---: | :--- | :--- | :--- | :--- |
| **`MotionTask`** | 3 (High) | 100 ms periodic | `g_systemEvents` | Delay (`vTaskDelay`) | Samples PIR sensor, tracks inactivity timeout counter, updates state bits[cite: 8, 9]. |
| **`InputTask`** | 3 (High) | 10 ms periodic | `navQueue`, `g_systemEvents` | Delay (`vTaskDelay`) | Debounces encoder pins, decodes quadrature rotation, dispatches navigation tokens[cite: 8, 9]. |
| **`SensorTask`** | 2 (Med) | 2000 ms (`vTaskDelayUntil`) | `displayQueue`, `alarmQueue`, `serialMutex` | Delay Until Next Period | Samples DHT22 and LDR ADC, packs telemetry struct, broadcasts data packets[cite: 8, 9]. |
| **`AlarmTask`** | 2 (Med) | Event-Driven | `alarmQueue`, `g_systemEvents`, `serialMutex` | Queue receive (`portMAX_DELAY`) | Evaluates thermal limits, asserts alarm flags, drives buzzer GPIO[cite: 8, 9]. |
| **`DisplayTask`** | 1 (Low) | 100 ms periodic | `displayQueue`, `navQueue`, `g_systemEvents` | Delay (`vTaskDelay`) | Consumes telemetry, tracks page index, renders SSD1306 buffer over I2C[cite: 8, 9]. |

### Priority Justification
- **Priority 3 (`MotionTask`, `InputTask`):** High scheduling urgency[cite: 9]. Mechanical rotary encoder detents last only 10–20 ms; dropping priority risks missing edge transitions and causing sluggish UI response[cite: 9]. PIR occupancy changes must immediately wake the system[cite: 9].
- **Priority 2 (`SensorTask`, `AlarmTask`):** Medium urgency[cite: 9]. Environmental temperature and humidity change gradually, but safety evaluation must take precedence over display drawing to ensure alarms are triggered without delay[cite: 9].
- **Priority 1 (`DisplayTask`):** Low urgency[cite: 9]. Refreshing the 1024-byte OLED buffer across the 400 kHz I2C bus takes notable execution time and can tolerate brief scheduling delays without impacting safety or sensor acquisition[cite: 9].

---

## Inter-Task Communication

- **`displayQueue` (Length: 1, Mailbox):** Holds the latest `SensorData` snapshot using `xQueueOverwrite()` so `DisplayTask` renders fresh sensor data without queue backlog[cite: 8, 9].
- **`alarmQueue` (Length: 5, FIFO):** Queues every temperature packet so `AlarmTask` processes every sample without skipping transient thermal spikes[cite: 8, 9].
- **`navQueue` (Length: 5, FIFO):** Buffers navigation direction tokens (`NavDirection::NEXT` / `NavDirection::PREVIOUS`) from `InputTask` to `DisplayTask`[cite: 8, 9].
- **`g_systemEvents` (Event Group):** Manages atomic bit flags across asynchronous tasks[cite: 8, 9]:
  - `EVENT_ACTIVE` (`BIT0`): System power status ($1 =$ Active, $0 =$ Inactive)[cite: 9].
  - `EVENT_MOTION` (`BIT1`): Dynamic occupancy status from the PIR motion sensor[cite: 9].
  - `EVENT_ALARM` (`BIT2`): High/Low temperature alarm trigger status[cite: 9].
- **`serialMutex` (Mutex):** Enforces mutual exclusion on the UART standard output driver (`printf`) across concurrent tasks to eliminate interleaved console output[cite: 8, 9].

---

## State Machine

![System State Machine Diagram](docs/images/state-machine-diagram.png)

```
                     +---------------------------------------+
                     |             ACTIVE STATE              |
                     | - OLED screen active and rendering    |
                     | - Periodic sensor monitoring running  |
                     | - Inactivity counter increments       |
                     +-------------------+-------------------+
                                         |
                       No motion for     |  Motion detected (PIR)
                       >= 15 seconds     |  OR Rotary encoder turned
                                         |
                                         v
                     +-------------------+-------------------+
                     |            INACTIVE STATE             |
                     | - OLED blanked / display disabled     |
                     | - Sensor & Alarm tasks run in bg      |
                     | - Low power consumption footprint     |
                     +---------------------------------------+
```

- **ACTIVE State:** The OLED display actively renders cyclic telemetry screens[cite: 8, 9]. `MotionTask` monitors the PIR sensor every 100 ms and resets the inactivity timer upon detection[cite: 8, 9]. If no motion is detected for 15 consecutive seconds (15,000 ms), the system invokes `evaluateSystemState()`, clears `EVENT_ACTIVE`, and transitions to `INACTIVE`[cite: 8, 9].
- **INACTIVE State:** `DisplayTask` detects the loss of `EVENT_ACTIVE`, blanks the screen buffer, and shuts off OLED power consumption[cite: 8, 9]. `AlarmTask` continues monitoring thermal safety in the background[cite: 8, 9]. Any PIR trigger or rotary movement immediately restores the system to `ACTIVE`[cite: 8, 9].

---

## Repository Structure

```
bca152-freertos-multisensor/
├── include/
│   ├── alarm.h              # Pure temperature evaluation & threshold types
│   ├── display.h            # Display mode enums & SSD1306 rendering interface
│   ├── input.h              # Rotary encoder quadrature debouncing types
│   ├── motion.h             # PIR motion acquisition & timer interfaces
│   ├── rtos_objects.h       # Shared FreeRTOS queue, event group, and mutex handles
│   ├── sensors.h            # DHT22 & LDR sensor payload structs
│   └── system_state.h       # ACTIVE/INACTIVE state machine logic
├── src/
│   ├── CMakeLists.txt       # ESP-IDF component build registration
│   ├── alarm.cpp            # AlarmTask implementation & buzzer control
│   ├── display.cpp          # DisplayTask implementation & frame formatting
│   ├── input.cpp            # InputTask implementation & pin sampling
│   ├── main.cpp             # app_main(): Peripheral init, IPC init, task spawns
│   ├── motion.cpp           # MotionTask implementation & timeout evaluation
│   ├── rtos_objects.cpp     # Allocation of FreeRTOS kernel primitives
│   ├── sensors.cpp          # SensorTask implementation (DHT22 & ADC read)
│   └── system_state.cpp     # State transition evaluation implementation
├── test/
│   └── test_logic/
│       └── test_logic.cpp   # Host-native Unity unit test suite (13 test cases)
├── docs/
│   ├── laboratory-report.pdf# Formal academic report
│   ├── laboratory-report.md # Markdown source of formal academic report
│   └── images/              # Technical architecture and schematic diagrams
│       ├── architecture-diagram.png
│       ├── circuit-diagram.png
│       ├── simulation-running.png
│       ├── state-machine-diagram.png
│       └── task-communication-diagram.jpg
├── diagram.json             # Wokwi simulation circuit layout & pin wiring
├── wokwi.toml               # Wokwi simulator runtime configuration
└── platformio.ini           # PlatformIO dual-environment configuration (esp32dev & native)
```

---

## Getting Started

### Prerequisites
- [PlatformIO Core / IDE](https://platformio.org/) installed inside Visual Studio Code[cite: 8, 9].
- [Wokwi Simulator Extension](https://wokwi.com/) for Visual Studio Code[cite: 8, 9].
- [Git](https://git-scm.com/) installed on your development workstation[cite: 9].

### Cloning the Project
```bash
git clone [https://github.com/GM-Mejos/bca152-freertos-multisensor.git](https://github.com/GM-Mejos/bca152-freertos-multisensor.git)
cd bca152-freertos-multisensor
```

---

## Building the Project

Compile the firmware using the PlatformIO command-line interface:

```powershell
pio run -e esp32dev
```

A clean build links the FreeRTOS kernel and ESP-IDF drivers, concluding with:
```text
Successfully created esp32 image.
[SUCCESS] Took ~8.5 seconds
```

---

## Running the Wokwi Simulation

![Finished Running System Screenshot](docs/images/simulation-running.png)

1. Open the project root folder in VS Code[cite: 8, 9].
2. Click on `diagram.json` to focus the circuit layout[cite: 8, 9].
3. Press `F1` (or `Ctrl + Shift + P`), type:
   ```text
   Wokwi: Start Simulator
   ```
   and press **Enter**[cite: 8, 9].
4. Switch to the **Wokwi Terminal** tab at the bottom right to inspect boot logs[cite: 9].
5. Click and drag the **KY-040 rotary knob** to cycle between display screens[cite: 8, 9].
6. Click the **DHT22 sensor** to adjust temperature above 30.0 °C or below 18.0 °C to trigger the buzzer alarm[cite: 8, 9].
7. Click the **PIR sensor** to simulate motion, or allow 15 seconds to elapse to observe the power-save sleep transition[cite: 8, 9].

---

## Unit Testing

Deterministic, hardware-independent decision logic is isolated from low-level register drivers (`#ifndef UNIT_TESTING`) and executed natively on the host workstation under PlatformIO's Unity test framework:

```powershell
pio test -e native
```

### Test Suite Coverage (13 Tests Passing)
- **Temperature Alarm Logic (5 tests):**
  - Temperature $< 18.0^\circ\text{C}$ $\rightarrow$ `LOW_TEMPERATURE` (Alarm active)[cite: 8, 9].
  - Temperature $== 18.0^\circ\text{C}$ $\rightarrow$ `NORMAL` (Alarm inactive)[cite: 8, 9].
  - Temperature $== 24.0^\circ\text{C}$ $\rightarrow$ `NORMAL` (Alarm inactive)[cite: 8, 9].
  - Temperature $== 30.0^\circ\text{C}$ $\rightarrow$ `NORMAL` (Alarm inactive)[cite: 8, 9].
  - Temperature $> 30.0^\circ\text{C}$ $\rightarrow$ `HIGH_TEMPERATURE` (Alarm active)[cite: 8, 9].
- **Display Mode Navigation (4 tests):**
  - Clockwise traversal: `TEMP -> HUM -> LIGHT -> MOTION -> TEMP`[cite: 8, 9].
  - Counter-clockwise traversal: `TEMP -> MOTION -> LIGHT -> HUM -> TEMP`[cite: 8, 9].
- **System State Machine (4 tests):**
  - `ACTIVE` state with continuous motion $\rightarrow$ remains `ACTIVE`[cite: 8, 9].
  - `ACTIVE` state without motion for $< 15$ s $\rightarrow$ remains `ACTIVE`[cite: 8, 9].
  - `ACTIVE` state without motion for $\ge 15$ s $\rightarrow$ transitions to `INACTIVE`[cite: 8, 9].
  - `INACTIVE` state with motion detected $\rightarrow$ immediately restores `ACTIVE`[cite: 8, 9].

---

## Static Code Analysis

Static analysis was executed using Cppcheck across the entire source and header tree:

```bash
pio check
```

**Results:** Zero errors, zero warnings, zero uninitialized variables, and zero memory leaks[cite: 8, 9].

| Finding | File / Line | Cause | Resolution |
| :--- | :--- | :--- | :--- |
| *No defects found* | `src/*`, `include/*` | Clean codebase adhering to strict typing and bounds checking | Passed with zero issues reported[cite: 8, 9]. |

---

## Functional Verification

System integration verified in the Wokwi simulation across ten functional test procedures:

| Test ID | Stimulus / Action | Expected Result | Actual Result | Status |
| :---: | :--- | :--- | :--- | :---: |
| **FT-01** | Adjust DHT22 temperature slider | Displayed temperature updates[cite: 9] | Temperature updates dynamically | **PASS**[cite: 8, 9] |
| **FT-02** | Adjust DHT22 humidity slider | Displayed humidity updates[cite: 9] | Humidity updates dynamically | **PASS**[cite: 8, 9] |
| **FT-03** | Adjust LDR lux level | Ambient light percentage updates[cite: 9] | Light level updates dynamically | **PASS**[cite: 8, 9] |
| **FT-04** | Rotate encoder clockwise | Next telemetry screen selected[cite: 9] | Advances cleanly with wraparound | **PASS**[cite: 8, 9] |
| **FT-05** | Rotate encoder counter-clockwise | Previous telemetry screen selected[cite: 9] | Reverses cleanly with wraparound | **PASS**[cite: 8, 9] |
| **FT-06** | Increase temperature $> 30.0^\circ\text{C}$ | Alarm activates and buzzer sounds[cite: 9] | Buzzer sounds; `EVENT_ALARM` set | **PASS**[cite: 8, 9] |
| **FT-07** | Return temperature to $24.0^\circ\text{C}$ | Alarm deactivates and buzzer silences[cite: 9] | Buzzer silences; `EVENT_ALARM` cleared | **PASS**[cite: 8, 9] |
| **FT-08** | Trigger PIR motion sensor | System is ACTIVE and OLED refreshes[cite: 9] | System enters ACTIVE state immediately | **PASS**[cite: 8, 9] |
| **FT-09** | Leave system idle for 15 seconds | System enters INACTIVE; OLED blanks[cite: 9] | Display powers down after timeout | **PASS**[cite: 8, 9] |
| **FT-10** | Trigger PIR sensor while INACTIVE | System returns to ACTIVE; OLED restores[cite: 9] | Display wakes up immediately | **PASS**[cite: 8, 9] |

---

## Engineering Decisions

1. **Native ESP-IDF FreeRTOS over Arduino Framework:** The Arduino framework introduces background task abstraction layers that obscure tick-level RTOS dynamics[cite: 9]. ESP-IDF provides direct control over task priorities, queue sizing, core pinning, and kernel tick rates[cite: 9].
2. **`vTaskDelayUntil()` for Sensor Ingestion:** Standard `vTaskDelay()` measures sleep time relative to when the delay function is invoked, producing cumulative timing drift as sensor execution duration fluctuates[cite: 9]. `vTaskDelayUntil()` tracks an absolute reference tick count, guaranteeing an exact 2000 ms execution cadence[cite: 9].
3. **Queue Mailbox Pattern for Display Refresh:** The SSD1306 display only needs the most recent sensor values[cite: 8, 9]. Using `xQueueOverwrite()` prevents backlog buildup and ensures the low-priority display task never renders stale sensor readings[cite: 8, 9].
4. **Decoupled Architecture for Host-Native Testing:** Hardware register headers are conditionally excluded (`#ifndef UNIT_TESTING`), enabling unit tests to compile directly on host x86 runners via Unity without flashing physical chips[cite: 8, 9].
5. **Mutex Protection on Standard Output:** FreeRTOS tasks execute preemptively across both ESP32 cores[cite: 9]. Wrapping UART `printf` calls in `serialMutex` prevents character scrambling caused by concurrent writes[cite: 8, 9].

---

## Limitations

- **Simulated Environmental Dynamics:** Wokwi provides ideal digital sensor signals[cite: 9]; physical DHT22 sensors introduce analog rise-time constraints, bus capacitance sensitivity, and thermal self-heating[cite: 9].
- **Single I2C Bus Contention:** Display rendering occupies the shared I2C bus for multi-byte buffer transfers, requiring careful task priority management to avoid starving other I2C peripherals[cite: 9].
- **Hardcoded Safety Thresholds:** Thermal warning limits (18.0 °C and 30.0 °C) and the 15-second inactivity timeout are compile-time constants rather than runtime-configurable values stored in Non-Volatile Storage (NVS)[cite: 9].

---

## Future Improvements

- **Non-Volatile Storage (NVS) Integration:** Persist alarm setpoints and timeout intervals in flash memory, editable directly on-screen via rotary encoder push-button long-presses[cite: 9].
- **Deep Sleep Power Optimization:** Place the ESP32 into deep sleep during INACTIVE states, leveraging the ULP co-processor or PIR external interrupt pin (`esp_sleep_enable_ext0_wakeup`) to wake the CPU[cite: 9].
- **Cloud Telemetry via MQTT/HTTP:** Stream periodic environmental metrics over Wi-Fi to a cloud dashboard or Home Assistant broker using the ESP-IDF networking stack[cite: 9].

---

## References and Acknowledgments

- [Espressif ESP-IDF Programming Guide — FreeRTOS (ESP-IDF)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html)[cite: 9]
- [FreeRTOS Official Documentation — Kernel API Reference](https://www.freertos.org/a00106.html)[cite: 9]
- [Wokwi ESP32 Simulator Documentation](https://docs.wokwi.com/)[cite: 9]
- Course syllabus, architectural guidance, and laboratory requirements provided by **Prof. Paul Rodolf P. Castor**, Department of Computer Applications, MSU-IIT[cite: 8, 9].