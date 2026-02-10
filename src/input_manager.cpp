#include "input_manager.h"
#include "utils.h"

InputManager::InputManager() {
    context = 0;
}

InputManager::~InputManager() {
    if (context) interception_destroy_context(context);
}

void InputManager::Initialize() {
    context = interception_create_context();
    
    // Apply filters to capture input from ALL keyboard and mouse devices at the driver level.
    // This allows the application to intercept and modify hardware signals before they reach the OS.
    interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);
    interception_set_filter(context, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_ALL);
}

void InputManager::UpdateDevices(InterceptionDevice device) {
    // Automatically detect and assign the first active keyboard/mouse device handle.
    if (interception_is_keyboard(device) && targetKeyboard == 0) {
        targetKeyboard = device;
    }
    else if (interception_is_mouse(device) && targetMouse == 0) {
        targetMouse = device;
    }
}

void InputManager::SendKey(unsigned short scanCode, int duration) {
    if (targetKeyboard == 0) return;

    InterceptionKeyStroke kStroke = {0};
    kStroke.code = scanCode;
    kStroke.state = INTERCEPTION_KEY_DOWN;
    interception_send(context, targetKeyboard, (InterceptionStroke*)&kStroke, 1);
    
    // Simulate physical hardware latency (Key press duration).
    // Using 0 duration might cause some applications to miss the input event.
    Sleep(duration > 0 ? duration : GetRandomDelay(50, 150));
    
    kStroke.state = INTERCEPTION_KEY_UP;
    interception_send(context, targetKeyboard, (InterceptionStroke*)&kStroke, 1);
}

void InputManager::SendMouseClick(int stateDown, int stateUp) {
    if (targetMouse == 0) return;

    InterceptionMouseStroke mStroke = {0};
    mStroke.state = stateDown;
    interception_send(context, targetMouse, (InterceptionStroke*)&mStroke, 1);
    
    // Introduce micro-latency between Down and Up events to mimic human behavior.
    Sleep(GetRandomDelay(80, 150));
    
    mStroke.state = stateUp;
    interception_send(context, targetMouse, (InterceptionStroke*)&mStroke, 1);
}