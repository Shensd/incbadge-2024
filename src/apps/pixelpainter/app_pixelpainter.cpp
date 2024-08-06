#include "app_pixelpainter.hpp"

AppPixelPainter::AppPixelPainter(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {
    for(int y = 0; y < HEIGHT/4; y++){
        for (int x = 0; x < WIDTH/4; x++){
            canvas[y][x] = false;
        }
    }
}

void AppPixelPainter::setup() {
    display->cp437();
    cursorX = WIDTH/2;
    cursorY = HEIGHT/2;

    rp2040.idleOtherCore();
}

void AppPixelPainter::settings_loop(ButtonStates btn_states) {

    if(btn_states.A_RISING_EDGE) {
        if(settings_option_erase_canvas) {
            for(int y = 0; y < HEIGHT/pixel_size; y++){
                for (int x = 0; x < WIDTH/pixel_size; x++){
                    canvas[y][x] = false;
                }
            }    
        }
        if(settings_option_invert_canvas) {
            for(int y = 0; y < HEIGHT/pixel_size; y++){
                for (int x = 0; x < WIDTH/pixel_size; x++){
                    canvas[y][x] = !canvas[y][x];
                }
            }
        }

        settings_option_invert_canvas = false;
        settings_option_erase_canvas = false;
        
        in_settings = false;
        return;
    }

    if(btn_states.B_RISING_EDGE) {
        settings_option_invert_canvas = false;
        settings_option_erase_canvas = false;
        in_settings = false;
        return;
    }

    if(btn_states.UP_RISING_EDGE) {
        settings_selected_option++;
    } else if(btn_states.DOWN_RISING_EDGE) {
        settings_selected_option--;
    }

    if(settings_selected_option < 0) settings_selected_option = num_settings_options - 1;
    if(settings_selected_option >= num_settings_options) settings_selected_option = 0;

    bool do_action = btn_states.LEFT_FALLING_EDGE ||  btn_states.RIGHT_FALLING_EDGE;

    switch(settings_selected_option) {
    case 0: // invert canvas
        if(do_action) settings_option_invert_canvas = !settings_option_invert_canvas;
        break;
    case 1: // erase canvas
        if(do_action) settings_option_erase_canvas = !settings_option_erase_canvas;
        break;
    default: // uh oh
        break;
    }

    display->clearDisplay();

    display->setTextColor(SSD1306_WHITE);
    display->setTextSize(1);
    display->cp437(true);
    display->setCursor(2, 2);

    display->write("a=save b=back");

    if(settings_selected_option == 0) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    display->setCursor(2, 10);
    char buffer[32];
    sprintf(buffer, "INVERT: %s", settings_option_invert_canvas ? "YES" : "NO");
    display->write(buffer);

    if(settings_selected_option == 1) display->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    else display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    display->setCursor(2, 20);
    sprintf(buffer, "ERASE CANVAS: %s", settings_option_erase_canvas ? "YES" : "NO");
    display->write(buffer);

    display->display(); 
}

void AppPixelPainter::loop(ButtonStates btn_states) {
    // Exit on B Press
    if(btn_states.B_FALLING_EDGE) {
        handler->exit_current();
        return;
    }

    if(in_settings) {
        settings_loop(btn_states);
        return;
    }

    if(btn_states.UP && btn_states.LEFT) {
        in_settings = true;
        return;
    }

    // Cursor Movement
    if(!btn_states.UP && !btn_states.DOWN && !btn_states.LEFT && !btn_states.RIGHT) {
        currently_held = false;
    }

    if(currently_held == false && millis() < held_timer + held_tick_timeout) {
        if (btn_states.UP_FALLING_EDGE) {
            cursorY -= pixel_size;
        } else if (btn_states.DOWN_FALLING_EDGE) {
            cursorY += pixel_size;
        } else if (btn_states.LEFT_FALLING_EDGE) {
            cursorX -= pixel_size;
        } else if (btn_states.RIGHT_FALLING_EDGE) {
            cursorX += pixel_size;
        }
    }

    if(btn_states.UP_RISING_EDGE || btn_states.DOWN_RISING_EDGE || btn_states.LEFT_RISING_EDGE || btn_states.RIGHT_RISING_EDGE) {
        if(!currently_held) {
            currently_held = true;
            held_timer = millis();
        }
    }

    if(currently_held && millis() > held_timer + held_tick_timeout) {
        held_timer = millis();

        if(btn_states.UP) {
            cursorY -= pixel_size;
        } else if(btn_states.DOWN) {
            cursorY += pixel_size;
        } else if(btn_states.LEFT) {
            cursorX -= pixel_size;
        } else if(btn_states.RIGHT) {
            cursorX += pixel_size;
        }
    }

    if (cursorX >= 128) cursorX -= 128;
    if (cursorY >= 64) cursorY -= 64;
    if (cursorX < 0) cursorX += 128;
    if (cursorY < 0) cursorY += 64;

    // Cursor Blinking
    if (millis() - cursorTick > cursorTickSpeed){
        cursorTick = millis();
        cursorWhite = !cursorWhite;
    }
    // Draw Pixel Press
    if(btn_states.A_RISING_EDGE) {
        canvas[cursorY/pixel_size][cursorX/pixel_size] = !canvas[cursorY/pixel_size][cursorX/pixel_size];
        heldColor = canvas[cursorY/pixel_size][cursorX/pixel_size];
    }else if (btn_states.A){
        canvas[cursorY/pixel_size][cursorX/pixel_size] = heldColor;
    }
    // Draw Canvas
    display->clearDisplay();
    for(int y = 0; y < HEIGHT/pixel_size; y++){
        for (int x = 0; x < WIDTH/pixel_size; x++){
            display->fillRect(x*pixel_size, y*pixel_size, pixel_size, pixel_size, canvas[y][x]);
        }
    }

    display->fillRect(cursorX, cursorY, pixel_size, pixel_size, cursorWhite);
    display->display();
}

void AppPixelPainter::loop1() {
    // core is idle for this application
}

void AppPixelPainter::close() {
    rp2040.resumeOtherCore();
}