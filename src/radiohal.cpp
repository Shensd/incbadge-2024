#include "radiohal.hpp"

namespace radiohal {

int16_t load_config(CC1101 radio, const uint8_t* config) {
    int16_t status = 0;
    int i = 0;
    while (config[i] != 0) {
        status = radio.SPIsetRegValue(config[i], config[i + 1]);

        if (status != RADIOLIB_ERR_NONE) {
            // Serial.printf("error setting register value; status code: %d;
            // register: %x; value: %x\n", status, config[i], config[i+1]);
            Serial.print("error setting register value; status code: ");
            Serial.print(status);
            Serial.print("; register: ");
            Serial.print(config[i]);
            Serial.print("; value: ");
            Serial.println(config[i + 1]);
            return status;
        }

        i += 2;
    }

    return RADIOLIB_ERR_NONE;
}

// On/Off Keying, 270kHz bandwidth, async(IO throw GP0)
int16_t config_FuriHalSubGhzPresetOok270Async(CC1101 radio) {
    return load_config(radio,
                       subghz_device_cc1101_preset_ook_270khz_async_regs);
}

// On/Off Keying, 650kHz bandwidth, async(IO throw GP0)
int16_t config_FuriHalSubGhzPresetOok650Async(CC1101 radio) {
    return load_config(radio,
                       subghz_device_cc1101_preset_ook_650khz_async_regs);
}

// 2 Frequency Shift Keying, deviation 2kHz, 270kHz bandwidth, async(IO throw
// GP0)
int16_t config_FuriHalSubGhzPreset2FSKDev238Async(CC1101 radio) {
    return load_config(radio,
                       subghz_device_cc1101_preset_2fsk_dev2_38khz_async_regs);
}

// 2 Frequency Shift Keying, deviation 47kHz, 270kHz bandwidth, async(IO throw
// GP0)
int16_t config_FuriHalSubGhzPreset2FSKDev476Async(CC1101 radio) {
    return load_config(radio,
                       subghz_device_cc1101_preset_2fsk_dev47_6khz_async_regs);
}

int16_t set_config_from_string(CC1101 radio, char* config_str) {
    if (strcmp(config_str, "FuriHalSubGhzPresetOok270Async") == 0) {
        return config_FuriHalSubGhzPresetOok270Async(radio);
    } else if (strcmp(config_str, "FuriHalSubGhzPresetOok650Async") == 0) {
        return config_FuriHalSubGhzPresetOok650Async(radio);
    } else if (strcmp(config_str, "FuriHalSubGhzPreset2FSKDev238Async") == 0) {
        return config_FuriHalSubGhzPreset2FSKDev238Async(radio);
    } else if (strcmp(config_str, "FuriHalSubGhzPreset2FSKDev476Async") == 0) {
        return config_FuriHalSubGhzPreset2FSKDev476Async(radio);
    } else {
        return RADIOLIB_ERR_UNKNOWN;
    }
}

int16_t set_config_from_short_string(CC1101 radio, char* config_str) {
    if (strcmp(config_str, STR_AM270) == 0) {
        return config_FuriHalSubGhzPresetOok270Async(radio);
    } else if (strcmp(config_str, STR_AM650) == 0) {
        return config_FuriHalSubGhzPresetOok650Async(radio);
    } else if (strcmp(config_str, STR_FM238) == 0) {
        return config_FuriHalSubGhzPreset2FSKDev238Async(radio);
    } else if (strcmp(config_str, STR_FM476) == 0) {
        return config_FuriHalSubGhzPreset2FSKDev476Async(radio);
    } else {
        return RADIOLIB_ERR_UNKNOWN;
    }
}

float get_RSSI(CC1101 radio) {
    uint8_t rawRssi = radio.SPIreadRegister(RADIOLIB_CC1101_REG_RSSI);
    if (rawRssi >= 128) {
        return ((rawRssi - 256) / 2) - 74;
    } else {
        return (rawRssi / 2) - 74;
    }
}

float adjust_frequency(float frequency) {
    if (frequency < 300) {
        frequency = 300;
    }
    if (frequency > 348 && frequency < 387) {
        if (frequency < 367)
            frequency = 387;
        else
            frequency = 348;
    }
    if (frequency > 464 && frequency < 779) {
        if (frequency < 621)
            frequency = 779;
        else
            frequency = 464;
    }
    if (frequency > 928) {
        frequency = 928;
    }

    return frequency;
}

int16_t set_config_from_preset_index(CC1101 radio, uint8_t preset_index,
                                     const uint8_t* custom_preset_data) {
    switch (preset_index) {
        case CONFIG_PRESET_AM270:
            if (config_FuriHalSubGhzPresetOok270Async(radio) !=
                RADIOLIB_ERR_NONE) {
                Serial.println("error configuring radio to AM270 config");
                return RADIOLIB_ERR_NONE;
            }
            break;
        case CONFIG_PRESET_AM650:
            if (config_FuriHalSubGhzPresetOok650Async(radio) !=
                RADIOLIB_ERR_NONE) {
                Serial.println("error configuring radio to AM650 config");
                return RADIOLIB_ERR_NONE;
            }
            break;
        case CONFIG_PRESET_FM238:
            if (config_FuriHalSubGhzPreset2FSKDev238Async(radio) !=
                RADIOLIB_ERR_NONE) {
                Serial.println("error configuring radio to FM238 config");
                return RADIOLIB_ERR_NONE;
            }
            break;
        case CONFIG_PRESET_FM476:
            if (config_FuriHalSubGhzPreset2FSKDev476Async(radio) !=
                RADIOLIB_ERR_NONE) {
                Serial.println("error configuring radio to FM476 config");
                return RADIOLIB_ERR_NONE;
            }
            break;
        case CONFIG_PRESET_CUSTOM:
            if (custom_preset_data == NULL) {
                Serial.println(
                    "attempted to set custom configuation with null "
                    "preset_data pointer");
                return RADIOLIB_ERR_UNKNOWN;
            }
            if (load_config(radio, custom_preset_data) != RADIOLIB_ERR_NONE) {
                Serial.println("error loading custom config (uh oh)");
                return RADIOLIB_ERR_NONE;
            }
            break;
        default:
            break;
    }

    return RADIOLIB_ERR_UNKNOWN;
}

const char* get_preset_string_from_index(uint8_t preset_index) {
    switch (preset_index) {
        case radiohal::CONFIG_PRESET_AM270:
            return STR_AM270;
            break;
        case radiohal::CONFIG_PRESET_AM650:
            return STR_AM650;
            break;
        case radiohal::CONFIG_PRESET_FM238:
            return STR_FM238;
            break;
        case radiohal::CONFIG_PRESET_FM476:
            return STR_FM476;
            break;
        case radiohal::CONFIG_PRESET_CUSTOM:
            return STR_CUSTOM;
            break;
    }

    return NULL;
}
}  // namespace radiohal
