#ifndef RADIOHAL_HPP
#define RADIOHAL_HPP

#include "flipper_configs.hpp"

namespace radiohal {

uint16_t load_config(CC1101 radio, const uint8_t* config);

// On/Off Keying, 270kHz bandwidth, async(IO throw GP0)
uint16_t config_FuriHalSubGhzPresetOok270Async(CC1101 radio);

// On/Off Keying, 650kHz bandwidth, async(IO throw GP0)
uint16_t config_FuriHalSubGhzPresetOok650Async(CC1101 radio);

// 2 Frequency Shift Keying, deviation 2kHz, 270kHz bandwidth, async(IO throw GP0)
uint16_t config_FuriHalSubGhzPreset2FSKDev238Async(CC1101 radio);

// 2 Frequency Shift Keying, deviation 47kHz, 270kHz bandwidth, async(IO throw GP0)
uint16_t config_FuriHalSubGhzPreset2FSKDev476Async(CC1101 radio);

uint16_t set_config_from_string(CC1101 radio, char* config_str);

float get_RSSI(CC1101 radio);

float adjust_frequency(float frequency);

}

#endif // !RADIOHAL_HPP