#include "app_mouse.hpp"

AppMouse::AppMouse(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

namespace AppMouse_NS {
    // uint8_t buffer[] = {0, 0, 0, 0};
    // bool buffer_filled = false;

    bool packet_available = false;

    CC1101* radio;
}

void AppMouse::setup() {
    int16_t status = 0;

    AppMouse_NS::radio = &radio;

    // Mouse.begin();

    radio.reset(); 

    if((status = radio.setFrequency(frequency)) != RADIOLIB_ERR_NONE) {
        // Mouse.end();
        Serial.printf("error setting frequency, %d\n", status);
        handler->exit_current();
        return;
    }

    if((status = radio.setNodeAddress(id, 1)) != RADIOLIB_ERR_NONE) {
        // Mouse.end();
        Serial.printf("error setting node id, %d\n", status);
        handler->exit_current();
        return;
    }

    if((status = radio.setCrcFiltering(true)) != RADIOLIB_ERR_NONE) {
        Serial.printf("error enabling crc filtering, %d\n", status);
        handler->exit_current();
        return;
    }

    if(do_receive) {
        radio.setPacketReceivedAction(AppMouse_NS::receive_cb);

        if((status = radio.startReceive()) != RADIOLIB_ERR_NONE) {
            // Mouse.end();
            Serial.printf("error starting receive, %d\n", status);
            handler->exit_current();
            return;
        }
    } else {
        radio.clearPacketReceivedAction();
        radio.standby();
    }
}

void AppMouse_NS::receive_cb(void) {
    packet_available = true;
}

void AppMouse::loop_configuration(ButtonStates btn_states) {
    int16_t status = 0;

    // exit and save, make sure you make the actual config changes here!
    if(btn_states.A_FALLING_EDGE) {
        in_configuration_loop = false;
        
        frequency = temp_frequency;
        id = temp_id;
        do_receive = temp_do_receive;

        radio.standby();

        if((status = radio.setFrequency(frequency)) != RADIOLIB_ERR_NONE) {
            // Mouse.end();
            Serial.printf("error setting frequency, %d\n", status);
            handler->exit_current();
            return;
        }

        if((status = radio.setNodeAddress(id, 1)) != RADIOLIB_ERR_NONE) {
            // Mouse.end();
            Serial.printf("error setting node id, %d\n", status);
            handler->exit_current();
            return;
        }

        if(do_receive) {
            radio.setPacketReceivedAction(AppMouse_NS::receive_cb);

            if((status = radio.startReceive()) != RADIOLIB_ERR_NONE) {
                // Mouse.end();
                Serial.printf("error starting receive, %d\n", status);
                handler->exit_current();
                return;
            }
        } else {
            radio.clearPacketReceivedAction();
            radio.standby();
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
    case 0: // frequency
        if(do_right_large_step) temp_frequency += large_freq_step;
        else if(do_right_medium_step) temp_frequency += medium_freq_step;
        else if(do_right_small_step) temp_frequency += small_freq_step;
        else if(do_left_large_step) temp_frequency -= large_freq_step;
        else if(do_left_medium_step) temp_frequency -= medium_freq_step;
        else if(do_left_small_step) temp_frequency -= small_freq_step;

        temp_frequency = radiohal::adjust_frequency(temp_frequency);

        break;
    case 1: // id
        if(do_right_small_step || do_right_medium_step) temp_id++;
        if(do_left_small_step || do_left_medium_step) temp_id--;

        if(temp_id < 0) temp_id = 0;

        break;
    case 2: // do_receive
        if(do_right_small_step || do_left_small_step) temp_do_receive = !temp_do_receive;
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
    sprintf(buffer, "FREQ: %.3f", temp_frequency);
    display->write(buffer);
    
    if(current_configuration_option == 1) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    display->setCursor(2, 20);
    sprintf(buffer, "ID: %d", temp_id);
    display->write(buffer);

    if(current_configuration_option == 2) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    display->setCursor(2, 30);
    sprintf(buffer, "MODE: %s", temp_do_receive ? "RECEIVE" : "TRANSMIT");
    display->write(buffer);

    display->display();
}

void AppMouse::start_receive_mode() {
    if(receive_mode_started) return;

    receive_mode_started = true;
}

void AppMouse::start_transmit_mode() {
    if(transmit_mode_started) return;

    transmit_mode_started = true;
}

void AppMouse::loop(ButtonStates btn_states) {
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
        temp_id = id;
        temp_do_receive = do_receive;
        
        return;
    }

    if(do_receive) {
        // if(btn_states.UP) {
        //     Mouse.move(0, -10);
        // } else if(btn_states.DOWN) {
        //     Mouse.move(0, 10);
        // } else if(btn_states.LEFT) {
        //     Mouse.move(-10, 0);
        // } else if(btn_states.RIGHT) {
        //     Mouse.move(10, 0);
        // }

        if(AppMouse_NS::packet_available) {
            radio.readData(packet_buffer, sizeof(packet_buffer));

            Serial.printf("MOVE %d, %d\n", (int8_t)packet_buffer[0], (int8_t)packet_buffer[1]);

            AppMouse_NS::packet_available = false;

            radio.startReceive();
        }
    } else {
        bool do_transmit = false;

        uint8_t data[3] = {0, 0, 0};

        if(btn_states.UP) {
            data[1] = -10;
            do_transmit = true;
        } 
        if(btn_states.DOWN) {
            data[1] = 10;
            do_transmit = true;
        } 
        if(btn_states.LEFT) {
            data[0] = -10;
            do_transmit = true;
        } 
        if(btn_states.RIGHT) {
            data[0] = 10;
            do_transmit = true;
        }

        if(do_transmit) {
            if((status =  radio.transmit(data, 3, id)) != RADIOLIB_ERR_NONE) {
                Serial.printf("error transmitting, %d\n", status);
                handler->exit_current();
                return;
            }
        }
    }

    display->clearDisplay();

    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->cp437(true);
    display->setCursor(2, 2);
    display->write("MOUSE");

    display->setCursor(2, 10);
    char buffer[32];
    sprintf(buffer, "last command: %d, %d", (int8_t)buffer[0], (int8_t)buffer[1]);

    display->display();
}

void AppMouse::loop1() {

}

void AppMouse::close() {
    // Mouse.end();
}