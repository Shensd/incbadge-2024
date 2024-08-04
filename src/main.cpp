#include <RadioLib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SPI.h>
#include <stdint.h>

#define DO_SERIAL_DEBUG_INFO 1

#include "pins.hpp"
#include "buttons.hpp"
#include "app.hpp"

#include "apps/idle/app_idle.hpp"
#include "apps/morse/app_morse.hpp"
#include "apps/jammer/app_jammer.hpp"
#include "apps/foxhunt/app_foxhunt.hpp"
#include "apps/pixelpainter/app_pixelpainter.hpp"
#include "apps/fox/app_fox.hpp"
#include "apps/readme/app_readme.hpp"
#include "apps/replay/app_replay.hpp"
#include "apps/usb/app_usb.hpp"
#include "apps/record/app_record.hpp"

pin_size_t button_pins[] = {BTN_PIN_UP, BTN_PIN_DOWN, BTN_PIN_LEFT, BTN_PIN_RIGHT, BTN_PIN_A, BTN_PIN_B};

Adafruit_SSD1306* display = NULL;
CC1101 radio = NULL;

// init display
int init_display(int width, int height, int address) {
    display = new Adafruit_SSD1306(width, height, &Wire, -1);

    if(!display->begin(SSD1306_SWITCHCAPVCC, address)) {
        return 0;
    }

    return 1;
}

int init_radio() { // cc1101 already a pointer
    radio = new Module(RADIO_CS, RADIO_gd0, RADIOLIB_NC, RADIO_gd2);

    return radio.begin() == RADIOLIB_ERR_NONE;
}

AppHandler* handler;

void setup() {
    Serial.begin(115200);

    Wire.setSDA(SCREEN_SDA);
    Wire.setSCL(SCREEN_SCL);

    SPI.setTX(RADIO_MOSI);
    SPI.setRX(RADIO_MISO);
    SPI.setCS(RADIO_CS);
    SPI.setSCK(RADIO_SCK);

    for(int i = 0; i < sizeof(button_pins) / sizeof(button_pins[0]); i++) {
        pinMode(button_pins[i], INPUT);
    }

    if(!init_display(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_ADDRESS)) {
        Serial.println("error initalizing display.");
        for(;;);
    }

    if(!init_radio()) {
        Serial.println("error initalizing radio.");
        for(;;);
    }

    handler = new AppHandler(display, radio);

    handler->add_app(new AppIdle(radio, display, handler));
    handler->add_app(new AppReadme(radio, display, handler));
    handler->add_app(new AppReplay(radio, display, handler));
    handler->add_app(new AppRecord(radio, display, handler));
    handler->add_app(new AppJammer(radio, display, handler));
    handler->add_app(new AppFoxHunt(radio, display, handler));
    handler->add_app(new AppFox(radio, display, handler));
    handler->add_app(new AppUSB(radio, display, handler));
    handler->add_app(new AppPixelPainter(radio, display, handler));

    handler->start_app_by_index(0); // load idle animation
}

void setup1() {
    delay(1000);
}

void loop() {
    ButtonStates btn_states = get_button_states();

    handler->loop(btn_states);
}

void loop1() {
    handler->loop1();
}