#include "app_scanner.hpp"

AppScanner::AppScanner(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppScanner::setup() {

}

void AppScanner::loop(ButtonStates btn_states) {
    frequency_start = radiohal::adjust_frequency(frequency_start);
    frequency_end = radiohal::adjust_frequency(frequency_end);
}

void AppScanner::loop1() {

}

void AppScanner::close() {
    
}