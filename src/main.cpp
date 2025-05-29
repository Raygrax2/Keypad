#include "freertos/FreeRTOS.h"
#include "esp_task_wdt.h"
#include "driver/gpio.h"

#define KEY_DEBOUNCE_MS 10

// Define rows and columns
#define ROWS 4
#define COLS 4

// GPIO pins
gpio_num_t rowPins[ROWS] = {GPIO_NUM_13, GPIO_NUM_12, GPIO_NUM_14, GPIO_NUM_27};
gpio_num_t colPins[COLS] = {GPIO_NUM_26, GPIO_NUM_25, GPIO_NUM_33, GPIO_NUM_32};

// Key layout
char keymap[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

void setup_keypad() {
    for (int i = 0; i < ROWS; i++) {
        gpio_reset_pin(rowPins[i]);
        gpio_set_direction(rowPins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(rowPins[i], 1);
    }

    for (int i = 0; i < COLS; i++) {
        gpio_reset_pin(colPins[i]);
        gpio_set_direction(colPins[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(colPins[i], GPIO_PULLUP_ONLY);
    }
}

char get_key() {
    for (int row = 0; row < ROWS; row++) {
        gpio_set_level(rowPins[row], 0); // Pull row low

        for (int col = 0; col < COLS; col++) {
            if (gpio_get_level(colPins[col]) == 0) {
                // debounce
                vTaskDelay(pdMS_TO_TICKS(KEY_DEBOUNCE_MS));
                if (gpio_get_level(colPins[col]) == 0) {
                    // wait for release
                    while (gpio_get_level(colPins[col]) == 0);
                    // restore row
                    gpio_set_level(rowPins[row], 1); 
                    return keymap[row][col];
                }
            }
        }

        gpio_set_level(rowPins[row], 1); // restore row to high
    }

    return '\0'; // no key pressed
}

extern "C" void app_main() {
    esp_task_wdt_deinit();

    setup_keypad();
    printf("Keypad Set\n");
    while(1) {
        char key = get_key();

        if (key != '\0') {
            printf("Key Pressed: %c\n", key);
        }
    }
}