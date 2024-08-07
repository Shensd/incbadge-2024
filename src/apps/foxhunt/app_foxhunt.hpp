#ifndef APP_FOXHUNT_HPP
#define APP_FOXHUNT_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include <RadioLib.h>

#include "radiohal.hpp"

#include "app.hpp"


class AppFoxHunt : public App {
private:
    const char* name = "FOXHUNT";
    const uint8_t CONFIG_ASK = 0;
    const uint8_t CONFIG_FSK = 1;

    float frequency = 433;

    // uint8_t modulation = CONFIG_ASK;

    int last_frequency_change = 0;

    uint32_t right_initial_hold = 0;
    uint32_t left_initial_hold = 0;
    uint32_t right_hold_tick = 0;
    uint32_t left_hold_tick = 0;

    bool require_frequency_confirmation = false;

    float previous_rssi_readings[128];
    uint32_t rssi_reading_frequency_ms = 50;
    uint32_t last_rssi_reading = 0;
    uint8_t rssi_reading_index = 0;

    long rssi_upper_bound = -20;
    long rssi_lower_bound = -110;

    const uint16_t available_bandwidths[16] = {
        58, 68, 81, 102, 116, 135, 162, 203, 232, 270, 325, 406, 464, 541, 650, 812
    };
    int8_t rx_bandwidth_index = 9;
    int8_t temp_rx_bandwidth_index = 0;


    // configuration loop vars
    bool in_configuration_loop = false;

    int8_t configuration_options = 4;
    int8_t current_configuration_option = 0;

    float temp_frequency = 0;
    long temp_rssi_upper_bound = 0;
    long temp_rssi_lower_bound = 0;


    uint8_t temp_modulation = CONFIG_ASK;

    void loop_configuration(ButtonStates btn_states);

public:
    AppFoxHunt(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_FOXHUNT_HPP