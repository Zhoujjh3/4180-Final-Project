# GT Dealer — Automated Card Shuffler & Dealer

**ECE 4180 Final Project | Georgia Institute of Technology**

<img width="1696" height="400" alt="project_photos_horizontal" src="https://github.com/user-attachments/assets/f4430ad4-8368-4f0c-8b97-17622b88a011" />
[![Demo Video](https://img.youtube.com/vi/HnhNEBLaP4c/hqdefault.jpg)](https://www.youtube.com/watch?v=HnhNEBLaP4c)

---

## Project Overview

GT Dealer is an automated card shuffling and dealing machine controlled wirelessly via Bluetooth Low Energy (BLE). The system uses two ESP32-C6 microcontrollers - one acting as a BLE client (the UI/remote controller) and one as a BLE server (the motor driver) — to  shuffle and deal a standard 52-card deck to up to 8 players. Users can configure game settings through a 3-button menu on a LCD display, then begin automated shuffle and deal sequences wirelessly.

---

## Mechanical Design

The physical housing was fully custom-designed on OnShape and 3D printed at the Hive. The enclosure was designed iteratively to find the best angle to position the deck, find the exact gap to allow for one card to be funneled out at a time, and best roller dimensions to catch each card. The modular design consists of two main assemblies: a shuffle module (the top component) and a deal module (the lower compartment). The designs for these two assemblies are fundamentally the same - they use identical rollers, ramp angles, and card blockers. The main difference is the RPM and torque of their respective motors.

The shuffle module uses two counter-rotating DC motors with rubberband rollers pressing against opposite faces of the deck, alternating card positions to randomize the deck. Because of the randomness introduced by the rubber grippers, the cards naturally do not shuffle in a perfect riffle, which allows for a better randomized deck. The deal module uses a third DC motor with another rubber roller contacting the bottom card of the deck, ejecting one card at a time through a slot at the base.

---

## Hardware Components

| Component | Quantity | Purpose |
|---|---|---|
| ESP32-C6 DevKitC-1 | 2 | Controller MCU (UI) + Motor MCU (BLE server) |
| TB6612FNG H-Bridge Motor Driver | 2 | Drive 3 DC motors with bidirectional PWM control |
| DC Brushed Motors (N20) | 3 | 2× shuffle rollers, 1× deal ejector |
| ST7735 1.8" LCD (128×160) | 1 | SPI display for menu UI |
| Tactile Push Buttons | 3 | Up / Down / Select navigation |
| AA Battery Pack (4×AA, 6V) | 1 | Motor power supply |
| USB (5V) | 1 | Controller ESP32 power |

### Component Roles

**ESP32-C6 (Controller Node):** Hosts the user interface. Reads 3 GPIO buttons with software debouncing, drives the ST7735 LCD over SPI using the Adafruit GFX library, manages a screen/menu state machine, and transmits 1-byte BLE command packets to the motor node via NimBLE.

**ESP32-C6 (Motor Node):** Acts as a BLE peripheral/server. Receives command bytes from the controller and drives the three DC motors accordingly via PWM on the TB6612 H-bridge drivers. Runs FreeRTOS with tickless idle and dynamic frequency scaling (160→10 MHz) to reduce power draw between BLE events. On BLE disconnect, immediately cuts all motor PWM to prevent runaway hardware state.

**TB6612FNG H-Bridges (×2):** Provide bidirectional PWM motor control. One driver handles both shuffle motors; the other handles the deal motor. 8-bit PWM resolution allows fine-tuned speed control per operation.

**ST7735 LCD:** Connected via SPI to display a navigable menu with options for Shuffle, Deal, Manual Deal, Set Player Count, and Set Card Amount. The screen's state is managed by our software's screen state machine.

**Buttons (×3):** Up, Down, and Select. Debounced in software. Drive the menu state machine to navigate options and trigger operations.

**Battery Pack (4×AA):** Provides a 6V supply to the motor H-bridges. The 6V is connected to a separate breadboard rail, while the GND rail is shared with the ESP32.

---

## Firmware Architecture

The firmware is organized as a single PlatformIO project with two build environments (`controller` and `motor`) sharing a common source tree, with `build_src_filter` selectively compiling each target. This eliminates code duplication and keeps shared definitions (BLE UUIDs, command bytes) in a single header.

Key firmware features:
- **Non-blocking FSM** for deal sequencing — timed card dispensing and inter-card pauses without `delay()`
- **Auto-shuffle-to-deal sequence** — after a 24-second shuffle, the system automatically transitions to the deal sequence
- **NVS persistence** — player count and card count saved to ESP32 Non-Volatile Storage flash, retained across power cycles
- **Self-healing BLE** — automatic re-scan and reconnect on link loss, with guaranteed motor safe-stop on disconnect

---

## Comparison to Real-World Embedded Systems

GT Dealer shares a similar architecture with commercial embedded products. The use of dual-MCU BLE master-slave mirrors designs used in wireless consumer electronics (e.g., BLE remote + actuator node in smart home devices). The non-blocking FSM deal sequencer is analogous to motor sequencing firmware in industrial dispensing machines. The use of non-volatile storage for persistent configuration mirrors techniques in IoT field devices that must retain calibration across power cycles.

Unlike commercial card dealers, GT Dealer does not use card sensing or closed-loop feedback — card dispensing is purely time-based, which makes single-card ejection very difficult.

---

## Problems Encountered

**Card dealing inconsistency:** The most significant hardware challenge was ensuring that only one card was ejected per deal cycle. This was difficult for two reasons:
1. Since we were not using rubber O-rings but instead a variety of differently sized rubber bands wrapped around the roller, the roller did not have a uniform circumference. Thus, even though we fine-tuned the card gap in CAD to only let one card out at a time, this would sometimes result in jamming. To solve this, we widened the gap, but this would sometimes allow for multiple cards to slip out.
2. Without a mechanism to maintain consistent downward pressure on the deck as cards were removed, the friction roller would sometimes grab multiple cards or slip entirely. Using a weight or a spring-loaded pressure plate above the deck could potentially resolve this.

**Motor torque:** We originally planned for the shuffler motors to have high torque and low RPM, and the dealer motor to have low torque and high RPM. In hindsight, it should have been the other way around. Because of the low torque, it was nearly impossible for our dealer motor to deal out one card at a time without turning up the speed. This would result in the dealer sometimes spitting out 1-2 more cards than it should have for each deal cycle. The shuffler motors, on the other hand, could have spun faster even though their RPM was already maxed. Precision is less necessary since the shuffler should introduce a degree of randomness anyway.

**Rotating Base:** We originally wanted to add a rotating base to the shuffler so it could deal in clockwise fashion. We determined a stepper motor to be the best option for this, but most commercial steppers run at 12V, requiring us to use a wall adapter. We found that the resulting current from these wall adapters were far above what our barrel jack connectors were rated for. Furthermore, when testing our stepper, its motor driver became burning hot even with a heatsink.

---

## Future Improvements

- **Spring-loaded card compression plate** — mechanically maintain consistent deck pressure to ensure single-card ejection accuracy
- **Higher-torque motors** — replace the dealer N20 motor with gear-reduced variant to handle dealing one card at a time without needing to turn power all the way up. 
- **Rotating stepper motor base** - add the stepper motor to allow for clockwise dealing, which is a more conventional form of dealing
- **Optical card sensor** — add an IR break-beam sensor at the ejection slot for card counting, replacing time-based dispensing
- **Rechargeable LiPo battery + charging circuit** — replace AA pack for a cleaner, self-contained form factor
- **PCB design** — replace breadboard prototypes with custom PCBs to reduce wiring complexity and improve reliability

## Schematic Diagrams
- **Motor** ESP32 Schematic
  
<img width="1948" height="1430" alt="image" src="https://github.com/user-attachments/assets/8ef24618-2843-490d-9224-d85dfaf9e088" />

- **Controller** ESP32 Schematic
  
<img width="1936" height="1574" alt="image" src="https://github.com/user-attachments/assets/0553ab45-5a06-4fd5-a274-fe9c027bc078" />




