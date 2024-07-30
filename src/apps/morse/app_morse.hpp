#ifndef APP_MORSE_HPP
#define APP_MORSE_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include <RadioLib.h>

#include "radiohal.hpp"

#include "app.hpp"


class AppMorse : public App {
private:
    const char* name = "MORSE";

    float frequency = 433;

    int last_frequency_change = 0;

    uint32_t up_hold_tick = 0;
    uint32_t down_hold_tick = 0;

    bool require_frequency_confirmation = false;

    bool currently_transmitting = false;
    PinStatus last_gd0_read = LOW;

    float last_rssi = 0;
public:
    AppMorse(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_MORSE_HPP