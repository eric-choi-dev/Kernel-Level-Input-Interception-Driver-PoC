#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include "input_manager.h"
#include "utils.h"

// [Configuration] Scan Code Definitions
enum ScanCodes { 
    KEY_ESC = 1, 
    KEY_W = 17, 
    KEY_A = 30, 
    KEY_S = 31, 
    KEY_D = 32, 
    KEY_SPACE = 57, 
    KEY_F10 = 68, // Exit Trigger
    KEY_F11 = 87  // Start Trigger
};

// Global State Management
InputManager inputMgr;
std::atomic<bool> isRunning(false);
std::atomic<bool> isAppRunning(true);

// [Drift Compensation] Variables to monitor coordinate deviation
// Used to prevent the cursor or logic from drifting too far from the origin.
std::atomic<long> x_axis_drift(0); 
std::atomic<long> y_axis_drift(0);
const int MAX_DRIFT_THRESHOLD_MS = 500; // Watchdog Threshold

// [Thread 1] Input Stability Test (Movement Simulation)
// Purpose: Continuously injects keyboard events to test kernel driver responsiveness
// and applies drift correction logic to prevent logical deadlocks.
void InputStabilityTestThread() {
    while (isAppRunning) {
        if (isRunning && inputMgr.targetKeyboard != 0) {
            
            // 1. Random Input Injection (Simulating sporadic user activity)
            if (GetRandomDelay(0, 10) > 8) {
                inputMgr.SendKey(KEY_SPACE, 100);
                Sleep(500); 
            }

            // 2. Drift Correction Logic (Watchdog mechanism)
            // Monitors input accumulation. If drift exceeds the threshold, 
            // the system automatically applies corrective input to restore balance.
            int duration = GetRandomDelay(100, 250); 
            int key = 0;

            if (x_axis_drift > MAX_DRIFT_THRESHOLD_MS) {
                key = KEY_A; x_axis_drift -= duration; // Correct Left
            } else if (x_axis_drift < -MAX_DRIFT_THRESHOLD_MS) {
                key = KEY_D; x_axis_drift += duration; // Correct Right
            } else {
                // Random movement within safe bounds
                if (GetRandomDelay(0, 1) == 0) { key = KEY_A; x_axis_drift -= duration; }
                else { key = KEY_D; x_axis_drift += duration; }
            }
            inputMgr.SendKey(key, duration);
            
            // Wait before next cycle to prevent IRQL saturation
            Sleep(GetRandomDelay(800, 1500)); 
        } else {
            // Idle state to save CPU resources
            Sleep(100);
        }
    }
}

// [Thread 2] Event Trigger Simulation
// Purpose: Simulates periodic mouse interrupt events to verify concurrent input handling.
void EventTriggerThread() {
    while (isAppRunning) {
        if (isRunning && inputMgr.targetMouse != 0) {
            inputMgr.SendMouseClick(INTERCEPTION_MOUSE_LEFT_BUTTON_DOWN, INTERCEPTION_MOUSE_LEFT_BUTTON_UP);
            Sleep(GetRandomDelay(400, 700));
        } else { 
            Sleep(100); 
        }
    }
}

int main() {
    SetConsoleTitleA("Kernel Input Interception Driver PoC - Debug Console");
    inputMgr.Initialize();

    InterceptionDevice device;
    InterceptionStroke stroke;

    std::cout << "===================================================" << std::endl;
    std::cout << " [Kernel Input Driver Research PoC] " << std::endl;
    std::cout << " Waiting for hardware interrupts (Move mouse/Press key)..." << std::endl;
    std::cout << " Controls: F11 (Start/Stop Test), F10 (Terminate)" << std::endl;
    std::cout << "===================================================" << std::endl;

    // Detach worker threads to run in background
    std::thread worker1(InputStabilityTestThread);
    std::thread worker2(EventTriggerThread);
    worker1.detach(); 
    worker2.detach();

    // Main Loop: Intercepts hardware signals from the driver buffer
    while (interception_receive(inputMgr.context, device = interception_wait(inputMgr.context), &stroke, 1) > 0) {
        // Dynamic Device Registration
        inputMgr.UpdateDevices(device);

        if (interception_is_keyboard(device)) {
            InterceptionKeyStroke& k = *(InterceptionKeyStroke*)&stroke;
            
            // Check for control keys (F11, F10)
            if (k.state == INTERCEPTION_KEY_DOWN) {
                if (k.code == KEY_F11) { 
                    isRunning = !isRunning;
                    std::cout << ">>> [STATE CHANGED] Test Running: " << (isRunning ? "TRUE" : "FALSE") << std::endl;
                }
                else if (k.code == KEY_F10) { 
                    std::cout << "XX [EXIT] Terminating Driver Connection..." << std::endl;
                    isAppRunning = false;
                    
                    // Forward the last keystroke and exit loop
                    interception_send(inputMgr.context, device, &stroke, 1);
                    break;
                }
            }
        }
        
        // Pass-through: Forward the original hardware input to the OS input stack.
        // Without this, the system would not receive any input (Freeze).
        interception_send(inputMgr.context, device, &stroke, 1);
    }
    return 0;
}