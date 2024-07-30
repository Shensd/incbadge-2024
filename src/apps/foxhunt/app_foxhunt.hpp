#ifndef APP_FOXHUNT_HPP
#define APP_FOXHUNT_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include <RadioLib.h>

#include "radiohal.hpp"

#include "app.hpp"


class AppFoxHunt : public App {
private:
    const char* name = "FOXHUNT";

    float frequency = 433;

    int last_frequency_change = 0;

    uint32_t up_initial_hold = 0;
    uint32_t down_initial_hold = 0;
    uint32_t up_hold_tick = 0;
    uint32_t down_hold_tick = 0;

    bool require_frequency_confirmation = false;
public:
    AppFoxHunt(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_FOXHUNT_HPP