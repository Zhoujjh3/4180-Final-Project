# GT Dealer — Automated Card Shuffler & Dealer

**ECE 4180 Final Project | Georgia Institute of Technology**
<img width="1440" height="1920" alt="image" src="https://github.com/user-attachments/assets/c8426ec3-7bfe-48be-b7b4-8cbfc400149a" />

---

## Project Overview

GT Dealer is a fully automated card shuffling and dealing machine controlled wirelessly via Bluetooth Low Energy (BLE). The system uses two ESP32-C6 microcontrollers — one acting as a BLE GATT client (the UI/controller node) and one as a BLE GATT server (the motor driver node) — to physically shuffle and deal a standard 52-card deck to up to 8 players. Users configure game settings through a 3-button menu on a TFT LCD display, then trigger automated shuffle and deal sequences wirelessly.

---

## Mechanical Design

The physical housing was fully custom-designed and 3D printed using Fusion 360. The enclosure was designed iteratively to achieve the exact internal dimensions needed to hold a standard playing card deck snugly while accommodating the three DC motors and their rubber O-ring rollers. The modular design consists of two main assemblies: a **shuffle module** (the outer clamshell) and a **deal module** (the lower ejection stage).

The shuffle module uses two counter-rotating DC motors with rubber O-ring rollers pressing against opposite faces of the deck, rapidly alternating card positions to randomize the deck. The deal module uses a third DC motor with a friction roller contacting the bottom card of the deck, ejecting one card at a time through a slot at the base. The enclosure tolerances were tuned through multiple print iterations to ensure consistent card contact pressure without jamming.

---

## Hardware Components

| Component | Quantity | Purpose |
|---|---|---|
| ESP32-C6 DevKitC-1 | 2 | Controller MCU (UI) + Motor MCU (BLE server) |
| TB6612FNG H-Bridge Motor Driver | 2 | Drive 3 DC motors with bidirectional PWM control |
| DC Brushed Motors (N20) | 3 | 2× shuffle rollers, 1× deal ejector |
| ST7735 1.8" TFT LCD (128×160) | 1 | SPI display for menu UI |
| Tactile Push Buttons | 3 | Up / Down / Select navigation |
| AA Battery Pack (4×AA, 6V) | 1 | Motor power supply |
| USB (5V) | 1 | Controller ESP32 power |

### Component Roles

**ESP32-C6 (Controller Node):** Hosts the user interface. Reads 3 GPIO buttons with software debouncing, drives the ST7735 LCD over SPI using the Adafruit GFX library, manages a screen/menu state machine, and transmits 1-byte BLE command packets to the motor node via NimBLE GATT.

**ESP32-C6 (Motor Node):** Acts as a BLE GATT peripheral/server. Receives command bytes from the controller and drives the three DC motors accordingly via PWM on the TB6612 H-bridge drivers. Runs FreeRTOS with tickless idle and dynamic frequency scaling (160→10 MHz) to reduce power draw between BLE events. On BLE disconnect, immediately cuts all motor PWM to prevent runaway hardware state.

**TB6612FNG H-Bridges (×2):** Provide bidirectional PWM motor control with built-in current limiting and thermal protection. One driver handles both shuffle motors; the other handles the deal motor. 8-bit PWM resolution allows fine-tuned speed control per operation.

**ST7735 TFT LCD:** Connected via SPI, rendering a navigable menu with options for Shuffle, Deal, Manual Deal, Set Player Count, and Set Card Amount. Display state is tightly coupled to the firmware's screen state machine.

**Buttons (×3):** Up, Down, and Select. Debounced in software. Drive the menu state machine to navigate options and trigger operations.

**Battery Pack (4×AA):** Provides isolated 6V supply to the motor H-bridges, keeping high-current inductive loads off the ESP32 supply rail and preventing brown-outs.

---

## Firmware Architecture

The firmware is organized as a single PlatformIO project with two build environments (`controller` and `motor`) sharing a common source tree, with `build_src_filter` selectively compiling each target. This eliminates code duplication and keeps shared definitions (BLE UUIDs, command bytes) in a single header.

Key firmware features:
- **Non-blocking FSM** for deal sequencing — timed card dispensing and inter-card pauses without `delay()`
- **Auto-shuffle-to-deal pipeline** — after a 24-second shuffle, the system automatically transitions to the deal sequence
- **NVS persistence** — player count and card count saved to ESP32 Non-Volatile Storage flash, retained across power cycles
- **Self-healing BLE** — automatic re-scan and reconnect on link loss, with guaranteed motor safe-stop on disconnect

---

## Comparison to Real-World Embedded Systems

GT Dealer shares architectural patterns with commercial embedded products. The dual-MCU BLE master-slave topology mirrors designs used in wireless consumer electronics (e.g., BLE remote + actuator node in smart home devices) where UI logic is intentionally isolated from high-current hardware to ensure stability. The non-blocking FSM deal sequencer is analogous to motor sequencing firmware in industrial dispensing machines. The use of NVS for persistent configuration mirrors techniques in IoT field devices that must retain calibration across power cycles.

Unlike commercial card dealers (e.g., casino shufflers), GT Dealer does not use optical card sensing or closed-loop feedback — card dispensing is purely time-based, which limits single-card ejection precision.

---

## Problems Encountered

**Card compression inconsistency:** The most significant hardware challenge was ensuring that only one card was ejected per deal cycle. Without a mechanism to maintain consistent downward pressure on the deck as cards were removed, the friction roller would sometimes grab multiple cards or slip entirely. A compliant spring-loaded pressure plate above the deck would resolve this.

**Motor torque:** The N20 DC motors provided adequate speed but marginal torque, particularly for shuffle operations with a full 52-card deck. Cards occasionally stalled the rollers under high friction. Higher-torque motors (or gear-reduced variants) would significantly improve reliability.

**BLE timing sensitivity:** Tuning the deal dispense pulse width to reliably eject exactly one card required iterative calibration and was sensitive to battery voltage level as the AA cells discharged.

---

## Future Improvements

- **Spring-loaded card compression plate** — mechanically maintain consistent deck pressure to ensure single-card ejection accuracy
- **Higher-torque motors** — replace N20 motors with gear-reduced variants to handle full deck loads without stalling
- **Optical card sensor** — add an IR break-beam sensor at the ejection slot for closed-loop card counting, replacing time-based dispensing
- **Rechargeable LiPo battery + charging circuit** — replace AA pack for a cleaner, self-contained form factor
- **PCB design** — replace breadboard prototypes with custom PCBs to reduce wiring complexity and improve reliability
