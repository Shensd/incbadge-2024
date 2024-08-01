#ifndef APP_FOX_HPP
#define APP_FOX_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include <RadioLib.h>

#include "radiohal.hpp"

#include "app.hpp"


class AppFox : public App {
private:
    const char* name = "FOX";
    const uint8_t CONFIG_ASK = 0;
    const uint8_t CONFIG_FSK = 1;

    float frequency = 433;

    uint8_t modulation = CONFIG_ASK;

    int last_frequency_change = 0;

    uint32_t right_initial_hold = 0;
    uint32_t left_initial_hold = 0;
    uint32_t right_hold_tick = 0;
    uint32_t left_hold_tick = 0;

    bool require_frequency_confirmation = false;

    uint32_t transmit_timer = 0;
    int32_t transmit_delay_ms = 30000; // 30 seconds
    int32_t transmit_duration_ms = 10000; // 10 seconds

    bool in_configuration_loop = false;

    int8_t configuration_options = 4;
    int8_t current_configuration_option = 0;

    float temp_frequency = 0;
    int32_t temp_transmit_delay_ms = 0;
    int32_t temp_transmit_duration_ms = 0;
    uint8_t temp_modulation = CONFIG_ASK;

    void loop_configuration(ButtonStates btn_states);
public:
    AppFox(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_FOX_HPP