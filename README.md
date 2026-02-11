# Kernel-Level Input Interception Driver PoC

## Overview
This project demonstrates a **Proof of Concept (PoC)** for manipulating Human Interface Device (HID) inputs at the kernel level using the **Interception API**. 

The primary goal of this research was to bypass user-mode hooks (used by standard automation tools) and simulate hardware-level input signals directly into the OS input stack. It also implements a **fail-safe watchdog mechanism** to prevent input lockups or "Drifting" issues that can occur during automated testing.

## Technical Highlights
* **Kernel-Mode Communication:** Interfaced with a signed kernel driver (`interception.sys`) to inject keystrokes and mouse events.
* **Drift Correction Watchdog:** Implemented a custom balancing algorithm (`x_axis_drift`) to monitor cursor/character deviation over time. If the drift exceeds `MAX_DRIFT_THRESHOLD_MS`, the system automatically applies corrective input to prevent logical deadlocks.
* **Latency Simulation:** Integrated a randomization engine to simulate human-like input latency (50ms - 150ms), testing the driver's responsiveness under varied load conditions.

## Architecture
<img width="885" height="920" alt="architecture_diagram" src="https://github.com/user-attachments/assets/25c8bbef-452b-40c7-a7da-cde903a038c2" />

1.  **Driver Initialization:** The application establishes a context with the Interception driver.
2.  **Device Filtering:** Filters Hardware IDs to identify specific keyboards/mice.
3.  **Input Injection:** Sends `InterceptionKeyStroke` and `InterceptionMouseStroke` structs to the driver buffer.

## Troubleshooting & Debugging (BSOD)
During the early development phase, I encountered a **DPC_WATCHDOG_VIOLATION (Bug Check 0x133)**.
* **Cause:** The input injection thread was spamming the driver buffer too rapidly without yielding, causing the system to hang at `DISPATCH_LEVEL`.
* **Resolution:** Implemented thread sleeping and a drift monitoring system to ensure the driver has time to process the I/O request packet (IRP).
* *See `docs/bsod_analysis_log.txt` for the windbg analysis log.*

## Disclaimer
This project is for **educational and research purposes only**. It demonstrates system programming concepts including thread management, driver communication, and debugging.
