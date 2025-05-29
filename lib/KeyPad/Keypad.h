#include "driver/gpio.h"

#define ROWS 4
#define COLS 4
#define DEBOUNCE_MS 10

class Keypad {
public:
    Keypad();

    char poll();
private:
    enum class KeyState { Idle, Debouncing, WaitingForRelease };
    KeyState state;

    gpio_num_t rowPins[ROWS] = {GPIO_NUM_13, GPIO_NUM_12, GPIO_NUM_14, GPIO_NUM_27};
    gpio_num_t colPins[COLS] = {GPIO_NUM_26, GPIO_NUM_25, GPIO_NUM_33, GPIO_NUM_32};
    // Key layout
    char keymap[ROWS][COLS] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };

    char lastKey;
    int64_t lastPressTime;
};
