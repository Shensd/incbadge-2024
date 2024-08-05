#include "app_scanner.hpp"

AppScanner::AppScanner(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppScanner::setup() {
    int16_t status = 0;

    rp2040.idleOtherCore();

    if((status = radio.setOOK(true)) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting OOK mode, %d\n", status);
        handler->exit_current();
        return;
    }
    if((status = radio.setFrequency(scanner_frequency_list[frequency_index])) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting frequency, %d\n", status);
        handler->exit_current();
        return;
    }
    if((status = radio.setRxBandwidth(rx_bw)) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting rx bandwidth, %d\n", status);
        handler->exit_current();
        return;
    }
    if((status = radio.receiveDirectAsync()) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting receive direct async, %d\n", status);
        handler->exit_current();
        return;
    }

    currently_scanning = false;
}

void AppScanner::step_frequency_index() {
    frequency_index = (frequency_index + 1) % num_scanner_frequencies;
}

void AppScanner::loop(ButtonStates btn_states) {
    int16_t status = 0;

    if(btn_states.B_FALLING_EDGE) {
        handler->exit_current();
        return;
    }

    if(btn_states.UP_FALLING_EDGE) {
        if(!currently_scanning) {
            activity_on_frequency = false;
        }

        currently_scanning = !currently_scanning;
        scanner_timer = millis();

        step_frequency_index();
    }

    if(currently_scanning) {
        if(millis() > scanner_timer + time_at_frequency_ms) {
            activity_on_frequency = false;

            step_frequency_index();

            if((status = radio.standby()) != RADIOLIB_ERR_NONE) {
                Serial.printf("error putting radio in standby, %d\n", status);
                handler->exit_current();
                return;
            }

            if(radio.setFrequency(scanner_frequency_list[frequency_index]) != RADIOLIB_ERR_NONE) {
                Serial.printf("error setting frequency %.03f\n", scanner_frequency_list[frequency_index]);
            }

            if((status = radio.receiveDirectAsync()) != RADIOLIB_ERR_NONE) {
                Serial.printf("error setting receive direct async, %d\n", status);
                handler->exit_current();
                return;
            }

            scanner_timer = millis();
        } else {
            rssi = radiohal::get_RSSI(radio);
        }
    }

    display->clearDisplay();

    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->cp437(true);
    display->setCursor(2, 2);
    if(currently_scanning) {
        display->write("UP=stop");
    } else {
        display->write("UP=start");
    }

    display->setCursor(2, 10);
    char buffer[32];
    sprintf(buffer, "FREQ: %.03fMHz", scanner_frequency_list[frequency_index]);
    display->write(buffer);

    display->setCursor(2, 20);
    sprintf(buffer, "RX BW: %dkHz", rx_bw);
    display->write(buffer);

    display->setCursor(2, 30);
    sprintf(buffer, "RSSI: %.02f", rssi);
    display->write(buffer);

    display->setCursor(2, 40);
    sprintf(buffer, "RSSI THRESH: %.02f", rssi_thresh);
    display->write(buffer);

    if(currently_scanning) {
        if(rssi > rssi_thresh) {
            activity_on_frequency = true; // set false by frequency stepper
            // so it doesn't give someone a seizure 
            freq_index_with_activity = frequency_index;
            currently_scanning = false;
        } 
    }

    if(activity_on_frequency) {
        display->setCursor(2, 50);
        display->write("Caught activity!");
    }

    display->invertDisplay(activity_on_frequency);

    display->display();
}

void AppScanner::loop1() {
    // second core idle for this application
}

void AppScanner::close() {
    int16_t status = 0;

    rp2040.resumeOtherCore();

    if((status = radio.standby()) != RADIOLIB_ERR_NONE) {
        Serial.printf("error putting radio in standby, %d\n", status);
    }
}