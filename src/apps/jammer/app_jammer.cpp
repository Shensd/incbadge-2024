#include "app_jammer.hpp"

AppJammer::AppJammer(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppJammer::setup() {
    if(radio.setOOK(true) != RADIOLIB_ERR_NONE) {
        Serial.println("error setting OOK");
    }

    if(radio.setFrequency(frequency) != RADIOLIB_ERR_NONE) {
        Serial.println("error setting frequency");
    }

    if(radio.setOutputPower(10) != RADIOLIB_ERR_NONE) {
        Serial.println("error setting output power");
    }

    pinMode(RADIO_gd0, OUTPUT);
    digitalWrite(RADIO_gd0, LOW);

    if(radio.transmitDirectAsync() != RADIOLIB_ERR_NONE) {
        Serial.println("error setting transmitDirectAsync");
    }
}

void AppJammer::loop(ButtonStates btn_states) {
    if(btn_states.B_FALLING_EDGE) {
        handler->exit_current();
        return;
    }

    if(btn_states.UP_RISING_EDGE) {
        up_hold_tick = millis();
        frequency+=1;
        require_frequency_confirmation = true;
    }
    if(btn_states.UP) {
        if(millis() > up_hold_tick + 500) {
            frequency+=10;
            up_hold_tick = millis();
        }

        require_frequency_confirmation = true;
    }
    if(btn_states.DOWN_RISING_EDGE) {
        down_hold_tick = millis();
        frequency-=1;
        require_frequency_confirmation = true;
    }
    if(btn_states.DOWN) {
        if(millis() > down_hold_tick + 500) {
            frequency-=10;
            down_hold_tick = millis();
        }

        require_frequency_confirmation = true;
    }

    frequency = radiohal::adjust_frequency(frequency);

    if(require_frequency_confirmation) {
        if(btn_states.A_FALLING_EDGE) {
            radio.finishTransmit();
            radio.setFrequency(frequency);
            radio.transmitDirectAsync();
            require_frequency_confirmation = false;
        }
    }

    if(!require_frequency_confirmation) {
        do_jamming = btn_states.A;
    }

    display->clearDisplay();

    char text_buffer[32];
    sprintf(text_buffer, "FREQ: %.2fMHz", frequency);
    display->setTextColor(SSD1306_WHITE);
    display->setTextSize(1);
    display->cp437(true);
    display->setCursor(2, 2);
    display->write(text_buffer);
    
    display->setCursor(2, 10);
    if(require_frequency_confirmation) {
        display->write("press A to confirm");
    } else {
        display->write("hold A to jam!");
    }

    if(do_jamming) {
        display->setCursor(2, 18);
        display->write("jamming!");

        jam_square_x = random(0, SCREEN_WIDTH);
        jam_square_y = random(0, SCREEN_HEIGHT);
        display->drawRect(jam_square_x, jam_square_y, 16, 16, SSD1306_WHITE);
    }
    
    display->display();
}

void AppJammer::loop1() {
    if(do_jamming) {
        digitalWrite(RADIO_gd0, jammer_status);
        // digitalWrite(RADIO_gd0, HIGH);
        jammer_status = !jammer_status;
        delayMicroseconds(50);
    } else {
        digitalWrite(RADIO_gd0, LOW);
    }
}

void AppJammer::close() {
    if(radio.setOutputPower(0) != RADIOLIB_ERR_NONE) {
        Serial.println("error setting output power");
    }

    if(radio.finishTransmit() != RADIOLIB_ERR_NONE) {
        Serial.println("error putting radio to standby");
    }
}