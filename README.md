# USSIM — Universal Signal Simulation in Microwave Landing System (MLS)

**Authors:**  
William Marchineck, Muhammad Umar, Aisha Mohammed, and Abner Ben  
**Date:** May 12, 2025  
**Memo Number:** SDD-2501  

---

## 🛰️ Overview

The **Universal Signal Simulation (USSIM)** project simulates the transmission of aviation navigation signals within a **Microwave Landing System (MLS)** environment. It replicates the behavior of ground station signals — **Azimuth (AZ)**, **Elevation (EL)**, and **Back Azimuth (BAZ)** — while conforming to **FAA-STD-022C** and **ICAO Annex 10, Chapter 3.11** specifications.

USSIM interacts with a **Universal Timing Control Unit (UTCU)** to generate near-real-time **sinusoidal waveforms** with modulation patterns that emulate real-world MLS ground station transmissions.

The system is **microcontroller-based** and built using an **Adafruit ItsyBitsy M4 Express**. It produces analog signals via amplitude and phase modulation, creating an accurate, low-cost avionics simulation tool for research and education.

---

## ✈️ Executive Summary

- **Purpose:** Simulate MLS signal transmission and scanning beam patterns.  
- **Hardware:** Adafruit Itsy Bitsy M4 Express (ARM Cortex M4, 120 MHz, 12-bit DAC).  
- **Input:** Control data from UTCU via digital I/O interface.  
- **Output:** Analog MLS signal-in-space waveform via DAC (A0 pin).  
- **Validation:** Tested using oscilloscope waveform captures; matched FAA/ICAO specifications.

---

## ⚙️ System Architecture

### Functional Modules

| **Module** | **Description** |
|-------------|-----------------|
| **Input Control Interface** | Reads control data from UTCU (e.g., signal type, scan direction, phase, and start/stop flags) via digital input pins. |
| **Update Signal Block** | Processes control data and updates internal state variables (antenna type, scan direction, transmission state). |
| **Generate Base Signal Block** | Computes sinusoidal carrier and scanning beam envelopes using amplitude and phase modulation. |
| **Analog Output Module** | Outputs the final modulated waveform via DAC (A0 pin). |

### Interfaces

- **External:** UTCU → USSIM (digital I/O control signals)  
- **Internal:** State variables (antenna_num, currPhase, to_fro, etc.) shared among modules  
- **Output:** DAC analog voltage output → oscilloscope

---

## 🔩 Hardware Design

### Adafruit Itsy Bitsy M4 Express
- **Processor:** ARM Cortex M4 @ 120 MHz  
- **DAC:** Integrated 12-bit  
- **Analog Output:** Pin A0  
- **I/O:** Digital input pins for UTCU control  

### Connections
| Pin | Function |
|------|-----------|
| 5 | aux0 (control input) |
| 7 | aux1 (control input) |
| 9 | aux2 (control input) |
| 10 | tx (transmit enable) |
| 11 | currPhase (phase control) |
| 12 | start_stop (TX on/off) |
| 13 | to_fro (scan direction) |
| A0 | Analog output (DAC) |

---

## 💻 Software Design

### Core Structure
The program runs entirely on the Itsy Bitsy M4 Express using Arduino C++.  
It is organized around `setup()` and `loop()` functions, with helper functions for waveform generation and signal updates.

### Key Functions
- `CarrierSignal()` – Generates sinusoidal carrier waves (0 and π phase shift).  
- `ScanningBeam()` – Produces Gaussian and sinc-based beam envelopes for scanning patterns.  
- `convert_to_int()` – Converts auxiliary pin states into an antenna index.  
- `loop()` – Reads inputs, applies modulation logic, and outputs the correct waveform sample via DAC.

### Libraries Used
- **Arduino Core** (for digital/analog I/O and timing)  
- **Math.h** (sine, power, and trigonometric calculations)  
- **Serial Library** (debugging and waveform output monitoring)

---

## 🧪 Testing and Validation

### Integration Testing
- Simulated UTCU control signals verified proper transitions between AZ, EL, and BAZ modes.  
- Real-time waveform output validated using an oscilloscope.

### Verification & Validation Testing
- Confirmed correct reading of digital input states and phase transitions.  
- Validated output timing intervals and waveform structure against **FAA-STD-022** and **ICAO Annex 10** specifications.  
- Observed correct **DPSK modulation**, **beam scanning patterns**, and **amplitude envelopes**.

---

## 📈 Results

- Accurate generation of **DPSK-modulated** sinusoidal signals.  
- Simulated beam scanning envelopes matched theoretical sinc-based behavior.  
- Demonstrated real-time performance using only Arduino-compatible hardware.

---

## 🧰 Future Improvements

- Implement additional modulation formats.  
- Add EMI interface protection for real-world applications.  
- Upgrade to STM32 or FPGA for higher clock resolution and better timing control.

---

## 🧠 Lessons Learned

- **Timing accuracy** on Arduino is limited; advanced timing hardware improves results.  
- **DPSK detection** depends heavily on precise preamble formatting.  
- **Modular design** simplified debugging and integration testing.

---

## ✅ Conclusion

The USSIM system successfully demonstrates **MLS signal-in-space simulation**, accurately reproducing sinusoidal signals with amplitude and phase modulation. The project meets all performance goals and adheres to FAA and ICAO standards.  
Its modular, low-cost design makes it ideal for **educational, research, and development** use in avionics signal simulation.

---

## 📚 References

1. **ICAO Annex 10**, Chapter 3.11 – MLS signal protocols and timing.  
2. **FAA-STD-022C** – Ground Equipment Performance and scanning beam structure.  
3. **IEC 62680** – USB 2.0 Data Transfer Standard.  
4. **Kelly-Redlein Paper** – Reference study for MLS modulation behavior.  

---

## 💾 Appendix: Source Code

The complete Arduino sketch for waveform generation, signal handling, and DAC output is included in the folders. 
It defines hardware pin configurations, waveform-generation logic, and control-signal handling for the simulation.

