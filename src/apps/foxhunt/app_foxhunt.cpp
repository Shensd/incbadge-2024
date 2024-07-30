#include "app_foxhunt.hpp"

AppFoxHunt::AppFoxHunt(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppFoxHunt::setup() {
    pinMode(RADIO_gd0, INPUT);

    if(radio.setOOK(true) != RADIOLIB_ERR_NONE) {
        Serial.println("error setting OOK");
    }

    if(radio.setFrequency(frequency) != RADIOLIB_ERR_NONE) {
        Serial.println("error setting frequency");
    }

    if(radio.setOutputPower(10) != RADIOLIB_ERR_NONE) {
        Serial.println("error setting output power");
    }

    if(radio.receiveDirectAsync() != RADIOLIB_ERR_NONE) {
        Serial.println("error setting receive direct async");
    }
    delay(100);

    Serial.println("radio setup.");
}

void AppFoxHunt::loop(ButtonStates btn_states) {
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
    if(btn_states.UP || btn_states.UP_FALLING_EDGE) {
        if(frequency > 348 && frequency < 387) frequency = 387;
        if(frequency > 464 && frequency < 779) frequency = 779;
        if(frequency > 928) frequency = 928;
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
    if(btn_states.DOWN || btn_states.DOWN_FALLING_EDGE) {
        if(frequency < 300) frequency = 300;
        if(frequency > 348 && frequency < 387) frequency = 348;
        if(frequency > 464 && frequency < 779) frequency = 464;
    }

    if(require_frequency_confirmation) {
        if(btn_states.A_FALLING_EDGE) {
            radio.standby();
            radio.setFrequency(frequency);
            radio.receiveDirectAsync();
            require_frequency_confirmation = false;
        }
    }

    display->clearDisplay();

    display->setTextColor(SSD1306_WHITE);
    display->setTextSize(3);
    display->cp437(true);

    float rssi = radiohal::get_RSSI(radio); 

    char text_buffer[32];
    // uint8_t rawRssi = radio.SPIreadRegister(RADIOLIB_CC1101_REG_RSSI);
    sprintf(text_buffer, "%d", (int) rssi);
    // sprintf(text_buffer, "RSSI: %d", rawRssi);
    display->setCursor(2, 2);
    display->write(text_buffer);
    sprintf(text_buffer, "%.3f", frequency);
    display->setCursor(2, 24);
    display->write(text_buffer);

    display->setTextSize(1);

    if(require_frequency_confirmation) {
        display->setCursor(2, 46);
        display->write("A to confirm freq");
    }

    display->display();
}

void AppFoxHunt::loop1() {
    // if(currently_transmitting) return;
    // last_gd0_read = digitalRead(RADIO_gd0);
    // float rssi = radiohal::get_RSSI(radio);

    // last_gd0_read = (rssi > -100) ? HIGH : LOW;

    // last_rssi = rssi;
}

void AppFoxHunt::close() {
    if(radio.setOutputPower(0) != RADIOLIB_ERR_NONE) {
        Serial.println("error setting output power");
    }

    radio.standby();
}