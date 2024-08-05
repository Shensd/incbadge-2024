#include "buttons.hpp"



bool last_state_up = false, 
    last_state_down = false, 
    last_state_left = false, 
    last_state_right = false, 
    last_state_a = false, 
    last_state_b = false;

ButtonStates get_button_states() {
    struct ButtonStates button_states;

    button_states.UP = digitalRead(BTN_PIN_UP);
    button_states.DOWN = digitalRead(BTN_PIN_DOWN);
    button_states.LEFT = digitalRead(BTN_PIN_LEFT);
    button_states.RIGHT = digitalRead(BTN_PIN_RIGHT);
    button_states.A = digitalRead(BTN_PIN_A);
    button_states.B = digitalRead(BTN_PIN_B);

    button_states.UP_RISING_EDGE = 0;
    button_states.UP_FALLING_EDGE = 0;
    button_states.DOWN_RISING_EDGE = 0;
    button_states.DOWN_FALLING_EDGE = 0;
    button_states.LEFT_RISING_EDGE = 0;
    button_states.LEFT_FALLING_EDGE = 0;
    button_states.RIGHT_RISING_EDGE = 0;
    button_states.RIGHT_FALLING_EDGE = 0;
    button_states.A_RISING_EDGE = 0;
    button_states.A_FALLING_EDGE = 0;
    button_states.B_RISING_EDGE = 0;
    button_states.B_FALLING_EDGE = 0;

    bool UNPRESSED = false;
    bool PRESSED = true;

    if(last_state_up == PRESSED && button_states.UP == UNPRESSED) button_states.UP_FALLING_EDGE = true;
    if(last_state_up == UNPRESSED && button_states.UP == PRESSED) button_states.UP_RISING_EDGE = true;
    if(last_state_down == PRESSED && button_states.DOWN == UNPRESSED) button_states.DOWN_FALLING_EDGE = true;
    if(last_state_down == UNPRESSED && button_states.DOWN == PRESSED) button_states.DOWN_RISING_EDGE = true;
    if(last_state_left == PRESSED && button_states.LEFT == UNPRESSED) button_states.LEFT_FALLING_EDGE = true;
    if(last_state_left == UNPRESSED && button_states.LEFT == PRESSED) button_states.LEFT_RISING_EDGE = true;
    if(last_state_right == PRESSED && button_states.RIGHT == UNPRESSED) button_states.RIGHT_FALLING_EDGE = true;
    if(last_state_right == UNPRESSED && button_states.RIGHT == PRESSED) button_states.RIGHT_RISING_EDGE = true;
    if(last_state_a == PRESSED && button_states.A == UNPRESSED) button_states.A_FALLING_EDGE = true;
    if(last_state_a == UNPRESSED && button_states.A == PRESSED) button_states.A_RISING_EDGE = true;
    if(last_state_b == PRESSED && button_states.B == UNPRESSED) button_states.B_FALLING_EDGE = true;
    if(last_state_b == UNPRESSED && button_states.B == PRESSED) button_states.B_RISING_EDGE = true;

    last_state_up = button_states.UP;
    last_state_down = button_states.DOWN;
    last_state_left = button_states.LEFT;
    last_state_right = button_states.RIGHT;
    last_state_a = button_states.A;
    last_state_b = button_states.B;

    return button_states;
}