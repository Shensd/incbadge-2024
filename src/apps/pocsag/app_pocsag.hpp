#ifndef APP_POCSAG_HPP
#define APP_POCSAG_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include "app.hpp"

class AppPOCSAG : public App {
private:
    const char* name = "POCSAG";

    PagerClient* pager = NULL;

    uint8_t read_buffer[256];
    size_t bytes_read = sizeof(read_buffer) / sizeof(read_buffer[0]);
    uint32_t received_addr = 0;

    char display_buffer[160];

    float frequency = 433;
    float temp_frequency = 0;

    const uint16_t available_speeds[3] = {512, 1200, 2400};

    uint16_t speed_index = 1;
    uint16_t temp_speed_index = 0; 
public:
    AppPOCSAG(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_POCSAG_HPP