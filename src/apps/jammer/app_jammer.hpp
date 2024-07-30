#ifndef APP_JAMMER_HPP
#define APP_JAMMER_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include "app.hpp"

class AppJammer : public App {
private:
    const char* name = "JAMMER";

    float frequency = 433;

    uint32_t up_hold_tick = 0, down_hold_tick = 0;
    bool require_frequency_confirmation = false;

    bool do_jamming = false;

    bool jammer_status = false;

    uint32_t jam_square_x = SCREEN_WIDTH / 2;
    uint32_t jam_square_y = SCREEN_HEIGHT / 2;
public:
    AppJammer(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_JAMMER_HPP