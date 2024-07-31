#ifndef APP_SCANNER_HPP
#define APP_SCANNER_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include "app.hpp"

#include "radiohal.hpp"

class AppScanner : public App {
private:
    const char* name = "SCANNER";

    float frequency_start = 430.0;
    float frequency_end = 440.0;

    float step = 0.125;
public:
    AppScanner(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_SCANNER_HPP