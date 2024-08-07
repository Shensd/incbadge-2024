#ifndef APP_HPP
#define APP_HPP

#include <RadioLib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SPI.h>

#include "buttons.hpp"
#include "radiohal.hpp"

class AppHandler;

class App {
private:
    const char name[32] = {0};

protected:
    CC1101 radio;
    Adafruit_SSD1306 *display;
    AppHandler *handler;
public:
    App(CC1101 radio, Adafruit_SSD1306 *display, AppHandler *handler) : radio(radio), display(display), handler(handler) {}

    virtual void setup() = 0;
    virtual void loop(ButtonStates btn_states) = 0;
    virtual void loop1() = 0;
    virtual void close() = 0;

    virtual const char *get_name() { return name; }
};

#define MAX_APPS 32

class AppHandler {
private:
    Adafruit_SSD1306 *display;
    CC1101 radio;
    App* apps[MAX_APPS];
    uint32_t num_apps = 0;

    App* current_app = NULL;

    int32_t ui_selected_app = 0;

    // int8_t circle_radius = 4;
    uint16_t cube_w = 16, cube_h = 16;
    int16_t cube_dx = 1, cube_dy = -1;
    int16_t cube_x = SCREEN_WIDTH / 2, cube_y = SCREEN_HEIGHT / 2;

    void draw_bouncing_cube();

    void display_error_messasge();

    bool previous_app_closed_with_error = false;
    int16_t previous_app_error_status = 0;
    const char* placeholder_null = "NULL";
    char const* previous_app_name = NULL;
public:
    AppHandler(Adafruit_SSD1306 *display, CC1101 radio);

    void add_app(App* app) {
        if(num_apps < MAX_APPS) {
            apps[num_apps] = app;
            num_apps++;
        } else {
            Serial.printf("attempted to add app when at app limit (%d)\n", MAX_APPS);
        }
    }

    void start_app(App* app);

    void start_app_by_index(uint32_t index);

    void exit_current();
    void exit_current_with_error(int16_t status);
    void loop(ButtonStates btn_states);
    void loop1();
};

#endif // !APP_HPP