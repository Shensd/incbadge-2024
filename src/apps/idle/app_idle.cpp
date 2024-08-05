#include "app_idle.hpp"

AppIdle::AppIdle(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler) : App(radio, display, handler) {}

void AppIdle::setup() {
    // since idle animations will likely be running most of the time, idle the other
    // core in animation mode since that saves about 10mA
    rp2040.idleOtherCore();

    effect_timer = millis();

    // dvd logo
    for(int i = 0; i < sizeof(balls) / sizeof(AppIdle_NS::ball); i++) {
        // balls[i].size = random(4, 10);
        balls[i].size = 8;
        balls[i].x = random(0, SCREEN_WIDTH - balls[i].size);
        balls[i].y = random(0, SCREEN_HEIGHT - balls[i].size);
        balls[i].d_x = (random(0,2) == 0 ? -1 : 1) * 3;
        balls[i].d_y = (random(0,2) == 0 ? -1 : 1) * 0;
        balls[i].filled = i % 2;
    }

    // matrix effect
    for(int i = 0; i < sizeof(matrix_lines) / sizeof(AppIdle_NS::matrix_line); i++) {
        matrix_lines[i].x = 0;
        matrix_lines[i].y = 0;
        matrix_lines[i].trail_len = 0;
    }

    // pong game
    pong_paddle_a_pos = random(0, SCREEN_HEIGHT - 16);
    pong_paddle_b_pos = random(0, SCREEN_HEIGHT - 16);
    pong_ball.x = SCREEN_WIDTH / 2;
    pong_ball.y = SCREEN_HEIGHT / 2;
    pong_ball.d_x = (random(0,2) == 0 ? -1 : 1) * 3;
    pong_ball.d_y = (random(0,2) == 0 ? -1 : 1) * 3;

    // triangle
    for(int i = 0; i < sizeof(triangle_points) / sizeof(triangle_points[0]); i++) {
        triangle_points[i].x = random(8, SCREEN_WIDTH - 8);
        triangle_points[i].y = random(8, SCREEN_HEIGHT - 8);
        triangle_points[i].d_x = (random(0,2) == 0 ? -1 : 1) * random(1, 3);
        triangle_points[i].d_y = (random(0,2) == 0 ? -1 : 1) * random(1, 3);
    }

    // bouncing ball
    bouncing_ball.x = SCREEN_WIDTH / 2;
    bouncing_ball.y = SCREEN_HEIGHT / 2;
    bouncing_ball.d_x = 1;
    bouncing_ball.d_y = 1;
    bouncing_ball.size = 18;

    // tunnel
    for(int i = 0; i < sizeof(circle_sizes) / sizeof(circle_sizes[0]); i++) {
        circle_sizes[i] = i * 8;
    }
    tunnel_update_timer = millis();
}

void AppIdle::loop_dvd_boxes() {
    display->clearDisplay();

    display->fillRect(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, SSD1306_WHITE);

    for(int i = 0; i < sizeof(balls) / sizeof(AppIdle_NS::ball); i++) {
        balls[i].x += balls[i].d_x;
        balls[i].y += balls[i].d_y;

        if(balls[i].x + balls[i].size > SCREEN_WIDTH || balls[i].x < 0) {
            balls[i].d_x = -balls[i].d_x;
        }
        if(balls[i].y + balls[i].size > SCREEN_HEIGHT || balls[i].y < 0) {
            balls[i].d_y = -balls[i].d_y;
        }

        if(balls[i].x < (SCREEN_WIDTH / 2) - balls[i].size) {
            display->fillRect(balls[i].x, balls[i].y, balls[i].size, balls[i].size, SSD1306_WHITE);
        } else if(balls[i].x > (SCREEN_WIDTH / 2)) {
            display->fillRect(balls[i].x, balls[i].y, balls[i].size, balls[i].size, SSD1306_BLACK);
        } else {
            display->fillRect(balls[i].x, balls[i].y, (SCREEN_WIDTH / 2) - balls[i].x, balls[i].size, SSD1306_WHITE);
            display->fillRect((SCREEN_WIDTH / 2), balls[i].y, balls[i].size - ((SCREEN_WIDTH / 2) - balls[i].x), balls[i].size, SSD1306_BLACK);
        }
        
    }

    display->display();
}

void AppIdle::loop_matrix_effect() {

    if(millis() < matrix_last_update + 80) return;

    matrix_last_update = millis();

    display->clearDisplay();

    for(int i = random(0, sizeof(matrix_lines) / sizeof(AppIdle_NS::matrix_line)); i < sizeof(matrix_lines) / sizeof(AppIdle_NS::matrix_line); i++) {
        if(matrix_lines[i].y == 0) {
            matrix_lines[i].x = random(0, SCREEN_WIDTH / 8);
            matrix_lines[i].trail_len = random(3, 12);
            matrix_lines[i].y = 8 + matrix_lines[i].trail_len;
            break;
        }
    }

    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->cp437(true);

    for(int i = 0; i < sizeof(matrix_lines) / sizeof(AppIdle_NS::matrix_line); i++) {
        if(matrix_lines[i].y == 0) continue;

        for(int j = matrix_lines[i].y - matrix_lines[i].trail_len; j < matrix_lines[i].y; j++) {
            display->setCursor(i*8, SCREEN_HEIGHT - (j*8));
            display->write(random(0x21, 0xFD));
            // display->write('A');
        }

        matrix_lines[i].y--;
    }

    display->display();
}

void AppIdle::loop_pong() {
    display->clearDisplay();

    pong_ball.x += pong_ball.d_x;
    pong_ball.y += pong_ball.d_y;

    if(pong_ball.d_x < 0) {
        // pong_paddle_a_pos = pong_ball.y;
        if(pong_ball.y < pong_paddle_a_pos + 16) pong_paddle_a_pos-=2;
        if(pong_ball.y > pong_paddle_a_pos + 16) pong_paddle_a_pos+=2;
        if(pong_ball.y < pong_paddle_a_pos + 8) pong_paddle_a_pos--;
        if(pong_ball.y > pong_paddle_a_pos + 8) pong_paddle_a_pos++;
    } else {
        if(pong_ball.y < pong_paddle_b_pos + 16) pong_paddle_b_pos-=2;
        if(pong_ball.y > pong_paddle_b_pos + 16) pong_paddle_b_pos+=2;
        if(pong_ball.y < pong_paddle_b_pos + 8) pong_paddle_b_pos--;
        if(pong_ball.y > pong_paddle_b_pos + 8) pong_paddle_b_pos++;
    }

    if(pong_ball.x < 8 || pong_ball.x + 8 > SCREEN_WIDTH) pong_ball.d_x = -1 * pong_ball.d_x;
    if(pong_ball.y < 8 || pong_ball.y + 8 > SCREEN_HEIGHT) pong_ball.d_y = -1 * pong_ball.d_y;


    display->fillRect(2, pong_paddle_a_pos, 4, 16, SSD1306_WHITE);
    display->fillRect(SCREEN_WIDTH - 4, pong_paddle_b_pos, 4, 16, SSD1306_WHITE);
    display->fillCircle(pong_ball.x, pong_ball.y, 4, SSD1306_WHITE);

    display->fillRect(SCREEN_WIDTH / 2, 0, 2, SCREEN_HEIGHT, SSD1306_WHITE);

    display->display();
}

void AppIdle::loop_dot_triangle() {
    display->clearDisplay();

    for(int i = 0; i < 3; i++) {
        triangle_points[i].x += triangle_points[i].d_x;
        triangle_points[i].y += triangle_points[i].d_y;

        if(triangle_points[i].x < 0 || triangle_points[i].x > SCREEN_WIDTH - 3) triangle_points[i].d_x = -triangle_points[i].d_x;
        if(triangle_points[i].y < 0 || triangle_points[i].y > SCREEN_HEIGHT - 3) triangle_points[i].d_y = -triangle_points[i].d_y;

        display->fillCircle(
            triangle_points[i].x,
            triangle_points[i].y,
            3,
            SSD1306_WHITE);
    }

    display->drawTriangle(
        triangle_points[0].x, triangle_points[0].y,
        triangle_points[1].x, triangle_points[1].y,
        triangle_points[2].x, triangle_points[2].y,
        SSD1306_WHITE
    );

    display->display();
}

void AppIdle::loop_dc_32_scrolling_text() {
    display->clearDisplay();

    scrolling_text_offset += scrolling_text_dx;

    if(scrolling_text_offset > 256 || scrolling_text_offset < -8) {
        scrolling_text_dx = -scrolling_text_dx;
    }

    display->setTextSize(8);
    display->setCursor(-scrolling_text_offset,0);
    display->setTextColor(SSD1306_WHITE);
    display->setTextWrap(false);
    display->write("DEFCON32");

    display->display(); 
}

void AppIdle::loop_bouncing_ball() {

    display->clearDisplay();

    bouncing_ball.x += bouncing_ball.d_x;
    bouncing_ball.y += bouncing_ball.d_y;

    if(bouncing_ball.x <= 0 + (bouncing_ball.size / 2) || bouncing_ball.x >= SCREEN_WIDTH - (bouncing_ball.size / 2)) {
        bouncing_ball.d_x = -bouncing_ball.d_x;
    }
    if(bouncing_ball.y <= 0 + (bouncing_ball.size / 2) || bouncing_ball.y >= SCREEN_HEIGHT - (bouncing_ball.size / 2)) {
        bouncing_ball.d_y = -bouncing_ball.d_y;
    }

    display->fillCircle(bouncing_ball.x, bouncing_ball.y, bouncing_ball.size, SSD1306_WHITE);

    uint8_t square_size = 8;

    for(uint32_t x = 0; x < SCREEN_WIDTH / square_size; x++) {
        for(uint32_t y = 0; y < SCREEN_HEIGHT / square_size; y++) {
            // if(y%2 == 0) continue;
            if((x+(y%2))%2 == 0) continue;
            display->fillRect(x*square_size, y*square_size, square_size, square_size, SSD1306_BLACK);
        }
    }

    display->display();

}

void AppIdle::loop_tunnel() {
    display->clearDisplay();

    if(millis() > tunnel_update_timer + 100) {
        tunnel_update_timer = millis();
    } else {
        return;
    }

    for(int i = 0; i < sizeof(circle_sizes) / sizeof(circle_sizes[0]); i++) {
        circle_sizes[i] = (circle_sizes[i] + 1) % 80;
        display->drawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, circle_sizes[i], SSD1306_WHITE);
    }

    display->display();
}

void AppIdle::loop_cards() {
    int8_t x = random(-32, SCREEN_WIDTH), y = random(-32, SCREEN_HEIGHT);

    display->fillRect(
        x, 
        y, 
        16, 
        32, 
        SSD1306_BLACK
    );
    display->drawRect(
        x, 
        y, 
        16, 
        32, 
        SSD1306_WHITE
    );
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(x+6, y+12);
    display->write(random(0x03, 0x07));

    display->display();
}

void AppIdle::loop(ButtonStates btn_states) {
    if(
        btn_states.UP_FALLING_EDGE || btn_states.DOWN_FALLING_EDGE || btn_states.LEFT_FALLING_EDGE || 
        btn_states.RIGHT_FALLING_EDGE || btn_states.A_FALLING_EDGE || btn_states.B_FALLING_EDGE) {
        handler->exit_current();
        return;
    }

    if(millis() > effect_timer + NEXT_EFFECT_MS) {
        effect_timer = millis();
        effect_index++;

        if(effect_index >= NUM_EFFECTS) effect_index = 0;
    }

    switch(effect_index) {
    case 0:
        loop_matrix_effect();
        break;
    case 1:
        loop_cards();
        break;
    case 2:
        loop_pong();
        break;
    case 3:
        loop_dot_triangle();
        break;
    case 4:
        loop_dc_32_scrolling_text();
        break;
    case 5:
        loop_bouncing_ball();
        break;
    case 6:
        loop_tunnel();
        break;
    case 7:
        loop_dvd_boxes();
        break;
    }
    
}

void AppIdle::loop1() {

}

void AppIdle::close() {
    rp2040.resumeOtherCore();
}