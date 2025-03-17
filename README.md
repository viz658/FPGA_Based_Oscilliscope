# FPGA-Based Digital Oscilliscope
## Overview
This FPGA-based digital oscilloscope is implemented on an FPGA-embedded SoC and designed to measure and display peak-to-peak voltage from multiple channels. It features real-time VGA waveform rendering, keyboard-controlled interface adjustments, and ADC signal acquisition.

The project is based on Pong Chu’s prototyping using systemverilog framework, with modifications to the VGA core driver and custom integration of existing software drivers to create an interactive oscilloscope.  

---

## Features

### Multi-Channel Signal Acquisition
- Supports multiple ADC input channels for real-time signal capture.
- Displays peak-to-peak voltage readings from the selected channel.

---

### Real-Time VGA Graphical Display
- Custom VGA core modification for smooth waveform rendering.
- Uses a framebuffer-based plotting system for visualization.

---

### Keyboard-Controlled User Interface
- Users can switch channels, adjust time base, and toggle waveform display using PS/2 keyboard controls.
  
---

### Peak-to-Peak Voltage Measurement
- Computes and displays peak-to-peak voltage in real time.
- On-Screen Display overlay shows voltage values on VGA output.

---

## Demo Video:
https://youtube.com/shorts/Sao8s9ptafk?feature=share

---
