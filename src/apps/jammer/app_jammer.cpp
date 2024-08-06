#include "app_jammer.hpp"

AppJammer::AppJammer(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppJammer::setup() {
    int16_t status = 0;

    if((status = radio.setOOK(true)) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting OOK, %d\n", status);
        handler->exit_current_with_error(status);
        return;
    }

    if((status = radio.setFrequency(frequency)) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting frequency, %d\n", status);
        handler->exit_current_with_error(status);
        return;
    }

    pinMode(RADIO_gd0, OUTPUT);
    digitalWrite(RADIO_gd0, LOW);

    if((status = radio.transmitDirectAsync()) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting transmitDirectAsync, %d\n", status);
        handler->exit_current_with_error(status);
        return;
    }
}

void AppJammer::loop_configuration(ButtonStates btn_states) {
    int16_t status = 0;

    // exit and save, make sure you make the actual config changes here!
    if(btn_states.A_FALLING_EDGE) {
        in_configuration_loop = false;
        
        frequency = temp_frequency;

        radio.standby();

        if((status = radio.setFrequency(frequency)) != RADIOLIB_ERR_NONE) {
            Serial.printf("error setting frequency, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }

        if((status = radio.transmitDirectAsync()) != RADIOLIB_ERR_NONE) {
            Serial.printf("error putting radio in transmit direct async, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }

        return;
    }
    // exit without saving
    if(btn_states.B_FALLING_EDGE) {
        in_configuration_loop = false;
        if((status = radio.transmitDirectAsync()) != RADIOLIB_ERR_NONE) {
            Serial.printf("error putting radio in transmit direct async, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }
        return;
    }
    
    // choose which configuration option to configure
    if(btn_states.UP_FALLING_EDGE) {
        current_configuration_option--;
        if(current_configuration_option < 0) current_configuration_option = configuration_options - 1;
        return;
    }
    if(btn_states.DOWN_FALLING_EDGE) {
        current_configuration_option = (current_configuration_option + 1) % configuration_options;
        return;
    }

    // for frequency
    float small_freq_step = 0.025;
    float medium_freq_step = 1;
    float large_freq_step = 10;

    // step delay ms
    uint32_t medium_step_delay = 250;
    uint32_t large_step_delay = 2000;

    // should be used in order from large to small
    bool do_right_small_step = false, do_right_medium_step = false, do_right_large_step = false;
    bool do_left_small_step = false, do_left_medium_step = false, do_left_large_step = false;

    // this mess of if statements is to keep track of how long the buttons have been pressed
    // and decides how large of a step can be taken accordingly

    if(btn_states.RIGHT_RISING_EDGE) {
        right_hold_tick = millis();
        right_initial_hold = millis();
    }
    // don't do small step if they release after holding -- very annoying otherwise!
    if(btn_states.RIGHT_FALLING_EDGE && millis() < right_initial_hold + medium_step_delay) {
        do_right_small_step = true;
    }
    if(btn_states.RIGHT) {
        // if time after last tick is greater than medium step requirement, do medium step
        if(millis() > right_hold_tick + medium_step_delay) {
            do_right_medium_step = true;
            if(millis() > right_initial_hold + large_step_delay) do_right_large_step = true;

            right_hold_tick = millis();
        }
    }
    if(btn_states.LEFT_RISING_EDGE) {
        left_hold_tick = millis();
        left_initial_hold = millis();
    }
    // don't do small step if they release after holding -- very annoying otherwise!
    if(btn_states.LEFT_FALLING_EDGE && millis() < left_initial_hold + medium_step_delay) {
        do_left_small_step = true;
    }
    if(btn_states.LEFT) {
        // if time after last tick is greater than medium step requirement, do medium step
        if(millis() > left_hold_tick + medium_step_delay) {
            do_left_medium_step = true;
            if(millis() > left_initial_hold + large_step_delay) do_left_large_step = true;

            left_hold_tick = millis();
        }
    }

    switch(current_configuration_option) {
    case 0: // frequency
        if(do_right_large_step) temp_frequency += large_freq_step;
        else if(do_right_medium_step) temp_frequency += medium_freq_step;
        else if(do_right_small_step) temp_frequency += small_freq_step;
        else if(do_left_large_step) temp_frequency -= large_freq_step;
        else if(do_left_medium_step) temp_frequency -= medium_freq_step;
        else if(do_left_small_step) temp_frequency -= small_freq_step;

        temp_frequency = radiohal::adjust_frequency(temp_frequency);

        break;
    default:    
        break;
    }

    display->clearDisplay();

    display->setTextColor(SSD1306_WHITE);
    display->setTextSize(1);
    display->cp437(true);

    char text_buffer[32];

    display->setCursor(2, 2);
    display->write("A=set B=cancel");

    if(current_configuration_option == 0) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    sprintf(text_buffer, "FREQ %.3f", temp_frequency);
    display->setCursor(2, 10);
    display->write(text_buffer);

    display->display();
}

void AppJammer::loop(ButtonStates btn_states) {
    int16_t status = 0;

    if(in_configuration_loop) {
        loop_configuration(btn_states);
        return;
    }

    if(btn_states.B_FALLING_EDGE) {
        handler->exit_current();
        return;
    }

    if(btn_states.A_FALLING_EDGE) {
        in_configuration_loop = true;

        radio.standby();

        temp_frequency = frequency;

        return;
    }

    // do_jamming = btn_states.UP || btn_states.DOWN || btn_states.LEFT || btn_states.RIGHT;

    if(btn_states.UP_RISING_EDGE || btn_states.DOWN_RISING_EDGE || btn_states.LEFT_RISING_EDGE || btn_states.RIGHT_RISING_EDGE) {
        if((status = radio.transmitDirectAsync()) != RADIOLIB_ERR_NONE) {
            Serial.printf("error putting radio in transmit direct async, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }
        delay(50);
        do_jamming = true;
    }  else if(btn_states.UP_FALLING_EDGE || btn_states.DOWN_FALLING_EDGE || btn_states.LEFT_FALLING_EDGE || btn_states.RIGHT_FALLING_EDGE) {
        do_jamming = false;
        delay(50);
        if((status = radio.finishTransmit()) != RADIOLIB_ERR_NONE) {
            Serial.printf("error putting radio in standby, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }
    }

    display->clearDisplay();

    if(do_jamming) {
        display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display->fillScreen(SSD1306_WHITE);
    } else {
        display->setTextColor(SSD1306_WHITE);
    }

    display->setTextSize(1);
    display->cp437(true);
    
    if(do_jamming) {
        for(int i = 0; i < 16; i++) {
            display->drawRect(random(0, SCREEN_WIDTH - 16), random(0, SCREEN_HEIGHT - 16), 16, 16, SSD1306_BLACK);
        }
    }

    char text_buffer[32];
    sprintf(text_buffer, "FREQ: %.3fMHz", frequency);
    display->setCursor(2, 2);
    display->write(text_buffer);
    display->setCursor(2, 10);
    display->write("Hold any directional");
    display->setCursor(2, 20);
    display->write("to transmit!");
    
    display->display();
}

void AppJammer::loop1() {
    if(do_jamming) {
        digitalWrite(RADIO_gd0, HIGH);
        delayMicroseconds(100);
        digitalWrite(RADIO_gd0, LOW);
        delayMicroseconds(100);
    } else {
        digitalWrite(RADIO_gd0, LOW);
    }
}

void AppJammer::close() {
    int16_t status = 0;

    if((status = radio.finishTransmit()) != RADIOLIB_ERR_NONE) {
        Serial.printf("error putting radio in standby, %d\n", status);
    }
}