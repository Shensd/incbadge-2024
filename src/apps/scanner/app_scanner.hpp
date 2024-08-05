#ifndef APP_SCANNER_HPP
#define APP_SCANNER_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include "app.hpp"

#include "radiohal.hpp"

class AppScanner : public App {
private:
    const char* name = "SCANNER";

    const uint8_t num_scanner_frequencies = 17;
    const float scanner_frequency_list[17] = {
        300.00, 303.87, 304.25, 310.00, 315.00, 318.00, 390.00, 418.00, 433.07, 433.42, 433.92, 434.42, 434.77, 438.90, 868.35, 915.00, 925.00
    };

    bool currently_scanning = false;

    uint8_t frequency_index = 0;

    uint16_t rx_bw = 270;

    float rssi = 0;
    float rssi_thresh = -80.0;

    int8_t freq_index_with_activity = -1;
    bool activity_on_frequency = false;

    uint32_t time_at_frequency_ms = 500;
    uint32_t scanner_timer = 0;

    void step_frequency_index(void);
public:
    AppScanner(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_SCANNER_HPP