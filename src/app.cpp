#include "app.hpp"

AppHandler::AppHandler(Adafruit_SSD1306 *display, CC1101 radio) : display(display), radio(radio) {}

void AppHandler::exit_current() {
    if(current_app == NULL) return;

    current_app->close();

    // make sure radio is always powered down when an app is closed,
    // since it's the most power hungry componenet
    radio.finishTransmit();
    radio.packetMode();
    radio.standby();

    rp2040.idleOtherCore();

    current_app = NULL;
}

void AppHandler::start_app(App* app) {
    exit_current();

    current_app = app;

    rp2040.resumeOtherCore();

    current_app->setup();
}

void AppHandler::start_app_by_index(uint32_t index) {
    if(index < num_apps) {
        start_app(apps[index]);
    } else {
        #ifdef DO_SERIAL_DEBUG_INFO
        Serial.printf("attempted to start app of index %d when max index %d\n", index, num_apps - 1);
        #endif // DO_SERIAL_DEBUG_INFO
    }
}

void AppHandler::draw_bouncing_cube() {
    cube_x += cube_dx;
    cube_y += cube_dy;

    if(cube_x < 0 || cube_x > SCREEN_WIDTH - cube_w) cube_dx = -cube_dx;
    if(cube_y < 0 || cube_y > SCREEN_HEIGHT - cube_h) cube_dy = -cube_dy;

    for(int x = cube_x; x < cube_x + cube_w; x++) {
        for(int y = cube_y; y < cube_y + cube_h; y++) {
            // if(dist_from_pixel(x - cube_x, y - cube_y, 8, 8) < circle_radius) 
            display->writePixel(x, y, !display->getPixel(x, y));
        }
    }
}

void AppHandler::loop(ButtonStates btn_states) {
    if(current_app != NULL) {
        current_app->loop(btn_states);
        return;
    }

    if(btn_states.A_FALLING_EDGE) {
        start_app_by_index(ui_selected_app);
        return;
    }

    if(btn_states.DOWN_FALLING_EDGE) {
        ui_selected_app = (ui_selected_app + 1) % num_apps;
    }
    if(btn_states.UP_FALLING_EDGE) {
        ui_selected_app = ui_selected_app - 1;
        if(ui_selected_app < 0) ui_selected_app = num_apps - 1;
    }

    display->clearDisplay();

    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(0, 0);
    display->cp437(true);

    uint8_t num_apps_to_display = 6;

    display->fillRect(0, 0, 128, 9, SSD1306_WHITE);

    for(uint32_t i = ui_selected_app; i < min(ui_selected_app + num_apps_to_display, num_apps); i++) {
        uint32_t position = i - ui_selected_app;

        if(position == 0) {
            display->setTextColor(SSD1306_BLACK);
        } else {
            display->setTextColor(SSD1306_WHITE);
        }

        display->setCursor(0, position*10);
        display->write(apps[i]->get_name());
    }

    draw_bouncing_cube();

    display->display();
}
void AppHandler::loop1() {
    if(current_app != NULL) {
        current_app->loop1();
        return;
    }
}