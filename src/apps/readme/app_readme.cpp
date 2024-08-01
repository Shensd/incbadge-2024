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

    // screen has a max width of 21 characters
    char messages[][160] = {
        "Controls: The right\ntwo buttons are\nB and A.\nThe left four are\nup/down/left/right.\nNav this readme with left and right.",
        "Apps can be exited by\npressing B, pressing\nA in most apps opens\na settings menu.",
        "This device\ntransmits radio!\nMake sure you are\npermitted to transmit\non the frequencies\nyou transmit on!",
        "Source code can\nbe found at\ngithub.com/shensd/\nincbadge-2024",
        "Happy DEFCON 32!\n(you can press B to\nexit to main menu)",
    };

    const uint32_t num_messages = sizeof(messages) / sizeof(messages[0]) - 1;

    if(btn_states.RIGHT_RISING_EDGE) {
        scroll_offset++;
        if(scroll_offset > num_messages) scroll_offset = num_messages;
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
    for(int i = 0; i < sizeof(messages[0]); i++) {
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
    char buffer[32];
    sprintf(buffer, "%s %02d / %02d %s", scroll_offset > 0 ? "<-" : "[ ", scroll_offset + 1, num_messages + 1, scroll_offset < num_messages ? "->" : " ]");
    display->write(buffer);

    display->display();
}

void AppReadme::loop1() {

}

void AppReadme::close() {
    
}