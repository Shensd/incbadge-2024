#include "app_fox.hpp"

AppFox::AppFox(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppFox::setup() {
    pinMode(RADIO_gd0, OUTPUT);
    digitalWrite(RADIO_gd0, HIGH);

    int16_t status = 0;

    if((status = radio.setOOK(true)) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting OOK, %d\n", status);
        handler->exit_current();
        return;
    }

    if((status = radio.setFrequency(frequency)) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting frequency, %d\n", status);
        handler->exit_current();
        return;
    }

    if((status = radio.transmitDirectAsync()) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting receive direct async, %d\n", status);
        handler->exit_current();
        return;
    }
    delay(100);
}

void AppFox::loop_configuration(ButtonStates btn_states) {

    int16_t status = 0;
    if(btn_states.A_FALLING_EDGE) {
        in_configuration_loop = false;
        
        frequency = temp_frequency;
        transmit_delay_ms = temp_transmit_delay_ms;
        transmit_duration_ms = temp_transmit_duration_ms;
        modulation = temp_modulation;

        radio.standby();

        if((status = radio.setFrequency(frequency)) != RADIOLIB_ERR_NONE) {
            Serial.printf("error setting frequency, %d\n", status);
            handler->exit_current();
            return;
        }

        if(modulation == CONFIG_ASK) {
            if((status = radio.setOOK(true)) != RADIOLIB_ERR_NONE) {
                Serial.printf("error setting OOK, %d\n", status);
                handler->exit_current();
                return;
            }
        }
        if(modulation == CONFIG_FSK) {
            if((status = radio.setOOK(false)) != RADIOLIB_ERR_NONE) {
                Serial.printf("error setting ASK (setOOK(false)), %d\n", status);
                handler->exit_current();
                return;     
            }
        }

        if((status = radio.transmitDirectAsync()) != RADIOLIB_ERR_NONE) {
            Serial.printf("error putting radio in transmit direct async, %d\n", status);
            handler->exit_current();
            return;
        }

        return;
    }
    // exit without saving
    if(btn_states.B_FALLING_EDGE) {
        in_configuration_loop = false;
        if((status = radio.transmitDirectAsync()) != RADIOLIB_ERR_NONE) {
            Serial.printf("error putting radio in transmit direct async. %d\n", status);
            handler->exit_current();
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
    case 1:
        if(do_right_large_step) temp_transmit_delay_ms += 10000;
        else if(do_right_medium_step) temp_transmit_delay_ms += 1000;
        else if(do_right_small_step) temp_transmit_delay_ms += 1000;
        else if(do_left_large_step) temp_transmit_delay_ms -= 10000;
        else if(do_left_medium_step) temp_transmit_delay_ms -= 1000;
        else if(do_left_small_step) temp_transmit_delay_ms -= 1000;

        if(temp_transmit_delay_ms < 0) temp_transmit_delay_ms = 0;

        break;
    case 2:
        if(do_right_large_step) temp_transmit_duration_ms += 10000;
        else if(do_right_medium_step) temp_transmit_duration_ms += 1000;
        else if(do_right_small_step) temp_transmit_duration_ms += 1000;
        else if(do_left_large_step) temp_transmit_duration_ms -= 10000;
        else if(do_left_medium_step) temp_transmit_duration_ms -= 1000;
        else if(do_left_small_step) temp_transmit_duration_ms -= 1000;

        if(temp_transmit_duration_ms < 0) temp_transmit_duration_ms = 0;

        break;
    case 3: // ASK or FSK
        if(btn_states.LEFT_RISING_EDGE) temp_modulation = CONFIG_FSK;
        if(btn_states.RIGHT_RISING_EDGE) temp_modulation = CONFIG_ASK;

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

    if(current_configuration_option == 1) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    sprintf(text_buffer, "TX GAP %d", temp_transmit_delay_ms / 1000);
    display->setCursor(2, 20);
    display->write(text_buffer);

    if(current_configuration_option == 2) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    sprintf(text_buffer, "TX DURATION %d", temp_transmit_duration_ms / 1000);
    display->setCursor(2, 30);
    display->write(text_buffer);

    if(current_configuration_option == 3) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    sprintf(text_buffer, "MODULATION %s", temp_modulation == CONFIG_ASK ? "ASK" : "FSK");
    display->setCursor(2, 40);
    display->write(text_buffer);

    display->display();
}

void AppFox::loop(ButtonStates btn_states) {
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
        temp_transmit_delay_ms = transmit_delay_ms;
        temp_transmit_duration_ms = transmit_duration_ms;
        temp_modulation = modulation;

        return;
    }

    display->clearDisplay();

    if(millis() > transmit_timer + transmit_delay_ms &&
        millis() < transmit_timer + transmit_delay_ms + transmit_duration_ms) 
    {
        display->fillScreen(SSD1306_WHITE);
        display->setTextColor(SSD1306_BLACK);

        display->setCursor(2, 30);
        display->write("transmitting!");
    } else {
        display->setTextColor(SSD1306_WHITE);
    }

    display->setTextSize(1);
    display->cp437(true);

    char text_buffer[32];
    display->setCursor(2, 2);
    sprintf(text_buffer, "%s %.3fMHz", modulation == CONFIG_ASK ? "ASK" : "FSK", frequency);
    display->write(text_buffer);
    display->setCursor(2, 10);
    sprintf(text_buffer, "Gap duration: %ds", transmit_delay_ms / 1000);
    display->write(text_buffer);
    display->setCursor(2, 20);
    sprintf(text_buffer, "TX duration: %ds", transmit_duration_ms / 1000);
    display->write(text_buffer);
    // display->write("This app transmits while open!");

    display->display();
}

void AppFox::loop1() {

    if(millis() < transmit_timer + transmit_delay_ms) {
        return;
    }

    if(in_configuration_loop) {
        digitalWrite(RADIO_gd0, LOW);
        return;
    }

    digitalWrite(RADIO_gd0, HIGH);
    delayMicroseconds(100);
    digitalWrite(RADIO_gd0, LOW);
    delayMicroseconds(100);

    if(millis() > transmit_timer + transmit_delay_ms + transmit_duration_ms) {
        transmit_timer = millis();
    }
}

void AppFox::close() {
    int16_t status = 0;

    if((status = radio.finishTransmit()) != RADIOLIB_ERR_NONE) {
        Serial.printf("error putting radio in standby, %d\n", status);
    }
}