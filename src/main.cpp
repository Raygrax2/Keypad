#include "esp_task_wdt.h"
#include "Keypad.h"

extern "C" void app_main() {
    esp_task_wdt_deinit();

    Keypad keypad;
    printf("Keypad initialized.\n");

    while (true) {
        char key = keypad.poll();
        if (key != '\0') {
            printf("Key Pressed: %c\n", key);
        }
    }
}
