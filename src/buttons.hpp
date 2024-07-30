#ifndef BUTTONS_HPP
#define BUTTONS_HPP

#include <Arduino.h>
#include "pins.hpp"

// typedef void (*button_tick_callback)(uint8_t);

// #define BUTTON_TICK_MAX_CBS 64

// namespace Buttons_NS {
//     struct ButtonTick {
//         uint32_t timer;
//         button_tick_callback callback;
//         uint32_t tick_length_ms;
//     };

//     ButtonTick button_ticks[BUTTON_TICK_MAX_CBS];
//     uint8_t callback_index = 0;
// };

struct ButtonStates {
    bool UP = 0;
    bool DOWN = 0;
    bool LEFT = 0;
    bool RIGHT = 0;
    bool A = 0;
    bool B = 0;

    bool UP_RISING_EDGE = 0; // first frame button pressed down
    bool UP_FALLING_EDGE = 0; // first frame button released
    bool DOWN_RISING_EDGE = 0;
    bool DOWN_FALLING_EDGE = 0;
    bool LEFT_RISING_EDGE = 0;
    bool LEFT_FALLING_EDGE = 0;
    bool RIGHT_RISING_EDGE = 0;
    bool RIGHT_FALLING_EDGE = 0;
    bool A_RISING_EDGE = 0;
    bool A_FALLING_EDGE = 0;
    bool B_RISING_EDGE = 0;
    bool B_FALLING_EDGE = 0;
};

ButtonStates get_button_states();

// int32_t set_button_hold_tick_callback(button_tick_callback cb, uint32_t tick_length_ms);

// void tick_button(ButtonStates button_states, int32_t callback_id);

#endif // !BUTTONS_HPP