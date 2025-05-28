// main.cpp
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "keypad.h"

extern "C" void app_main() {
    esp_task_wdt_deinit();

    setup_keypad();
    printf("Keypad Set\n");

    while (1) {
        char key = get_key();
        if (key != '\0') {
            printf("Key Pressed: %c\n", key);
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Pequeña espera para evitar saturar la CPU
    }
}
