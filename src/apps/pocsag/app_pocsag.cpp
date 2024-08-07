#include "app_pocsag.hpp"

AppPOCSAG::AppPOCSAG(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {
    for(int i = 0; i < sizeof(read_buffer) / sizeof(read_buffer[0]); i++) {
        read_buffer[i] = 0;
    }
}

void AppPOCSAG::setup() {
    if(pager == NULL) {
        pager = new PagerClient(&radio);
    }

    int16_t status = 0;

    pinMode(RADIO_gd0, INPUT);
    pinMode(RADIO_gd2, INPUT);

    radio.standby();

    radio.reset();

    radio.begin();

    // if((status = radio.setOOK(false)) != RADIOLIB_ERR_NONE) {
    //     Serial.printf("error putting radio in fsk, %d", status);
    //     handler->exit_current();
    //     return;
    // }

    // if((status = radio.receiveDirectAsync()) != RADIOLIB_ERR_NONE) {
    //     Serial.printf("error putting radio in receive direct, %d", status);
    //     handler->exit_current();
    //     return;
    // }

    if((status = pager->begin(frequency, available_speeds[speed_index])) != RADIOLIB_ERR_NONE) {
        Serial.printf("error starting pager listener, %d\n", status);
        handler->exit_current();
        return;
    }

    if((status = pager->startReceive(RADIO_gd2, 1234567)) != RADIOLIB_ERR_NONE) { // mask of all zeros to receive everything
        Serial.printf("error starting pager receive, %d\n", status);
        handler->exit_current();
        return;
    }
}

void AppPOCSAG::loop(ButtonStates btn_states) {
    if(btn_states.B_FALLING_EDGE) {
        handler->exit_current();
        return;
    }
    
    if(btn_states.UP_RISING_EDGE)  {
        pager->transmit("baba", 1234567);
    }

    display->clearDisplay();

    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->cp437(true);
    display->setCursor(2, 2);

    display->write("433MHz/1200");

    size_t batches_available = pager->available();

    bytes_read = sizeof(read_buffer) / sizeof(read_buffer[0]);

    display->setCursor(2, 10);

    for(int i = 0; i < batches_available; i++) {
        pager->readData(
            read_buffer, 
            &bytes_read,
            &received_addr
        );

        if(bytes_read >= sizeof(read_buffer) / sizeof(read_buffer[0])) 
            bytes_read = (sizeof(read_buffer) / sizeof(read_buffer[0])) - 1;

        read_buffer[bytes_read] = 0;

        Serial.println("data received");
        Serial.println((char*) read_buffer);
    }

    snprintf(
        display_buffer, 
        sizeof(display_buffer) / sizeof(display_buffer[0]), 
        "%x: %s", 
        received_addr, 
        read_buffer);
    
    display->write(display_buffer);

    display->display();
}

void AppPOCSAG::loop1() {

}

void AppPOCSAG::close() {
    
}