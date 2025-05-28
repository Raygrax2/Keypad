#include "keypad.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "esp_task_wdt.h"

#define KEY_DEBOUNCE_MS 10 ///< Tiempo de rebote para evitar lecturas falsas (en milisegundos)
#define ROWS 4             ///< Número de filas en el teclado
#define COLS 4             ///< Número de columnas en el teclado

// Pines GPIO para filas del teclado
static gpio_num_t rowPins[ROWS] = {GPIO_NUM_13, GPIO_NUM_12, GPIO_NUM_14, GPIO_NUM_27};

// Pines GPIO para columnas del teclado
static gpio_num_t colPins[COLS] = {GPIO_NUM_26, GPIO_NUM_25, GPIO_NUM_33, GPIO_NUM_32};

// Mapa de teclas del teclado 4x4
static char keymap[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

void setup_keypad() {
    // Configura pines de fila como salida
    for (int i = 0; i < ROWS; i++) {
        gpio_reset_pin(rowPins[i]);
        gpio_set_direction(rowPins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(rowPins[i], 1); // Apagado por defecto
    }

    // Configura pines de columna como entrada con pull-up
    for (int i = 0; i < COLS; i++) {
        gpio_reset_pin(colPins[i]);
        gpio_set_direction(colPins[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(colPins[i], GPIO_PULLUP_ONLY);
    }
}

char get_key() {
    for (int row = 0; row < ROWS; row++) {
        gpio_set_level(rowPins[row], 0); // Activar fila

        for (int col = 0; col < COLS; col++) {
            if (gpio_get_level(colPins[col]) == 0) {
                vTaskDelay(pdMS_TO_TICKS(KEY_DEBOUNCE_MS));
                if (gpio_get_level(colPins[col]) == 0) {
                    while (gpio_get_level(colPins[col]) == 0); // Espera a que se suelte la tecla
                    gpio_set_level(rowPins[row], 1); // Restaurar fila
                    return keymap[row][col];
                }
            }
        }

        gpio_set_level(rowPins[row], 1); // Restaurar fila
    }

    return '\0'; // Ninguna tecla presionada
}
