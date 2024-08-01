#include "app_pixelpainter.hpp"

AppPixelPainter::AppPixelPainter(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppPixelPainter::setup() {
    display->cp437();
    cursorX = WIDTH/2;
    cursorY = HEIGHT/2;
    for(int y = 0; y < HEIGHT/4; y++){
        for (int x = 0; x < WIDTH/4; x++){
            canvas[y][x] = false;
        }
    }
}

void AppPixelPainter::loop(ButtonStates btn_states) {
    // Exit on B Press
    if(btn_states.B_FALLING_EDGE) {
        handler->exit_current();
        return;
    }
    // Cursor Movement
    if (btn_states.UP_FALLING_EDGE) {
        cursorY -= 4;
    }
    if (btn_states.DOWN_FALLING_EDGE) {
        cursorY += 4;
    }
    if (btn_states.LEFT_FALLING_EDGE) {
        cursorX -= 4;
    }
    if (btn_states.RIGHT_FALLING_EDGE) {
        cursorX += 4;
    }
    if (cursorX >= 128){
        cursorX -= 128;
    }
    if (cursorY >= 64){
        cursorY -= 64;
    }
    if (cursorX < 0){
        cursorX += 128;
    }
    if (cursorY < 0){
        cursorY += 64;
    }
    // Cursor Blinking
    if (millis() - cursorTick > cursorTickSpeed){
        cursorTick = millis();
        cursorWhite = !cursorWhite;
    }
    // Draw Pixel Press
    if(btn_states.A_RISING_EDGE) {
        canvas[cursorY/4][cursorX/4] = !canvas[cursorY/4][cursorX/4];
        heldColor = canvas[cursorY/4][cursorX/4];
    }else if (btn_states.A){
        canvas[cursorY/4][cursorX/4] = heldColor;
    }
    // Draw Canvas
    display->clearDisplay();
    for(int y = 0; y < HEIGHT/4; y++){
        for (int x = 0; x < WIDTH/4; x++){
            display->fillRect(x*4, y*4, 4, 4, canvas[y][x]);
        }
    }

    display->fillRect(cursorX, cursorY, 4, 4, cursorWhite);
    display->display();
}

void AppPixelPainter::loop1() {

}

void AppPixelPainter::close() {
    
}