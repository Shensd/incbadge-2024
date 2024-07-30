#ifndef APP_IDLE_HPP
#define APP_IDLE_HPP

#include "pins.hpp"
#include "app.hpp"

namespace AppIdle_NS {
    struct ball {
        int32_t x, y, d_x, d_y, size;
        bool filled;
    };

    struct matrix_line {
        int32_t x, y, trail_len;
    };
    
    struct point {
        int8_t x, y;
        int8_t d_x, d_y;
    };
}

class AppIdle : public App {
private:
    const char* name = "IDLE";

    // bouncing squares effect
    AppIdle_NS::ball balls[32];

    // matrix effect
    AppIdle_NS::matrix_line matrix_lines[SCREEN_WIDTH / 8] = {0};
    int matrix_last_update = 0;

    // pong animation
    uint32_t pong_paddle_a_pos = 0;
    uint32_t pong_paddle_b_pos = 0;
    AppIdle_NS::ball pong_ball;

    // triangle points
    AppIdle_NS::point triangle_points[3];

    // dc scrolling text
    int scrolling_text_dx = 4;
    int scrolling_text_offset = 0;

    // bouncing ball checker
    AppIdle_NS::ball bouncing_ball;

    // tunnel circles
    uint32_t circle_sizes[16] = {0};
    uint32_t tunnel_update_timer = 0;

    const uint32_t NEXT_EFFECT_MS = 10000;
    const uint32_t NUM_EFFECTS = 7;
    int effect_index = 1;
    uint32_t effect_timer = 0;

    void loop_dvd_boxes();
    void loop_matrix_effect();
    void loop_pong();
    void loop_dot_triangle();
    void loop_dc_32_scrolling_text();
    void loop_bouncing_ball();
    void loop_tunnel();
public:
    AppIdle(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler);

    void setup() override;
    void loop(ButtonStates btn_states) override;
    void loop1() override;
    void close() override;

    const char* get_name() override { return name; }
};

#endif // !APP_IDLE_HPP