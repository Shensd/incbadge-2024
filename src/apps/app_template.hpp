#ifndef APP_TEMPLATE_HPP
#define APP_TEMPLATE_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include "app.hpp"

class AppTemplate : public App {
private:
    const char* name = "Idle Animation";
public:
    AppTemplate(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_TEMPLATE_HPP