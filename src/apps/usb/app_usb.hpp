#ifndef APP_USB_HPP
#define APP_USB_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include "app.hpp"
#include "radiohal.hpp"

#include <getopt.h>

#define APPUSB_FIFO_COMMAND_START_RECORD 0x100
#define APPUSB_FIFO_COMMAND_START_REPLAY 0x102
#define APPUSB_FIFO_COMMAND_FINISHED_RECORD  0x300
#define APPUSB_FIFO_COMMAND_FINISHED_REPLAY  0x302

#define APPUSB_TIMING_BUFFER_SIZE 8192

class AppUSB : public App {
private:
    const char* name = "USB CLI MODE";

    bool SHARED_should_run = false;

    const float DEFAULT_FREQUENCY = 433;

    uint32_t SHARED_timing_index = 0;
    int32_t SHARED_timings[APPUSB_TIMING_BUFFER_SIZE]; 

    bool GLOBAL_config_lock = false;

    int16_t replay_sub(const int32_t* timings, uint32_t timings_length);
    int16_t record_sub();
    uint32_t parse_cli_input(char* raw_command, size_t command_len, char** command_tokens, uint32_t max_tokens);
    void start_raw_tx();
    void stop_rxtx();
    void print_help();
public:
    AppUSB(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_USB_HPP