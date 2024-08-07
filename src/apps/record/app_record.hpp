#ifndef APP_RECORD_HPP
#define APP_RECORD_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include "app.hpp"

#include "radiohal.hpp"

class AppRecord : public App {
private:
    const char* name = "RECORD+REPLAY";

    const uint8_t num_presets = 4; 
    int8_t temp_preset_index = 0;

    bool use_ook = true;
    bool temp_use_ook = false;

    float frequency = 433.0;
    float temp_frequency = 0;

    const uint16_t available_bandwidths[16] = {
        58, 68, 81, 102, 116, 135, 162, 203, 232, 270, 325, 406, 464, 541, 650, 812
    };
    int8_t rx_bandwidth_index = 9;
    int8_t temp_rx_bandwidth_index = 0;

    float fsk_deviation = 238;
    float temp_fsk_deviation = 0;


    uint64_t tick_timer = 0;
    uint64_t last_bit_time = 0;
    bool last_bit_state = HIGH;

    float previous_rssi_readings[128];
    uint32_t rssi_reading_frequency_ms = 50;
    uint32_t last_rssi_reading = 0;
    uint8_t rssi_reading_index = 0;

    long rssi_upper_bound = -20;
    long rssi_lower_bound = -110;
    long rssi_recording_thresh = -90;

    bool do_record = false;

    bool do_replay = false;

    bool should_close = false;

    #define APPRECORD_NUM_TIMINGS_SLOTS (3)
    int8_t timings_slot = 0;

    int32_t timings[APPRECORD_NUM_TIMINGS_SLOTS][8192];
    uint32_t timings_index[APPRECORD_NUM_TIMINGS_SLOTS] = {0, 0, 0}; // make sure to update this if you add more save slots
    long last_timing_micros = 0;

    uint32_t right_initial_hold = 0;
    uint32_t left_initial_hold = 0;
    uint32_t right_hold_tick = 0;
    uint32_t left_hold_tick = 0;
    int8_t configuration_options = 5;
    int8_t current_configuration_option = 0;
    bool in_configuration_loop = false;
    void loop_configuration(ButtonStates btn_states);

    void dump_recording_over_serial();
public:
    AppRecord(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_RECORD_HPP