#include "app_usb.hpp"

AppUSB::AppUSB(CC1101 radio, Adafruit_SSD1306* display, AppHandler* handler)
    : App(radio, display, handler) {}

void AppUSB::setup() {
    display->clearDisplay();

    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->cp437(true);
    display->setCursor(2, 2);
    display->write("USB mode started,");

    display->setCursor(2, 10);
    display->write("Connect to serial to");

    display->setCursor(2, 20);
    display->write("access CLI.");

    display->setCursor(2, 40);
    display->write("Press B to exit.");

    display->display();

    int16_t state = radio.setFrequency(DEFAULT_FREQUENCY);
    if (state != RADIOLIB_ERR_NONE) {
        // Serial.printf("error in setting default frequency: %d\n", state);
        // while(1);
        Serial.print("error in setting default frequency: ");
        Serial.println(state);
    }

    state = radiohal::config_FuriHalSubGhzPresetOok650Async(radio);
    if (state != RADIOLIB_ERR_NONE) {
        // Serial.printf("error in setting default configuration: %d\n", state);
        // while(1);
        Serial.print("error in setting default configuration: ");
        Serial.println(state);
    }
}

void AppUSB::loop(ButtonStates btn_states) {
    if (btn_states.B_FALLING_EDGE) {
        handler->exit_current();
        return;
    }

    char cmd_placeholder[] = "x";
    char raw_command_buffer[128];
    char* argv[16];

    static struct option long_options[] = {
        /* NAME         ARGUMENT           FLAG  SHORTNAME */
        {"frequency", required_argument, NULL,
         'f'},  // set the frequency of the cc1101
        {"preset", required_argument, NULL,
         'p'},                              // set to use a flipper preset
        {"stop", no_argument, NULL, 's'},   // stop running concurrent rx/tx
        {"rxraw", no_argument, NULL, 'r'},  // receive raw mode
        {"txraw", no_argument, NULL, 't'},  // transmit raw mode
        {"replay", no_argument, NULL,
         'R'},  // replay last recorded raw mode signal
        {"help", no_argument, NULL,
         'h'},  // replay last recorded raw mode signal
        {"ping", no_argument, NULL,
         'P'},              // Pong! (used for connection checking)
        {NULL, 0, NULL, 0}  // end struct
    };

    int errcode;

    // set frequency vars
    bool do_set_freq = false;
    int frequency;
    float freq_mhz;
    // set preset args
    bool do_set_config = false;
    char* new_config_str;
    // tx/rx args
    bool do_raw_tx = false;
    bool do_raw_rx = false;
    bool do_tx_replay = false;
    bool do_print_settings = false;

    /********** SERIAL HANDLING **********/
    if (Serial.available() > 0) {
        // zero out command buffer
        memset(raw_command_buffer, 0, sizeof(raw_command_buffer));
        // null all entries in argv
        memset(argv, 0, sizeof(argv));
        // read raw command into command buffer
        Serial.setTimeout(10000);
        size_t bytes_read = Serial.readBytesUntil(
            '\n', raw_command_buffer,
            sizeof(raw_command_buffer) / sizeof(raw_command_buffer[0]));
        Serial.setTimeout(1000);
        // parse into typical argc/argv style cli arguments
        // (offset argv buffer by 1 since that's normally the command in a
        // normal cli)
        argv[0] = cmd_placeholder;  // placeholder for what would normally be a
                                    // shell command

        uint32_t argc = parse_cli_input(raw_command_buffer, bytes_read, argv,
                                        (sizeof(argv) / sizeof(argv[0])));

        int long_option_index = 0;
        int c;
        optind = 1;  // reset getopt state
        while ((c = getopt_long(argc, argv, ":f:p:srRthP", long_options,
                                &long_option_index)) != -1) {
            switch (c) {
                case 0:  // long option was selected
                    // Serial.printf("long option with no shorthand selected,
                    // you should not see this error message\n");
                    Serial.println(
                        "long option with no shorthand selected, you should "
                        "not see this error message");
                    break;
                case 'f':  // set frequency
                    do_set_freq = true;

                    frequency = atoi(optarg);

                    freq_mhz = (float)frequency / 1000000.0;
                    break;
                case 'p':  // set present config to use
                    do_set_config = true;

                    new_config_str = optarg;
                    break;
                case 's':  // stop current rx/tx
                    stop_rxtx();
                    break;
                case 'r':  // record raw
                    do_raw_rx = true;
                    break;
                case 't':  // transmit raw
                    do_raw_tx = true;
                    break;
                case 'R':  // replay last recording
                    do_tx_replay = true;
                    break;
                case 'h':  // print help
                    print_help();
                    return;  // exit early
                case 'P':    // pong!
                    // Serial.printf("pong!\n");
                    Serial.println("pong!");
                    return;
                case ':':
                    // Serial.printf("missing argument for option %s\n",
                    // optind);
                    Serial.print("missing argument for option ");
                    Serial.println(optind);
                    return;
                case '?':
                    // Serial.printf("error parsing cli options, offending flag:
                    // %c\n", optopt); return;
                    break;
                default:
                    // Serial.printf("?? getopt returned character code %c (%x)
                    // ??\n", c, c);
                    Serial.print("?? getopt returned character code ");
                    Serial.println(c);
                    return;
            }
        }
    }
    /********** END SERIAL HANDLING **********/

    /********** CLI HANDLING **********/

    if (do_raw_rx + do_raw_tx + do_tx_replay > 1) {
        // Serial.printf("raw rx, raw tx, and replay are mutually exclusive
        // commands\n");
        Serial.println(
            "raw rx, raw tx, and replay are mutually exclusive commands");
        return;
    }

    // allow config commands if new rx/tx is being started
    bool start_new_rxtx = (do_raw_rx + do_raw_tx + do_tx_replay) == 1;

    if (start_new_rxtx) {
        stop_rxtx();
        delay(100);
    }

    if (do_set_freq) {
        if (GLOBAL_config_lock) {
            // Serial.printf("configuration commands locked while running tx or
            // rx\n");
            Serial.println(
                "configuration commands locked while running tx or rx");
            return;
        }

        errcode = radio.setFrequency(freq_mhz);
        if (errcode != RADIOLIB_ERR_NONE) {
            // Serial.printf("invalid frequency, errcode: %d\n", errcode);
            Serial.print("invalid frequency, errcode: ");
            Serial.println(errcode);
            return;
        } else {
            // Serial.printf("frequency set to %f MHz\n", freq_mhz);
            Serial.print("frequency set to ");
            Serial.print(freq_mhz);
            Serial.println(" MHz");
        }
    }

    if (do_set_config) {
        if (GLOBAL_config_lock) {
            // Serial.printf("configuration commands locked while running tx or
            // rx\n");
            Serial.println(
                "configuration commands locked while running tx or rx");
            return;
        }

        errcode = radiohal::set_config_from_short_string(radio, new_config_str);
        if (errcode != RADIOLIB_ERR_NONE) {
            // Serial.printf("error setting configuration '%s' (probabaly bad
            // config string).", new_config_str);
            Serial.print("error setting configuration '");
            Serial.print(new_config_str);
            Serial.println("' (probably bad config string).");
            return;
        } else {
            // Serial.printf("new config set\n");
            Serial.println("new config set");
        }
    }

    if (do_raw_rx) {
        // Serial.printf("starting raw rx\n");
        Serial.println("starting raw rx");
        GLOBAL_config_lock = true;

        rp2040.fifo.push(APPUSB_FIFO_COMMAND_START_RECORD);
    } else if (do_raw_tx) {
        GLOBAL_config_lock = true;

        start_raw_tx();
    } else if (do_tx_replay) {
        GLOBAL_config_lock = true;

        rp2040.fifo.push(APPUSB_FIFO_COMMAND_START_REPLAY);
    }
    /********** END CLI HANDLING **********/

    /********** MULTI-CORE HANDLING **********/
    if (rp2040.fifo.available() > 0) {
        uint32_t fifo_command = rp2040.fifo.pop();

        switch (fifo_command) {
            case APPUSB_FIFO_COMMAND_FINISHED_RECORD:

                // Serial.print("RAW_Data: ");
                for (uint32_t i = 0; i < SHARED_timing_index; i++) {
                    if (i > 0 && i % 512 == 0) Serial.println("");
                    if (i % 512 == 0) Serial.print("RAW_Data: ");

                    // Serial.printf("%d ", SHARED_timings[i]);
                    Serial.print(SHARED_timings[i]);
                    Serial.print(" ");
                }

                Serial.println("");

                break;
            case APPUSB_FIFO_COMMAND_FINISHED_REPLAY:
                Serial.println("finished replay.\n");
                break;
            default:
                while (rp2040.fifo.available()) rp2040.fifo.pop();
                break;
        }
    }
}

void AppUSB::loop1() {
    uint32_t num_samples = 0;

    if (rp2040.fifo.available() > 0) {
        uint32_t command = rp2040.fifo.pop();

        switch (command) {
            case APPUSB_FIFO_COMMAND_START_RECORD:  // begin recording new sub
                // num_samples = rp2040.fifo.pop();

                SHARED_should_run = true;

                record_sub();

                rp2040.fifo.push(APPUSB_FIFO_COMMAND_FINISHED_RECORD);

                break;
            case APPUSB_FIFO_COMMAND_START_REPLAY:  // play sub data in shared timing
                                             // buffer
                // this command is also used for normal sub replay as primary
                // core just read timing data into shared buffer

                SHARED_should_run = true;

                replay_sub(SHARED_timings, SHARED_timing_index);

                rp2040.fifo.push(APPUSB_FIFO_COMMAND_FINISHED_REPLAY);

                break;
            default:
                while (rp2040.fifo.available()) rp2040.fifo.pop();
                break;
        }
    }
}

void AppUSB::close() {
    stop_rxtx();

    delay(500);

    if (radio.standby() != RADIOLIB_ERR_NONE) {
        Serial.println("error putting radio in standby");
    }
}

int16_t AppUSB::replay_sub(const int32_t* timings, uint32_t timings_length) {
    if (timings_length < 1) return RADIOLIB_ERR_NONE;

    pinMode(RADIO_gd0, OUTPUT);
    digitalWrite(RADIO_gd0, LOW);

    // Serial.printf("playing %d timings...\n", timings_length);
    Serial.print("playing ");
    Serial.print(timings_length);
    Serial.println(" timings...");

    radio.transmitDirectAsync();
    delay(100);
    while (SHARED_should_run) {
        for (int i = 0; i < timings_length; i++) {
            int d = timings[i];
            if (d < 0) {
                d = -1 * d;
            }
            digitalWrite(RADIO_gd0, timings[i] > 0);
            delayMicroseconds(d);
        }
        delay(100);
    }
    radio.finishTransmit();

    return RADIOLIB_ERR_NONE;
}

int16_t AppUSB::record_sub() {
    SHARED_timing_index = 0;

    pinMode(RADIO_gd0, INPUT);

    radio.receiveDirectAsync();
    delay(100);

    uint64_t tick_timer = micros();
    uint64_t last_bit_time = micros();
    uint8_t last_bit_state = 1;

    while (SHARED_timing_index < APPUSB_TIMING_BUFFER_SIZE && SHARED_should_run) {
        uint8_t d = digitalRead(RADIO_gd0);

        if (micros() - tick_timer > 20000) {
            tick_timer = micros();
            // Serial.printf("RSSI: %f;BUFFER_IDX: %u\n", radio.getRSSI(),
            // SHARED_timing_index);
            // Serial.print("R:");
            // Serial.print(radio.getRSSI());
            // Serial.print(",I:");
            // Serial.println(SHARED_timing_index);

            // Serial.printf("RSSI: %d, Buffer Index: %d\r", radio.getRSSI(), SHARED_timing_index);
        }

        if (d == last_bit_state) continue;

        SHARED_timings[SHARED_timing_index] = micros() - last_bit_time;

        if (last_bit_state == 0)
            SHARED_timings[SHARED_timing_index] =
                -1 * SHARED_timings[SHARED_timing_index];

        last_bit_state = d;
        last_bit_time = micros();
        SHARED_timing_index++;
    }

    return RADIOLIB_ERR_NONE;
}

uint32_t AppUSB::parse_cli_input(char* raw_command, size_t command_len,
                                 char** command_tokens, uint32_t max_tokens) {
    if (max_tokens < 1) return -1;

    // split string on space
    char delim[] = " ";

    char* token = strtok(raw_command, delim);

    int num_args = 1;

    if (token != NULL) {
        command_tokens[num_args] = token;
        num_args++;
    } else {
        return 1;
    }

    while ((token = strtok(NULL, delim)) != NULL) {
        command_tokens[num_args] = token;

        num_args++;

        if (num_args >= max_tokens) break;
    }

    return num_args;
}

/** COMMAND: playsub
 * - timings are fed as a stream of newline delimited numbers, ending with a 0
 * timing
 */
void AppUSB::start_raw_tx() {
    Serial.println("reading values until 0...");

    SHARED_timing_index = 0;

    char number_buffer[16];
    bool have_read_first_number = false;
    while (true) {
        memset(number_buffer, 0, sizeof(number_buffer));
        Serial.readBytesUntil('\n', number_buffer, sizeof(number_buffer));

        int32_t input_number = atoi(number_buffer);

        if (input_number == 0 && have_read_first_number) break;

        have_read_first_number = true;

        SHARED_timings[SHARED_timing_index] = input_number;

        SHARED_timing_index++;

        if (SHARED_timing_index >= APPUSB_TIMING_BUFFER_SIZE) break;
    }

    // Serial.printf("read %d timings, playing...\n", SHARED_timing_index);

    rp2040.fifo.push(APPUSB_FIFO_COMMAND_START_REPLAY);
}

void AppUSB::stop_rxtx() {
    GLOBAL_config_lock = false;  // release config lock
    SHARED_should_run = false;
    // Serial.printf("stopped current tx/rx\n");
    Serial.println("stopped current tx/rx");
}

void AppUSB::print_help() {
    Serial.println("CC1101 MOBILE CLI");
    Serial.println("made with <3 by inc");
    Serial.println("--frequency,-f set frequency in Hz");
    Serial.println("--preset,-p   set config preset, must be one of");
    Serial.println("               AM270, AM650, FM238, or FM476");
    Serial.println("--stop,-s     stop any active rx or tx");
    Serial.println("--rxraw,-r    begin recieve raw with current config");
    Serial.println("               RSSI printed over cli, RAW data printed when");
    Serial.println("               stopped or buffer full");
    Serial.println("--txraw,-t    begin transmit raw with current config,");
    Serial.println("               timings are fed over cli until 0 encountered");
    Serial.println("--replay,-R   replay last recorded signal");
    Serial.println("--help,-h     print this help");
}