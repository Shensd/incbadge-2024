#include "app_foxhunt.hpp"

AppFoxHunt::AppFoxHunt(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppFoxHunt::setup() {
    pinMode(RADIO_gd0, INPUT);

    if(modulation == CONFIG_ASK) {
        if(radio.setOOK(true) != RADIOLIB_ERR_NONE) {
            Serial.println("error setting OOK");
        }
    }
    if(modulation == CONFIG_FSK) {
        if(radio.setOOK(false) != RADIOLIB_ERR_NONE) {
            Serial.println("error setting ASK (setOOK(false))");
        }
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

    // clear out rssi reading array
    uint8_t rssi_reading_array_len = (sizeof(previous_rssi_readings) / sizeof(previous_rssi_readings[0]));
    for(uint8_t i = 0; i < rssi_reading_array_len; i++) {
        previous_rssi_readings[i] = rssi_lower_bound;
    }
}

void AppFoxHunt::loop_configuration(ButtonStates btn_states) {

    // exit and save, make sure you make the actual config changes here!
    if(btn_states.A_FALLING_EDGE) {
        in_configuration_loop = false;
        
        frequency = temp_frequency;
        rssi_upper_bound = temp_rssi_upper_bound;
        rssi_lower_bound = temp_rssi_lower_bound;
        modulation = temp_modulation;

        radio.standby();

        if(radio.setFrequency(frequency) != RADIOLIB_ERR_NONE) {
            Serial.println("error setting frequency");
        }

        if(modulation == CONFIG_ASK) {
            if(radio.setOOK(true) != RADIOLIB_ERR_NONE) {
                Serial.println("error setting OOK");
            }
        }
        if(modulation == CONFIG_FSK) {
            if(radio.setOOK(false) != RADIOLIB_ERR_NONE) {
                Serial.println("error setting ASK (setOOK(false))");
            }
        }

        radio.receiveDirectAsync();

        return;
    }
    // exit without saving
    if(btn_states.B_FALLING_EDGE) {
        in_configuration_loop = false;
        radio.receiveDirectAsync();
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
    float medium_step_delay = 250;
    float large_step_delay = 2000;

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
    case 1: // rssi lower bound
        if(do_right_medium_step) temp_rssi_lower_bound += 10;
        else if(do_right_small_step) temp_rssi_lower_bound += 1;
        else if(do_left_medium_step) temp_rssi_lower_bound -= 10;
        else if(do_left_small_step) temp_rssi_lower_bound -= 1;

        break;
    case 2: // rssi upper bound
        if(do_right_medium_step) temp_rssi_upper_bound += 10;
        else if(do_right_small_step) temp_rssi_upper_bound += 1;
        else if(do_left_medium_step) temp_rssi_upper_bound -= 10;
        else if(do_left_small_step) temp_rssi_upper_bound -= 1;

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

    sprintf(text_buffer, "RSSI LOW %d", temp_rssi_lower_bound);
    display->setCursor(2, 20);
    display->write(text_buffer);

    if(current_configuration_option == 2) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    sprintf(text_buffer, "RSSI HI %d", temp_rssi_upper_bound);
    display->setCursor(2, 30);
    display->write(text_buffer);

    if(current_configuration_option == 3) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    sprintf(text_buffer, "MODULATION %s", temp_modulation == CONFIG_ASK ? "ASK" : "FSK");
    display->setCursor(2, 40);
    display->write(text_buffer);

    display->display();
}

void AppFoxHunt::loop(ButtonStates btn_states) {
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
        temp_rssi_upper_bound = rssi_upper_bound;
        temp_rssi_lower_bound = rssi_lower_bound;

        return;
    }

    display->clearDisplay();

    display->setTextColor(SSD1306_WHITE);
    display->setTextSize(1);
    display->cp437(true);

    float rssi = radiohal::get_RSSI(radio); 

    uint8_t rssi_reading_array_len = (sizeof(previous_rssi_readings) / sizeof(previous_rssi_readings[0]));

    if(millis() > last_rssi_reading + rssi_reading_frequency_ms) {
        float adjusted_rssi = rssi;

        if(rssi < rssi_lower_bound) adjusted_rssi = rssi_lower_bound;
        if(rssi > rssi_upper_bound) adjusted_rssi = rssi_upper_bound;

        previous_rssi_readings[rssi_reading_index] = rssi;
        rssi_reading_index = (rssi_reading_index + 1) % rssi_reading_array_len;

        last_rssi_reading = millis();
    }

    char text_buffer[32];
    // uint8_t rawRssi = radio.SPIreadRegister(RADIOLIB_CC1101_REG_RSSI);
    sprintf(text_buffer, "%s, RSSI %d", modulation == CONFIG_ASK ? "ASK" : "FSK", (int) rssi);
    // sprintf(text_buffer, "RSSI: %d", rawRssi);
    display->setCursor(2, 2);
    display->write(text_buffer);
    sprintf(text_buffer, "%.3fMHz", frequency);
    display->setCursor(2, 10);
    display->write(text_buffer);

    sprintf(text_buffer, "%d", rssi_upper_bound);
    display->setCursor(2, 20);
    display->write(text_buffer);
    sprintf(text_buffer, "%d", rssi_lower_bound);
    display->setCursor(2, 64 - 8);
    display->write(text_buffer);

    for(uint8_t i = 0; i < rssi_reading_array_len; i++) {
        uint32_t height = map((long) previous_rssi_readings[i], rssi_lower_bound, rssi_upper_bound, 0, 48);

        display->fillRect(
            i * (SCREEN_WIDTH / rssi_reading_array_len),
            SCREEN_HEIGHT - height,
            (SCREEN_WIDTH / rssi_reading_array_len),
            SCREEN_HEIGHT,
            SSD1306_WHITE
        );
    }

    display->display();
}

void AppFoxHunt::loop1() {

}

void AppFoxHunt::close() {
    if(radio.standby() != RADIOLIB_ERR_NONE) {
        Serial.println("error putting radio in standby");
    }
}