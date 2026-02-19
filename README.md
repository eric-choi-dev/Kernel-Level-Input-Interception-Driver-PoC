# Kernel-Level Input Interception Driver PoC

![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows_Kernel-lightgrey.svg)
![Topic](https://img.shields.io/badge/topic-Systems_Programming-orange.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

## 🚀 Overview

This project demonstrates a **Proof of Concept (PoC)** for manipulating Human Interface Device (HID) inputs at the kernel level using the **Interception API**. 

The primary goal of this research was to bypass user-mode hooks (which are easily detected by standard automation/anti-cheat tools) and simulate hardware-level input signals directly into the OS input stack. It also implements a robust **fail-safe watchdog mechanism** to prevent input lockups or logical "drifting" issues that frequently occur during high-frequency automated testing.

## 🛠️ Tech Stack

* **Language:** Modern C++ (C++17)
* **Core Mechanisms:** Kernel-Mode Communication, Thread Synchronization, IRP (I/O Request Packet) Handling
* **Key Concepts:** IRQL (Interrupt Request Level), DPC (Deferred Procedure Call), User-to-Kernel Space Transitions
* **Debugging Tools:** WinDbg, Windows Driver Kit (WDK)

## 💡 Key Features

* **Kernel-Mode Communication:** Interfaced directly with a signed kernel driver (`interception.sys`) to inject keystrokes and mouse events at the lowest possible level in the input stack.
* **Drift Correction Watchdog:** Implemented a custom balancing algorithm (`x_axis_drift`) to continuously monitor cursor and character deviation over time. If the drift exceeds `MAX_DRIFT_THRESHOLD_MS`, the system automatically applies precise corrective inputs to prevent logical deadlocks.
* **Latency Simulation Engine:** Integrated a randomization engine to simulate human-like input latency (50ms - 150ms). This actively tests the driver's responsiveness and stability under varied and unpredictable load conditions.

## ⚙️ Architecture & Workflow

1. **Driver Initialization:** The application establishes a secure context with the Interception driver, opening a communication channel between user space and kernel space.
2. **Device Filtering:** Scans and filters Hardware IDs to identify and latch onto specific physical keyboards or mice.
3. **Input Injection:** Constructs and dispatches `InterceptionKeyStroke` and `InterceptionMouseStroke` structures directly to the driver buffer for OS execution.

## Architecture
<img width="885" height="920" alt="architecture_diagram" src="https://github.com/user-attachments/assets/25c8bbef-452b-40c7-a7da-cde903a038c2" />

## 🐛 Troubleshooting & WinDbg Analysis (BSOD)

During the early development and stress-testing phases, the system encountered a **`DPC_WATCHDOG_VIOLATION` (Bug Check 0x133)**. 

* **Root Cause Analysis:** The user-mode input injection thread was spamming the driver buffer too rapidly without yielding. This caused the kernel thread to hang at `DISPATCH_LEVEL`, preventing the processor from executing Deferred Procedure Calls (DPCs) within the allocated time limit.
* **Resolution:** Re-engineered the injection loop by implementing proper thread sleeping (yielding execution) and a drift monitoring system. This ensures the kernel driver has adequate cycles to process the **I/O Request Packets (IRPs)** without starving the CPU.

> 📝 **Note:** For a detailed breakdown of the crash dump, see [`docs/bsod_analysis_log.txt`](docs/bsod_analysis_log.txt) for the full WinDbg analysis log.

## ⚠️ Disclaimer

This project is strictly for **educational and research purposes only**. It was developed to demonstrate advanced system programming concepts, including thread management, user-kernel driver communication, and low-level debugging.



