#include "app_morse.hpp"

AppMorse::AppMorse(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppMorse::setup() {
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

void AppMorse::loop(ButtonStates btn_states) {
    if(btn_states.B_FALLING_EDGE) {
        handler->exit_current();
        return;
    }

    // change freq change to left+right so then up and down can be used to switch between
    // changing freq and rssi thresh

    // such logic just to change a single number
    if(btn_states.UP_RISING_EDGE) {
        up_hold_tick = millis();
        frequency+=1;
        require_frequency_confirmation = true;
    }
    if(btn_states.UP) {
        if(millis() > up_hold_tick + 500) {
            frequency+=10; // make it so you have to hit button twice to exit idle animation
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
        frequency-=1;
        require_frequency_confirmation = true;
    }
    if(btn_states.DOWN) {
        if(millis() > down_hold_tick + 500) {
            frequency-=10;
            down_hold_tick = millis();
        }

        if(frequency < 300) frequency = 300;
        if(frequency > 348 && frequency < 387) frequency = 348;
        if(frequency > 464 && frequency < 779) frequency = 464;

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

    currently_transmitting = false;

    if(!require_frequency_confirmation) {
        if(btn_states.A) {
            radio.standby();
            pinMode(RADIO_gd0, OUTPUT);
            digitalWrite(RADIO_gd0, HIGH);
            radio.transmitDirectAsync();

            currently_transmitting = true;
        } 
        if(btn_states.A_FALLING_EDGE) {
            radio.standby();
            pinMode(RADIO_gd0, INPUT);
            radio.receiveDirectAsync();
        }
    }

    display->clearDisplay();

    display->setTextColor(SSD1306_WHITE);
    display->setTextSize(1);
    display->cp437(true);

    float rssi = radiohal::get_RSSI(radio);

    if(rssi > -80.0) {
        display->fillScreen(SSD1306_WHITE);
        display->setTextColor(SSD1306_BLACK);
    } else {
        display->fillScreen(SSD1306_BLACK);
        display->setTextColor(SSD1306_WHITE);
    }

    char text_buffer[32];
    // uint8_t rawRssi = radio.SPIreadRegister(RADIOLIB_CC1101_REG_RSSI);
    sprintf(text_buffer, "RSSI: %.2f", rssi);
    // sprintf(text_buffer, "RSSI: %d", rawRssi);
    display->setCursor(2, 2);
    display->write(text_buffer);
    sprintf(text_buffer, "FREQ: %.2fMHz", frequency);
    display->setCursor(2, 12);
    display->write(text_buffer);

    if(require_frequency_confirmation) {
        display->setCursor(2, 22);
        display->write("press A to confirm freq");
    }
    if(currently_transmitting) {
        display->setCursor(2, 32);
        display->write("transmitting!");
    }

    display->display();
}

void AppMorse::loop1() {
    // if(currently_transmitting) return;
    // last_gd0_read = digitalRead(RADIO_gd0);
    // float rssi = radiohal::get_RSSI(radio);

    // last_gd0_read = (rssi > -100) ? HIGH : LOW;

    // last_rssi = rssi;
}

void AppMorse::close() {
    if(radio.setOutputPower(0) != RADIOLIB_ERR_NONE) {
        Serial.println("error setting output power");
    }

    radio.standby();
}