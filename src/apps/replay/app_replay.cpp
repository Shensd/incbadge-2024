#include "app_replay.hpp"

AppReplay::AppReplay(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppReplay::setup() {

}

void AppReplay::load_selected_sub_file(const radiohal::SubFile* subfile) {
    int16_t status = 0;

    radio.standby();

    if((status = radiohal::set_config_from_preset_index(radio, subfile->preset_index, subfile->custom_preset_data)) != RADIOLIB_ERR_NONE) {
        Serial.printf("error loading sub file configuration, %d\n", status);
        handler->exit_current_with_error(status);
        return;
    }

    if((status = radio.setFrequency((float)subfile->frequency_hz / 1000000.0)) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting radio frequency %dHz, (%d)\n", subfile->frequency_hz);
        handler->exit_current_with_error(status);
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
    strcpy(
        buffer+sizeof("PRESET:"), 
        radiohal::get_preset_string_from_index(
            AppReplay_NS::DEFAULT_SUB_FILES[temp_sub_file_index].preset_index
        )
    );

    display->setCursor(2, 40);
    display->write(buffer);
    
    sprintf(buffer, "# TIMINGS: %d", AppReplay_NS::DEFAULT_SUB_FILES[temp_sub_file_index].num_timings);
    display->setCursor(2, 50);
    display->write(buffer);

    display->display();
}

void AppReplay::loop(ButtonStates btn_states) {
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

        temp_sub_file_index = sub_file_index;

        return;
    }

    do_transmit = btn_states.UP;

    // transmit_once = btn_states.DOWN_FALLING_EDGE;

    if(!do_transmit) {
        if((status = radio.finishTransmit()) != RADIOLIB_ERR_NONE) {
            Serial.printf("error putting radio in standby, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }
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

        line_animation_offset = (line_animation_offset + 10) % SCREEN_WIDTH;

        display->fillCircle(
            line_animation_offset, 
            SCREEN_HEIGHT - 16, 
            4, 
            SSD1306_BLACK
        );
    } else {
        display->write("Press UP to TX!");
    }

    display->display();
}

void AppReplay::loop1() {
    int16_t status = 0;

    if(!do_transmit) {
        transmit_index = 0;
        radio_is_setup = false;
        return;
    }
    if(!radio_is_setup) {
        if((status = radio.transmitDirectAsync()) != RADIOLIB_ERR_NONE) {
            Serial.printf("error putting radio in transmit direct async, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }
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
    int16_t status = 0;
    
    digitalWrite(RADIO_gd0, LOW);
    
    if((status = radio.finishTransmit()) != RADIOLIB_ERR_NONE) {
        Serial.printf("error putting radio in standby, %d\n", status);
    }
}