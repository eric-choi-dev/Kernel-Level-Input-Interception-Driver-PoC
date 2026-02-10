#pragma once
#include <interception.h>
#include <windows.h>

class InputManager {
public:
    InterceptionContext context;
    InterceptionDevice targetMouse = 0;
    InterceptionDevice targetKeyboard = 0;

    InputManager();
    ~InputManager();

    // Initializes the interception context and sets up device filters.
    void Initialize();

    // Identifies and assigns device handles based on hardware IDs.
    void UpdateDevices(InterceptionDevice device); 
    
    // Injects a keystroke signal directly into the kernel input stack.
    // param scanCode: The hardware scan code of the key.
    // param duration: Simulates the physical key-press duration.
    void SendKey(unsigned short scanCode, int duration);

    // Injects mouse click events (Down/Up state) to the kernel.
    void SendMouseClick(int stateDown, int stateUp);
};