#ifndef APP_README_HPP
#define APP_README_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include "app.hpp"

class AppReadme : public App {
private:
    const char* name = "README";

    int32_t scroll_offset = 0;
public:
    AppReadme(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_README_HPP