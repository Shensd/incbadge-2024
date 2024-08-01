#ifndef APP_PIXELPAINTER_HPP
#define APP_PIXELPAINTER_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include "app.hpp"

class AppPixelPainter : public App {
private:
    const char* name = "PIXELPAINTER";
    const int WIDTH = 128;
    const int HEIGHT = 64;
    int cursorX = 0;
    int cursorY = 0;
    const int cursorTickSpeed = 300;
    int cursorTick = 0;
    int cursorWhite = true;
    bool canvas[64/4][128/4];
    bool holdingDraw = false;
    bool heldColor = true;

public:
    AppPixelPainter(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_PIXELPAINTER_HPP