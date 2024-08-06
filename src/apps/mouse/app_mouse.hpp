#ifndef APP_MOUSE_HPP
#define APP_MOUSE_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include "app.hpp"
#include "radiohal.hpp"

#include <Mouse.h>
// #include <Adafruit_TinyUSB.h>

namespace AppMouse_NS {
void receive_cb(void);
}

class AppMouse : public App {
private:
    // Adafruit_USBD_HID usb_mouse;

    const char* name = "MOUSE";

    const uint8_t MAGIC[4] = {0xBA, 0xBA, 0x12, 0x34};

    bool do_receive = true;
    bool temp_do_receive = 0;

    float frequency = 433.00;
    float temp_frequency = 0;

    uint8_t id = 20;
    int8_t temp_id = 0;

    bool receive_mode_started = false;
    bool transmit_mode_started = false;

    uint8_t packet_buffer[7];

    uint32_t right_initial_hold = 0;
    uint32_t left_initial_hold = 0;
    uint32_t right_hold_tick = 0;
    uint32_t left_hold_tick = 0;
    int8_t configuration_options = 3;
    int8_t current_configuration_option = 0;
    bool in_configuration_loop = false;
    void loop_configuration(ButtonStates btn_states);

    void start_receive_mode();
    void start_transmit_mode();
public:
    AppMouse(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_MOUSE_HPP