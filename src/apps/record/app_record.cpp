#include "app_record.hpp"

AppRecord::AppRecord(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppRecord::setup() {
    int16_t status = 0;

    for(int i = 0; i < sizeof(previous_rssi_readings) / sizeof(previous_rssi_readings[0]); i++) {
        previous_rssi_readings[i] = rssi_lower_bound;
    }

    if((status = radio.setOOK(use_ook)) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting OOK mode, %d\n", status);
        handler->exit_current_with_error(status);
        return;
    }
    if((status = radio.setFrequency(frequency)) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting frequency, %d\n", status);
        handler->exit_current_with_error(status);
        return;
    }
    if((status = radio.setRxBandwidth(available_bandwidths[rx_bandwidth_index])) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting rx bandwidth, %d\n", status);
        handler->exit_current_with_error(status);
        return;
    }
    if((status = radio.setFrequencyDeviation(fsk_deviation)) != RADIOLIB_ERR_NONE) {
        Serial.printf("error setting frequency deviation, %d\n", status);
        handler->exit_current_with_error(status);
        return;
    }
   
}


void AppRecord::loop_configuration(ButtonStates btn_states) {
    int16_t status = 0;

    // exit and save, make sure you make the actual config changes here!
    if(btn_states.A_FALLING_EDGE) {
        in_configuration_loop = false;
        
        use_ook = temp_use_ook;
        frequency = temp_frequency;
        rx_bandwidth_index = temp_rx_bandwidth_index;
        fsk_deviation = temp_fsk_deviation;

        if((status = radio.setOOK(use_ook)) != RADIOLIB_ERR_NONE) {
            Serial.printf("error setting OOK mode, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }
        if((status = radio.setFrequency(frequency)) != RADIOLIB_ERR_NONE) {
            Serial.printf("error setting frequency, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }
        if((status = radio.setRxBandwidth(available_bandwidths[rx_bandwidth_index])) != RADIOLIB_ERR_NONE) {
            Serial.printf("error setting rx bandwidth, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }
        if((status = radio.setFrequencyDeviation(fsk_deviation)) != RADIOLIB_ERR_NONE) {
            Serial.printf("error setting frequency deviation, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }

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

    // preset, frequency, modulation, bandwidth

    switch(current_configuration_option) {
    case 0: // preset
        if(do_right_small_step) temp_preset_index++;
        else if(do_left_small_step) temp_preset_index--;

        if(temp_preset_index < 0) temp_preset_index = num_presets - 1;
        else if(temp_preset_index >= num_presets) temp_preset_index = 0;

        switch(temp_preset_index) {
        case radiohal::CONFIG_PRESET_AM270:
            temp_use_ook = true;
            temp_rx_bandwidth_index = 9;
            break;
        case radiohal::CONFIG_PRESET_AM650:
            temp_use_ook = true;
            temp_rx_bandwidth_index = 14;
            break;
        case radiohal::CONFIG_PRESET_FM238:
            temp_use_ook = false;
            temp_fsk_deviation = 238;
            break;
        case radiohal::CONFIG_PRESET_FM476:
            temp_use_ook = false;
            temp_fsk_deviation = 476;
            break;
        }

        break;
    case 1: // frequency
        if(do_right_large_step) temp_frequency += large_freq_step;
        else if(do_right_medium_step) temp_frequency += medium_freq_step;
        else if(do_right_small_step) temp_frequency += small_freq_step;
        else if(do_left_large_step) temp_frequency -= large_freq_step;
        else if(do_left_medium_step) temp_frequency -= medium_freq_step;
        else if(do_left_small_step) temp_frequency -= small_freq_step;

        temp_frequency = radiohal::adjust_frequency(temp_frequency);

        break;
    case 2: // modulation
        if(do_right_small_step || do_left_small_step) temp_use_ook = !temp_use_ook;

        break;
    case 3: // bandwidth 
        if(do_right_small_step) temp_rx_bandwidth_index++;
        else if(do_left_small_step) temp_rx_bandwidth_index--;

        if(temp_rx_bandwidth_index < 0) temp_rx_bandwidth_index = sizeof(available_bandwidths) / sizeof(available_bandwidths[0]);
        if(temp_rx_bandwidth_index >= sizeof(available_bandwidths) / sizeof(available_bandwidths[0])) temp_rx_bandwidth_index = 0;

        break;
    case 4: // deviation
        if(do_right_large_step) temp_fsk_deviation += large_freq_step;
        else if(do_right_medium_step) temp_fsk_deviation += medium_freq_step;
        else if(do_right_small_step) temp_fsk_deviation += small_freq_step;
        else if(do_left_large_step) temp_fsk_deviation -= large_freq_step;
        else if(do_left_medium_step) temp_fsk_deviation -= medium_freq_step;
        else if(do_left_small_step) temp_fsk_deviation -= small_freq_step;

        if(temp_fsk_deviation < 1.587) temp_fsk_deviation = 1.587;
        if(temp_fsk_deviation > 380.8) temp_fsk_deviation = 380.8;
    default:    
        break;
    }

    display->clearDisplay();

    display->setTextColor(SSD1306_WHITE);
    display->setTextSize(1);
    display->cp437(true);

    display->setCursor(2, 2);
    display->write("A=set B=cancel");

    char buffer[32];

    if(current_configuration_option == 0) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    
    display->setCursor(2, 10);
    sprintf(buffer, "PRESET: ");
    strcpy(
        buffer+sizeof("PRESET:"), 
        radiohal::get_preset_string_from_index(
            temp_preset_index
        )
    );
    display->write(buffer);
    
    if(current_configuration_option == 1) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    display->setCursor(2, 20);
    sprintf(buffer, "FREQ: %.3f", temp_frequency);
    display->write(buffer);

    if(current_configuration_option == 2) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    display->setCursor(2, 30);
    sprintf(buffer, "MOD: %s", temp_use_ook ? "ASK" : "FSK");
    display->write(buffer);

    if(current_configuration_option == 3) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    display->setCursor(2, 40);
    sprintf(buffer, "BW: %d", available_bandwidths[temp_rx_bandwidth_index]);
    display->write(buffer);

    if(current_configuration_option == 4) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    display->setCursor(2, 50);
    sprintf(buffer, "DEV: %.3f", temp_fsk_deviation);
    display->write(buffer);

    display->display();
}

void AppRecord::dump_recording_over_serial() {
    Serial.println("Filetype: Flipper SubGhz RAW File");
    Serial.println("Version: 1");
    Serial.printf("Frequency: %d\n", (uint64_t)(frequency * 1000000));
    switch(temp_preset_index) {
    case 0:
        Serial.println("Preset: FuriHalSubGhzPresetOok270Async");
        break;
    case 1:
        Serial.println("Preset: FuriHalSubGhzPresetOok650Async");
        break;
    case 2:
        Serial.println("Preset: FuriHalSubGhzPreset2FSKDev238Async");
        break;
    case 3:
        Serial.println("Preset: FuriHalSubGhzPreset2FSKDev476Async");
        break;
    }
    Serial.print("Protocol: RAW");
    for(int i = 0; i < timings_index[timings_slot]; i++) {
        if(i % 512 == 0) {
            Serial.print("\nRAW_Data: ");
        }
        Serial.printf("%d ", timings[timings_slot][i]);
    }

    Serial.println("");
}

void AppRecord::loop(ButtonStates btn_states) {
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

        temp_use_ook = use_ook;
        temp_frequency = frequency;
        temp_rx_bandwidth_index = rx_bandwidth_index;
        temp_fsk_deviation = fsk_deviation;

        return;
    }

    if(btn_states.DOWN_RISING_EDGE) {
        if((status = radio.standby()) != RADIOLIB_ERR_NONE) {
            Serial.printf("error putting radio in standby, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }

        pinMode(RADIO_gd0, INPUT);
        tick_timer = micros();
        last_bit_time = micros();
        last_bit_state = HIGH;
        timings_index[timings_slot] = 0;

        if((status = radio.receiveDirectAsync()) != RADIOLIB_ERR_NONE) {
            Serial.printf("error setting receive direct async, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }

        do_record = true;
    } else if(btn_states.UP_RISING_EDGE) {
        dump_recording_over_serial();

        if((status = radio.standby()) != RADIOLIB_ERR_NONE) {
            Serial.printf("error putting radio in standby, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }

        pinMode(RADIO_gd0, OUTPUT);
        digitalWrite(RADIO_gd0, LOW);

        if((status = radio.transmitDirectAsync()) != RADIOLIB_ERR_NONE) {
            Serial.printf("error setting transmit direct async, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }

        do_replay = true;
    }

    if(!btn_states.UP) {
        do_replay = false;
    }
    if(!btn_states.DOWN) {
        do_record = false;
    }

    if(!do_record && !do_replay) {
        if((status = radio.standby()) != RADIOLIB_ERR_NONE) {
            Serial.printf("error putting radio in standby, %d\n", status);
            handler->exit_current_with_error(status);
            return;
        }

        if(btn_states.RIGHT_FALLING_EDGE) {
            timings_slot++;
        }
        if(btn_states.LEFT_FALLING_EDGE) {
            timings_slot--;
        }

        if(timings_slot < 0) timings_slot = APPRECORD_NUM_TIMINGS_SLOTS - 1;
        if(timings_slot >= APPRECORD_NUM_TIMINGS_SLOTS) timings_slot = 0;
    }

    display->clearDisplay();

    display->setTextSize(1);
    if(do_replay) {
        display->fillScreen(SSD1306_WHITE);
        display->setTextColor(SSD1306_BLACK);
    }
    else {
        display->setTextColor(SSD1306_WHITE);
    }
    display->cp437(true);
    display->setCursor(2, 2);
    display->write("UP=replay DOWN=record");

    display->setCursor(2, 10);
    char buffer[32];
    sprintf(buffer, "Timings: %d", timings_index[timings_slot]);
    display->write(buffer);

    display->setCursor(2, 20);
    sprintf(buffer, "Save slot: %d", timings_slot + 1);
    display->write(buffer);

    display->setCursor(2, 30);
    if(do_record) {
        display->write("Recording!");
    } else if(do_replay) {
        display->write("Transmitting!");
    }

    uint8_t rssi_reading_array_len = (sizeof(previous_rssi_readings) / sizeof(previous_rssi_readings[0]));

    if(do_record) {
        float rssi = radiohal::get_RSSI(radio); 

        if(millis() > last_rssi_reading + rssi_reading_frequency_ms) {
            float adjusted_rssi = rssi;

            if(rssi < rssi_lower_bound) adjusted_rssi = rssi_lower_bound;
            if(rssi > rssi_upper_bound) adjusted_rssi = rssi_upper_bound;

            previous_rssi_readings[rssi_reading_index] = rssi;
            rssi_reading_index = (rssi_reading_index + 1) % rssi_reading_array_len;

            last_rssi_reading = millis();
        }
    }

    for(uint8_t i = 0; i < rssi_reading_array_len; i++) {
        uint32_t height = map((long) previous_rssi_readings[i], rssi_lower_bound, rssi_upper_bound, 0, 32);

        display->fillRect(
            i * (SCREEN_WIDTH / rssi_reading_array_len),
            SCREEN_HEIGHT - height,
            (SCREEN_WIDTH / rssi_reading_array_len),
            SCREEN_HEIGHT,
            SSD1306_WHITE
        );
    }

    if(do_record) {
        display->fillRect(
            rssi_reading_index * (SCREEN_WIDTH / rssi_reading_array_len),
            SCREEN_HEIGHT - 32,
            1,
            SCREEN_HEIGHT,
            SSD1306_WHITE
        );
    }

    // display->setCursor(2, SCREEN_HEIGHT - 8);
    // display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    // display->write("a=settings");

    display->display();
}

void AppRecord::loop1() {
    if(do_record) {
        if(timings_index[timings_slot] >= sizeof(timings[timings_slot]) / sizeof(timings[timings_slot][0])) {
            return;
        }

        bool reading = digitalRead(RADIO_gd0);

        if(reading == last_bit_state) return;

        timings[timings_slot][timings_index[timings_slot]] = (last_bit_state == 0 ? -1 : 1) * (micros() - last_bit_time);

        last_bit_state = reading;
        last_bit_time = micros();
        timings_index[timings_slot]++;
    }
    if(do_replay) {
        for(int i = 0; i < timings_index[timings_slot]; i++) {
            if(!do_replay) break;

            if(timings[timings_slot][i] < 0) digitalWrite(RADIO_gd0, LOW);
            else digitalWrite(RADIO_gd0, HIGH);

            delayMicroseconds(abs(timings[timings_slot][i]));
        }
    }
}

void AppRecord::close() {
    int16_t status = 0;

    do_record = false;
    do_replay = false;

    if((status = radio.finishTransmit()) != RADIOLIB_ERR_NONE) {
        Serial.printf("error putting radio in standby, %d\n", status);
    }
}