#include "radiohal.hpp"

namespace radiohal
{

    uint16_t load_config(CC1101 radio, const uint8_t *config) {
        uint16_t status = 0;
        int i = 0;
        while (config[i] != 0)
        {
            status = radio.SPIsetRegValue(config[i], config[i + 1]);

            if (status != RADIOLIB_ERR_NONE)
            {
                // Serial.printf("error setting register value; status code: %d; register: %x; value: %x\n", status, config[i], config[i+1]);
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
    uint16_t config_FuriHalSubGhzPresetOok270Async(CC1101 radio) {
        return load_config(radio, subghz_device_cc1101_preset_ook_270khz_async_regs);
    }

    // On/Off Keying, 650kHz bandwidth, async(IO throw GP0)
    uint16_t config_FuriHalSubGhzPresetOok650Async(CC1101 radio) {
        return load_config(radio, subghz_device_cc1101_preset_ook_650khz_async_regs);
    }

    // 2 Frequency Shift Keying, deviation 2kHz, 270kHz bandwidth, async(IO throw GP0)
    uint16_t config_FuriHalSubGhzPreset2FSKDev238Async(CC1101 radio) {
        return load_config(radio, subghz_device_cc1101_preset_2fsk_dev2_38khz_async_regs);
    }

    // 2 Frequency Shift Keying, deviation 47kHz, 270kHz bandwidth, async(IO throw GP0)
    uint16_t config_FuriHalSubGhzPreset2FSKDev476Async(CC1101 radio) {
        return load_config(radio, subghz_device_cc1101_preset_2fsk_dev47_6khz_async_regs);
    }

    uint16_t set_config_from_string(CC1101 radio, char *config_str) {
        if (strcmp(config_str, "FuriHalSubGhzPresetOok270Async") == 0)
        {
            return config_FuriHalSubGhzPresetOok270Async(radio);
        }
        else if (strcmp(config_str, "FuriHalSubGhzPresetOok650Async") == 0)
        {
            return config_FuriHalSubGhzPresetOok650Async(radio);
        }
        else if (strcmp(config_str, "FuriHalSubGhzPreset2FSKDev238Async") == 0)
        {
            return config_FuriHalSubGhzPreset2FSKDev238Async(radio);
        }
        else if (strcmp(config_str, "FuriHalSubGhzPreset2FSKDev476Async") == 0)
        {
            return config_FuriHalSubGhzPreset2FSKDev476Async(radio);
        }
        else
        {
            return RADIOLIB_ERR_UNKNOWN;
        }
    }

    float get_RSSI(CC1101 radio) {
        uint8_t rawRssi = radio.SPIreadRegister(RADIOLIB_CC1101_REG_RSSI);
        if (rawRssi >= 128)
        {
            return ((rawRssi - 256) / 2) - 74;
        }
        else
        {
            return (rawRssi / 2) - 74;
        }
    }

}
