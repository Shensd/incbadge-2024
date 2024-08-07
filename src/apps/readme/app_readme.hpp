#ifndef APP_README_HPP
#define APP_README_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include "app.hpp"

class AppReadme : public App {
private:
    const char* name = "README";

    int32_t scroll_offset = 0;

    const char messages[19][160] = {
        "Controls: The right\ntwo buttons are\nB and A.\nThe left four are\nup/down/left/right.\nNav this readme with left and right.",
        "Apps can be exited by\npressing B, pressing\nA in most apps opens\na settings menu.",
        "This device\ntransmits radio!\nMake sure you are\npermitted to transmit\non the frequencies\nyou transmit on!",
        "DEFCON has a\nfoxhunt competition,\nmake sure you stay\noff their bands or\nyou'll have angry\nhams on your hands!",
        "Source code can\nbe found at\ngithub.com/shensd/\nincbadge-2024",
        "Here are now app\nspecific notes:",
        "IDLE:\nPressing any button\nexits you to main.",
        "README:\nYou are here now!\n",
        // "PLAY SUB:\nSelect and play\na few built-in sub\nfiles. Pressing A\nlets you select\na file to play.",
        "RECORD+REPLAY: (1/2)\nRecords and replays\nsignals. A brings up settings.\nUP also prints\ncurrent recording to\nserial.",
        "RECORD+REPLAY: (2/2)\nDevice has no\npersistent memory!\nPower off means\nrecordings disappear!",
        "SCANNER:\nUses flipper hopping\nfrequencies.",
        "JAMMER:\nA allows frequency\nchange.\nBe careful with what\nfrequencies you jam!",
        "MOUSE:\nUsing a second badge,\nconnect one to a PC\nand use the other\nto move the mouse!",
        "FOXHUNT:\nDirectional antenna\nsuggested!\n(not included)",
        "FOX:\nSee note about\nDEFCON fox hunting\ncompetition.",
        "PAINT:\nGet creative!\nA inverts selected\npixel.\nUP+LEFT opens\ntools menu.\n",
        "USB CLI MODE:\nConnect to serial to\naccess USB cli\n(arduino serial tool\nis recommended).",
        "FLASH MODE:\nJust reboots pico\ninto flash mode,\nfor developers.\nIf actived, power\ncycle to get back\nto main.",
        "Happy DEFCON 32!\n(you can press B to\nexit to main menu)",
    };

    char buffer[32];
public:
    AppReadme(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_README_HPP