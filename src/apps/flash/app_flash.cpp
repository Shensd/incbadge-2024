#include "app_flash.hpp"

AppFlash::AppFlash(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppFlash::setup() {
    display->clearDisplay();

    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(2, 2);
    display->write("Press A to go into");
    display->setCursor(2, 10);
    display->write("flash mode.");

    display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display->setCursor(2, 20);
    display->write("This will cause the");
    display->setCursor(2, 30);
    display->write("device to reboot!");

    display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display->setCursor(2, 40);
    display->write("Press B to go back");

    display->display();
}

void AppFlash::loop(ButtonStates btn_states) {
    if(btn_states.B_FALLING_EDGE) {
        handler->exit_current();
        return;
    }

    if(btn_states.A_FALLING_EDGE) {
        display->clearDisplay();
        display->display();
        rp2040.rebootToBootloader();
    }
}

void AppFlash::loop1() {

}

void AppFlash::close() {
    
}