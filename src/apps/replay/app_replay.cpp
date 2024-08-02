#include "app_replay.hpp"

AppReplay::AppReplay(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppReplay::setup() {

}

void AppReplay::load_selected_sub_file(const AppReplay_NS::SubFile* subfile) {
    radio.standby();

    switch(subfile->preset_index) {
    case AppReplay_NS::CONFIG_MOD_AM270:
        if(radiohal::config_FuriHalSubGhzPresetOok270Async(radio) != RADIOLIB_ERR_NONE) {
            Serial.println("error configuring radio to AM270 config");
            return;
        }
        break;
    case AppReplay_NS::CONFIG_MOD_AM650:
        if(radiohal::config_FuriHalSubGhzPresetOok650Async(radio) != RADIOLIB_ERR_NONE) {
            Serial.println("error configuring radio to AM650 config");
            return;
        }
        break;
    case AppReplay_NS::CONFIG_MOD_FM238:
        if(radiohal::config_FuriHalSubGhzPreset2FSKDev238Async(radio) != RADIOLIB_ERR_NONE) {
            Serial.println("error configuring radio to FM238 config");
            return;
        }
        break;
    case AppReplay_NS::CONFIG_MOD_FM476:
        if(radiohal::config_FuriHalSubGhzPreset2FSKDev476Async(radio) != RADIOLIB_ERR_NONE) {
            Serial.println("error configuring radio to FM476 config");
            return;
        }
        break;
    case AppReplay_NS::CONFIG_MOD_CUSTOM:
        if(radiohal::load_config(radio, subfile->custom_preset_data) != RADIOLIB_ERR_NONE) {
            Serial.println("error loading custom config (uh oh)");
            return;
        }
        break;
    default:
        break;
    }

    if(radio.setFrequency((float)subfile->frequency_hz / 1000000.0) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting radio frequency %dHz", subfile->frequency_hz);
        return;
    }
}

void AppReplay::loop_configuration(ButtonStates btn_states) {

    // exit and save, make sure you make the actual config changes here!
    if(btn_states.A_FALLING_EDGE) {
        in_configuration_loop = false;
        
        sub_file_index = temp_sub_file_index;
        current_subfile = &AppReplay_NS::DEFAULT_SUB_FILES[sub_file_index];

        load_selected_sub_file(current_subfile);

        return;
    }
    // exit without saving
    if(btn_states.B_FALLING_EDGE) {
        in_configuration_loop = false;
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
    case 0: // sub file
        if(do_right_small_step || do_right_medium_step) temp_sub_file_index++;
        else if(do_left_small_step || do_left_medium_step) temp_sub_file_index--;

        if(temp_sub_file_index < 0) temp_sub_file_index = AppReplay_NS::NUM_DEFAULT_SUB_FILES - 1;
        if(temp_sub_file_index >= AppReplay_NS::NUM_DEFAULT_SUB_FILES) temp_sub_file_index = 0;

        break;
    default:    
        break;
    }

    display->clearDisplay();

    display->setTextColor(SSD1306_WHITE);
    display->setTextSize(1);
    display->cp437(true);

    display->setCursor(2, 2);
    display->write("A=set B=cancel");

    if(current_configuration_option == 0) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    // sprintf(text_buffer, "SUB %s", AppReplay_NS::DEFAULT_SUB_FILES[temp_sub_file_index].name);
    display->setCursor(2, 10);
    display->write("SUB:");
    display->setCursor(2, 20);
    display->setTextWrap(false);
    display->write(AppReplay_NS::DEFAULT_SUB_FILES[temp_sub_file_index].name);
    display->setTextWrap(true);

    display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    char buffer[32];
    sprintf(buffer, "FREQ: %.3fMHz", (float)AppReplay_NS::DEFAULT_SUB_FILES[temp_sub_file_index].frequency_hz / 1000000);
    display->setCursor(2, 30);
    display->write(buffer);

    sprintf(buffer, "PRESET:  ");
    switch(AppReplay_NS::DEFAULT_SUB_FILES[temp_sub_file_index].preset_index) {
    case AppReplay_NS::CONFIG_MOD_AM270:
        strcpy(buffer+sizeof("PRESET:"), STR_AM270);
        break;
    case AppReplay_NS::CONFIG_MOD_AM650:
        strcpy(buffer+sizeof("PRESET:"), STR_AM650);
        break;
    case AppReplay_NS::CONFIG_MOD_FM238:
        strcpy(buffer+sizeof("PRESET:"), STR_FM238);
        break;
    case AppReplay_NS::CONFIG_MOD_FM476:
        strcpy(buffer+sizeof("PRESET:"), STR_FM476);
        break;
    case AppReplay_NS::CONFIG_MOD_CUSTOM:
        strcpy(buffer+sizeof("PRESET:"), STR_CUSTOM);
        break;
    }
    display->setCursor(2, 40);
    display->write(buffer);
    
    sprintf(buffer, "# TIMINGS: %d", AppReplay_NS::DEFAULT_SUB_FILES[temp_sub_file_index].num_timings);
    display->setCursor(2, 50);
    display->write(buffer);

    display->display();
}

void AppReplay::loop(ButtonStates btn_states) {
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

        temp_sub_file_index = sub_file_index;

        return;
    }

    do_transmit = btn_states.UP;

    // transmit_once = btn_states.DOWN_FALLING_EDGE;

    if(!do_transmit) {
        radio.finishTransmit();
        digitalWrite(RADIO_gd0, LOW);
    }

    display->clearDisplay();

    display->setTextSize(1);

    if(do_transmit) {
        display->setTextColor(SSD1306_BLACK);
        display->fillScreen(SSD1306_WHITE);
    } else {
         display->setTextColor(SSD1306_WHITE);
    }
   
    display->cp437(true);
    display->setCursor(2, 2);
    char buffer[128];
    // sprintf(buffer, "SUB: %s", current_subfile->name);
    display->write("SUB:");
    display->setCursor(2,10);
    display->setTextWrap(false);
    display->write(current_subfile->name);
    display->setTextWrap(true);

    display->setCursor(2, 30);
    if(do_transmit) {
        display->write("Transmitting!");

        display->fillRect(
            // just use a separate variable and fake the effect
            (((float)transmit_index / (float)current_subfile->num_timings) * (float)SCREEN_WIDTH), 
            SCREEN_HEIGHT - 8, 
            4, 
            8, 
            SSD1306_BLACK);
    } else {
        display->write("Press UP to TX!");
    }

    display->display();
}

void AppReplay::loop1() {
    if(!do_transmit) {
        transmit_index = 0;
        radio_is_setup = false;
        return;
    }
    if(!radio_is_setup) {
        radio.transmitDirectAsync();
        transmit_timer = millis();
        transmit_index = 0;
        radio_is_setup = true;
    }

    if(millis() < transmit_timer + current_subfile->timings[transmit_index]) {
        transmit_timer = millis();
        transmit_index++;

        if(transmit_index > current_subfile->num_timings) {
            transmit_index = 0;
        }

        digitalWrite(RADIO_gd0, transmit_index < 0 ? LOW : HIGH);
    }
}

void AppReplay::close() {
    digitalWrite(RADIO_gd0, LOW);
    radio.finishTransmit();
    radio.standby();
}