#ifndef APP_JAMMER_HPP
#define APP_JAMMER_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include "app.hpp"

#include "radiohal.hpp"

class AppJammer : public App {
private:
    const char* name = "JAMMER";

    float frequency = 433;

    uint32_t right_initial_hold = 0;
    uint32_t left_initial_hold = 0;
    uint32_t right_hold_tick = 0;
    uint32_t left_hold_tick = 0;
    bool in_configuration_loop = false;
    int8_t configuration_options = 1;
    int8_t current_configuration_option = 0;

    float temp_frequency = 0;

    volatile bool do_jamming = false;

    bool jammer_status = false;

    void loop_configuration(ButtonStates btn_states);
public:
    AppJammer(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_JAMMER_HPP