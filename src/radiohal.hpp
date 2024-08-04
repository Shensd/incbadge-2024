#ifndef RADIOHAL_HPP
#define RADIOHAL_HPP

#include "flipper_configs.hpp"

namespace radiohal {

const uint8_t CONFIG_PRESET_AM270 = 0;
const uint8_t CONFIG_PRESET_AM650 = 1;
const uint8_t CONFIG_PRESET_FM238 = 2;
const uint8_t CONFIG_PRESET_FM476 = 3;
const uint8_t CONFIG_PRESET_CUSTOM = 4;

const char STR_AM270[] = "AM270";
const char STR_AM650[] = "AM650";
const char STR_FM238[] = "FM238";
const char STR_FM476[] = "FM476";
const char STR_CUSTOM[] = "CUSTOM";

const uint8_t CONFIG_PROTOCOL_RAW = 0;

struct SubFile {
    char name[32];
    uint32_t frequency_hz;
    uint8_t preset_index;
    uint8_t custom_preset_data[56];
    uint8_t custom_protocol_index;
    int32_t timings[4096];
    int16_t num_timings;
};

int16_t load_config(CC1101 radio, const uint8_t* config);

// On/Off Keying, 270kHz bandwidth, async(IO throw GP0)
int16_t config_FuriHalSubGhzPresetOok270Async(CC1101 radio);

// On/Off Keying, 650kHz bandwidth, async(IO throw GP0)
int16_t config_FuriHalSubGhzPresetOok650Async(CC1101 radio);

// 2 Frequency Shift Keying, deviation 2kHz, 270kHz bandwidth, async(IO throw GP0)
int16_t config_FuriHalSubGhzPreset2FSKDev238Async(CC1101 radio);

// 2 Frequency Shift Keying, deviation 47kHz, 270kHz bandwidth, async(IO throw GP0)
int16_t config_FuriHalSubGhzPreset2FSKDev476Async(CC1101 radio);

int16_t set_config_from_string(CC1101 radio, char* config_str);

int16_t set_config_from_short_string(CC1101 radio, char* config_str);

float get_RSSI(CC1101 radio);

float adjust_frequency(float frequency);

int16_t set_config_from_preset_index(CC1101 radio, uint8_t preset_index, const uint8_t* custom_preset_data=NULL);

const char* get_preset_string_from_index(uint8_t preset_index);

}

#endif // !RADIOHAL_HPP