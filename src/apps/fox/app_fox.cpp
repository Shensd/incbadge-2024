#include "app_fox.hpp"

AppFox::AppFox(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppFox::setup() {
    pinMode(RADIO_gd0, OUTPUT);
    digitalWrite(RADIO_gd0, HIGH);

    if(radio.setOOK(true) != RADIOLIB_ERR_NONE) {
        Serial.println("error setting OOK");
    }

    if(radio.setFrequency(frequency) != RADIOLIB_ERR_NONE) {
        Serial.println("error setting frequency");
    }

    if(radio.setOutputPower(10) != RADIOLIB_ERR_NONE) {
        Serial.println("error setting output power");
    }

    if(radio.transmitDirectAsync() != RADIOLIB_ERR_NONE) {
        Serial.println("error setting receive direct async");
    }
    delay(100);

    Serial.println("radio setup.");
}

void AppFox::loop(ButtonStates btn_states) {
    if(btn_states.B_FALLING_EDGE) {
        handler->exit_current();
        return;
    }

    // change freq change to left+right so then up and down can be used to switch between
    // changing freq and rssi threshB

    float small_freq_step = 0.025;
    float large_freq_step = 1;
    float large_large_freq_step = 10;
    float large_step_delay = 250;
    float large_large_step_delay = 2000;

    // such logic just to change a single number
    if(btn_states.UP_RISING_EDGE) {
        up_hold_tick = millis();
        up_initial_hold = millis();
        frequency+=small_freq_step;
        require_frequency_confirmation = true;
    }
    if(btn_states.UP) {
        if(millis() > up_initial_hold + large_large_step_delay && millis() > up_hold_tick + large_step_delay) {
            frequency+=large_large_freq_step; // make it so you have to hit button twice to exit idle animation
            up_hold_tick = millis();
        } else if(millis() > up_hold_tick + large_step_delay) {
            frequency+=large_freq_step; // make it so you have to hit button twice to exit idle animation
            up_hold_tick = millis();
        }

        require_frequency_confirmation = true;
    }

    if(btn_states.DOWN_RISING_EDGE) {
        down_hold_tick = millis();
        down_initial_hold = millis();
        frequency-=small_freq_step;
        require_frequency_confirmation = true;
    }
    if(btn_states.DOWN) {
        if(millis() > down_initial_hold + large_large_step_delay && millis() > down_hold_tick + large_step_delay) {
            frequency-=large_large_freq_step;
            down_hold_tick = millis();
        } else if(millis() > down_hold_tick + large_step_delay) {
            frequency-=large_freq_step;
            down_hold_tick = millis();
        }
        require_frequency_confirmation = true;
    }

    frequency = radiohal::adjust_frequency(frequency);

    if(require_frequency_confirmation) {
        if(btn_states.A_FALLING_EDGE) {
            radio.standby();
            radio.setFrequency(frequency);
            radio.transmitDirectAsync();
            require_frequency_confirmation = false;
        }
    }

    display->clearDisplay();

    display->setTextColor(SSD1306_WHITE);
    display->setTextSize(1);
    display->cp437(true);

    char text_buffer[32];
    display->setCursor(2, 2);
    sprintf(text_buffer, "%.3f", frequency);
    display->write(text_buffer);
    display->setCursor(2, 10);
    display->write("This app transmits while open!");

    if(require_frequency_confirmation) {
        display->setCursor(2, 20);
        display->write("A to confirm freq");
    }

    display->display();
}

void AppFox::loop1() {
    // if(currently_transmitting) return;
    // last_gd0_read = digitalRead(RADIO_gd0);
    // float rssi = radiohal::get_RSSI(radio);

    // last_gd0_read = (rssi > -100) ? HIGH : LOW;

    // last_rssi = rssi;
    if(require_frequency_confirmation) {
        digitalWrite(RADIO_gd0, LOW);
        return;
    }

    digitalWrite(RADIO_gd0, HIGH);
    delayMicroseconds(100);
    digitalWrite(RADIO_gd0, LOW);
    delayMicroseconds(100);
}

void AppFox::close() {
    if(radio.setOutputPower(0) != RADIOLIB_ERR_NONE) {
        Serial.println("error setting output power");
    }

    radio.standby();
}