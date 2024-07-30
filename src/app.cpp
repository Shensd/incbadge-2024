#include "app.hpp"

AppHandler::AppHandler(Adafruit_SSD1306 *display) : display(display) {}

void AppHandler::exit_current() {
    if(current_app == NULL) return;

    current_app->close();

    current_app = NULL;
}
void AppHandler::loop(ButtonStates btn_states) {
    if(current_app != NULL) {
        current_app->loop(btn_states);
        return;
    }

    if(btn_states.A_FALLING_EDGE) {
        start_app_by_index(ui_selected_app);
        return;
    }

    if(btn_states.DOWN_FALLING_EDGE) {
        ui_selected_app = (ui_selected_app + 1) % num_apps;
    }
    if(btn_states.UP_FALLING_EDGE) {
        ui_selected_app = ui_selected_app - 1;
        if(ui_selected_app < 0) ui_selected_app = num_apps - 1;
    }

    display->clearDisplay();

    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(0, 0);
    display->cp437(true);

    uint8_t num_apps_to_display = 6;

    display->fillRect(0, 0, 128, 9, SSD1306_WHITE);

    for(uint32_t i = ui_selected_app; i < min(ui_selected_app + num_apps_to_display, num_apps); i++) {
        uint32_t position = i - ui_selected_app;

        if(position == 0) {
            display->setTextColor(SSD1306_BLACK);
        } else {
            display->setTextColor(SSD1306_WHITE);
        }

        display->setCursor(0, position*10);
        display->write(apps[i]->get_name());
    }

    display->display();
}
void AppHandler::loop1() {
    if(current_app != NULL) {
        current_app->loop1();
        return;
    }
}