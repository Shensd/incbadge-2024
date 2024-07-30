#ifndef APP_TEST_HPP
#define APP_TEST_HPP

#include "app.hpp"

class AppTesting1 : public App {
private:
    const char* name = "Test App 1";
public:
    AppTesting1(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

    void setup() override {}
    void loop(ButtonStates btn_states) override {
        if(btn_states.B_FALLING_EDGE) {
            handler->exit_current();
            return;
        }

        display->clearDisplay();
        display->setTextSize(1);      // Normal 1:1 pixel scale
        display->setTextColor(SSD1306_WHITE); // Draw white text
        display->setCursor(0, 0);     // Start at top-left corner
        display->cp437(true);         // Use full 256 char 'Code Page 437' font
        display->write(name);
        display->display();
    }
    void loop1() override {}
    void close() override {}

    const char* get_name() override { return name; }
};
class AppTesting2 : public App {
private:
    const char* name = "Test App 2";
public:
    AppTesting2(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

    void setup() override {}
    void loop(ButtonStates btn_states) override {
        if(btn_states.B_FALLING_EDGE) {
            handler->exit_current();
            return;
        }

        display->clearDisplay();
        display->setTextSize(1);      // Normal 1:1 pixel scale
        display->setTextColor(SSD1306_WHITE); // Draw white text
        display->setCursor(0, 0);     // Start at top-left corner
        display->cp437(true);         // Use full 256 char 'Code Page 437' font
        display->write(name);
        display->display();
    }
    void loop1() override {}
    void close() override {}

    const char* get_name() override { return name; }
};
class AppTesting3 : public App {
private:
    const char* name = "Test App 3";
public:
    AppTesting3(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

    void setup() override {}
    void loop(ButtonStates btn_states) override {
        if(btn_states.B_FALLING_EDGE) {
            handler->exit_current();
            return;
        }

        display->clearDisplay();
        display->setTextSize(1);      // Normal 1:1 pixel scale
        display->setTextColor(SSD1306_WHITE); // Draw white text
        display->setCursor(0, 0);     // Start at top-left corner
        display->cp437(true);         // Use full 256 char 'Code Page 437' font
        display->write(name);
        display->display();
    }
    void loop1() override {}
    void close() override {}

    const char* get_name() override { return name; }
};
class AppTesting4 : public App {
private:
    const char* name = "Test App 4";
public:
    AppTesting4(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

    void setup() override {}
    void loop(ButtonStates btn_states) override {
        if(btn_states.B_FALLING_EDGE) {
            handler->exit_current();
            return;
        }

        display->clearDisplay();
        display->setTextSize(1);      // Normal 1:1 pixel scale
        display->setTextColor(SSD1306_WHITE); // Draw white text
        display->setCursor(0, 0);     // Start at top-left corner
        display->cp437(true);         // Use full 256 char 'Code Page 437' font
        display->write(name);
        display->display();
    }
    void loop1() override {}
    void close() override {}

    const char* get_name() override { return name; }
};
class AppTesting5 : public App {
private:
    const char* name = "Test App 5";
public:
    AppTesting5(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

    void setup() override {}
    void loop(ButtonStates btn_states) override {
        if(btn_states.B_FALLING_EDGE) {
            handler->exit_current();
            return;
        }

        display->clearDisplay();
        display->setTextSize(1);      // Normal 1:1 pixel scale
        display->setTextColor(SSD1306_WHITE); // Draw white text
        display->setCursor(0, 0);     // Start at top-left corner
        display->cp437(true);         // Use full 256 char 'Code Page 437' font
        display->write(name);
        display->display();
    }
    void loop1() override {}
    void close() override {}

    const char* get_name() override { return name; }
};
class AppTesting6 : public App {
private:
    const char* name = "Test App 6";
public:
    AppTesting6(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

    void setup() override {}
    void loop(ButtonStates btn_states) override {
        if(btn_states.B_FALLING_EDGE) {
            handler->exit_current();
            return;
        }

        display->clearDisplay();
        display->setTextSize(1);      // Normal 1:1 pixel scale
        display->setTextColor(SSD1306_WHITE); // Draw white text
        display->setCursor(0, 0);     // Start at top-left corner
        display->cp437(true);         // Use full 256 char 'Code Page 437' font
        display->write(name);
        display->display();
    }
    void loop1() override {}
    void close() override {}

    const char* get_name() override { return name; }
};

#endif // !APP_TEST_HPP