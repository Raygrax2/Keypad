#include "freertos/FreeRTOS.h"
#include "esp_task_wdt.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "KeyPad.h"

Keypad::Keypad() {
    // Initialize outputs
    for (int i = 0; i < ROWS; i++) {
        gpio_reset_pin(rowPins[i]);
        gpio_set_direction(rowPins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(rowPins[i], 1);
    }

    // Initialize inputs
    for (int i = 0; i < COLS; i++) {
        gpio_reset_pin(colPins[i]);
        gpio_set_direction(colPins[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(colPins[i], GPIO_PULLUP_ONLY);
    }

    lastKey = '\0'; // initialize 
    lastPressTime = 0;
    state = KeyState::Idle;
}

// Polling method: returns '\0'if no key was pressed
char Keypad::poll() {
    // Check delta time
    int64_t now = esp_timer_get_time() / 1000; //microseconds to miliseconds
    int64_t deltaTime = now - lastPressTime;

    for (int row = 0; row < ROWS; row++) {
        gpio_set_level(rowPins[row], 0); // Set row to low

        for (int col = 0; col < COLS; col++) {
            if (gpio_get_level(colPins[col]) == 0) {
                char currentKey = keymap[row][col];

                switch (state) {
                    case KeyState::Idle:
                        lastPressTime = now;
                        lastKey = currentKey;
                        state = KeyState::Debouncing;
                        break;

                    case KeyState::Debouncing:
                        if (currentKey == lastKey && deltaTime > DEBOUNCE_MS) {
                            state = KeyState::WaitingForRelease;
                        }
                        break;

                    case KeyState::WaitingForRelease:
                        // Still waiting for key to be released
                        break;
                }

                gpio_set_level(rowPins[row], 1); // Restore row
                return '\0'; // Always return '\0' during press phase
            }
        }

        gpio_set_level(rowPins[row], 1); // retsore the row to high
    }

    // Check for key release
    if (state == KeyState::WaitingForRelease && deltaTime > DEBOUNCE_MS) {
        state = KeyState::Idle;
        char pressed = lastKey;
        lastKey = '\0';
        return pressed;
    }

    return '\0'; // No key press detected
}
