#ifndef APP_REPLAY_HPP
#define APP_REPLAY_HPP /*** DONT FORGET TO CHANGE INCLUDE GUARDS ***/

#include "app.hpp"
#include "subfiles.hpp"
#include "radiohal.hpp"

class AppReplay : public App {
private:
    const char* name = "PLAY SUB";

    // uint8_t modulation = AppReplay_NS::CONFIG_MOD_AM650;
    // uint8_t temp_modulation = 0; // for config screen

    // float frequency = 0;
    // float temp_frequency = 0;

    radiohal::SubFile const* current_subfile = &AppReplay_NS::DEFAULT_SUB_FILES[0];

    uint8_t sub_file_index = 0;
    int8_t temp_sub_file_index = 0;

    void load_selected_sub_file(const radiohal::SubFile* subfile);

    uint32_t right_initial_hold = 0;
    uint32_t left_initial_hold = 0;
    uint32_t right_hold_tick = 0;
    uint32_t left_hold_tick = 0;
    int8_t configuration_options = 1;
    int8_t current_configuration_option = 0;
    bool in_configuration_loop = false;
    void loop_configuration(ButtonStates btn_states);

    uint8_t line_animation_offset = 0;

    uint32_t transmit_index = 0;
    uint32_t transmit_timer = 0;
    bool do_transmit = false;

    bool should_close = false;
    // bool loop1_has_finished = false;
public:
    AppReplay(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_REPLAY_HPP