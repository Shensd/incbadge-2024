#include "app.hpp"

AppHandler::AppHandler(Adafruit_SSD1306 *display, CC1101 radio) : display(display), radio(radio) {}

void AppHandler::exit_current() {
    if(current_app == NULL) return;

    current_app->close();

    // make sure radio is always powered down when an app is closed,
    // since it's the most power hungry componenet
    int16_t status = 0;

    if((status = radio.finishTransmit()) != RADIOLIB_ERR_NONE) {
        Serial.printf("error running finish transmit, %d\n", status);
        return;
    }
    if((status = radio.packetMode()) != RADIOLIB_ERR_NONE) {
        Serial.printf("error putting radio in packet mode, %d\n", status);
        return;
    }
    if((status = radio.standby()) != RADIOLIB_ERR_NONE) {
        Serial.printf("error putting radio in standby, %d\n", status);
        return;
    }

    rp2040.idleOtherCore();

    current_app = NULL;
}

void AppHandler::exit_current_with_error(int16_t status) {
    previous_app_closed_with_error = true;
    previous_app_error_status = status;
    if(current_app != NULL) {
        previous_app_name = current_app->get_name();
    } else {
        previous_app_name = placeholder_null;
    }

    exit_current();
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
        Serial.printf("attempted to start app of index %d when max index %d\n", index, num_apps - 1);
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

void AppHandler::display_error_messasge() {
    display->clearDisplay();

    display->fillScreen(SSD1306_WHITE);
    display->setTextColor(SSD1306_BLACK);
    display->setTextSize(1);
    display->cp437(true); 

    display->setCursor(2, 2);
    display->write("X ERROR X");

    display->setCursor(2, 10);
    display->write("The previous app");
    display->setCursor(2, 20);
    display->write("encountered an error");
    display->setCursor(2, 30);
    display->write("and closed itself.");

    display->setCursor(2, 40);
    char buffer[32];
    display->setTextWrap(false);
    sprintf(buffer, "[%d,%s]", previous_app_error_status, previous_app_name);
    display->setTextWrap(true);
    display->write(buffer);

    display->setCursor(2, 50);
    display->write("Press A to continue.");

    display->display();
}

void AppHandler::loop(ButtonStates btn_states) {
    if(current_app != NULL) {
        current_app->loop(btn_states);
        return;
    }

    if(previous_app_closed_with_error && btn_states.A_FALLING_EDGE) {
        previous_app_closed_with_error = false;
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

    if(previous_app_closed_with_error) {
        display_error_messasge();
        return;
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