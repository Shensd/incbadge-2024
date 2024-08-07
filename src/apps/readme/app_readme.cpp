#include "app_readme.hpp"

AppReadme::AppReadme(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppReadme::setup() {
    scroll_offset = 0;
}

void AppReadme::loop(ButtonStates btn_states) {
    if(btn_states.B_RISING_EDGE) {
        handler->exit_current();
        return;
    }

    display->clearDisplay();

    const uint32_t num_messages = sizeof(messages) / sizeof(messages[0]);

    if(btn_states.RIGHT_RISING_EDGE) {
        scroll_offset++;
        if(scroll_offset >= num_messages) scroll_offset = num_messages - 1;
    }
    if(btn_states.LEFT_RISING_EDGE) {
        scroll_offset--;
        if(scroll_offset < 0) scroll_offset = 0;
    }

    display->setTextColor(SSD1306_WHITE);
    display->setTextSize(1);
    display->cp437(true);
    display->setCursor(0, 0);

    // display->write(messages[scroll_offset]);
    int y = 0;
    for(int i = 0; i < sizeof(messages[scroll_offset]); i++) {
        if(messages[scroll_offset][i] == 0) break;

        if(messages[scroll_offset][i] == '\n') {
            display->setCursor(0, display->getCursorY() + 8);
            continue;
        }

        display->write(messages[scroll_offset][i]);
    }

    display->fillRect(0, SCREEN_HEIGHT - 8, SCREEN_WIDTH, 8, SSD1306_WHITE);
    display->setTextColor(SSD1306_BLACK);
    display->setCursor(0, SCREEN_HEIGHT - 8);
    sprintf(buffer, "%s %02d / %02d %s", scroll_offset > 0 ? "<-" : "[ ", scroll_offset + 1, num_messages, scroll_offset < (num_messages - 1) ? "->" : " ]");
    display->write(buffer);

    display->display();
}

void AppReadme::loop1() {

}

void AppReadme::close() {
    
}