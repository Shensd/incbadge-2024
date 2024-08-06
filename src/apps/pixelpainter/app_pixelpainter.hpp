#ifndef APP_PIXELPAINTER_HPP
#define APP_PIXELPAINTER_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include "app.hpp"

class AppPixelPainter : public App {
private:
    const char* name = "PAINT";
    const int WIDTH = 128;
    const int HEIGHT = 64;
    int cursorX = 0;
    int cursorY = 0;
    const int cursorTickSpeed = 300;
    int cursorTick = 0;
    int cursorWhite = true;

    #define PIXELPAINTER_PIXEL_SIZE 4

    bool canvas[64/PIXELPAINTER_PIXEL_SIZE][128/PIXELPAINTER_PIXEL_SIZE];
    bool holdingDraw = false;
    bool heldColor = true;

    uint8_t pixel_size = PIXELPAINTER_PIXEL_SIZE;

    bool currently_held = false;
    uint32_t held_timer = 0;
    uint32_t held_tick_timeout = 200;

    bool in_settings = false;

    int8_t settings_selected_option = 0;
    uint8_t num_settings_options = 2;
    bool settings_option_invert_canvas = false;
    bool settings_option_erase_canvas = false;
    void settings_loop(ButtonStates btn_states);

public:
    AppPixelPainter(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_PIXELPAINTER_HPP