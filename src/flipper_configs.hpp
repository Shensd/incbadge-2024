#ifndef FLIPPER_CONFIGS_H
#define FLIPPER_CONFIGS_H

#include <RadioLib.h>
#include <stdint.h>

// from 
// https://github.com/flipperdevices/flipperzero-firmware/blob/a47a55bfee0c0bbd5724953d89144f55544f6a52/lib/subghz/devices/cc1101_configs.c

const uint8_t subghz_device_cc1101_preset_ook_270khz_async_regs[] = {
    // https://e2e.ti.com/support/wireless-connectivity/sub-1-ghz-group/sub-1-ghz/f/sub-1-ghz-forum/382066/cc1101---don-t-know-the-correct-registers-configuration

    /* GPIO GD0 */
    RADIOLIB_CC1101_REG_IOCFG0,
    0x0D, // GD0 as async serial data output/input

    /* FIFO and internals */
    RADIOLIB_CC1101_REG_FIFOTHR,
    0x47, // The only important bit is ADC_RETENTION, FIFO Tx=33 Rx=32

    /* Packet engine */
    RADIOLIB_CC1101_REG_PKTCTRL0,
    0x32, // Async, continious, no whitening

    /* Frequency Synthesizer Control */
    RADIOLIB_CC1101_REG_FSCTRL1,
    0x06, // IF = (26*10^6) / (2^10) * 0x06 = 152343.75Hz

    // Modem Configuration
    RADIOLIB_CC1101_REG_MDMCFG0,
    0x00, // Channel spacing is 25kHz
    RADIOLIB_CC1101_REG_MDMCFG1,
    0x00, // Channel spacing is 25kHz
    RADIOLIB_CC1101_REG_MDMCFG2,
    0x30, // Format ASK/OOK, No preamble/sync
    RADIOLIB_CC1101_REG_MDMCFG3,
    0x32, // Data rate is 3.79372 kBaud
    RADIOLIB_CC1101_REG_MDMCFG4,
    0x67, // Rx BW filter is 270.833333kHz

    /* Main Radio Control State Machine */
    RADIOLIB_CC1101_REG_MCSM0,
    0x18, // Autocalibrate on idle-to-rx/tx, PO_TIMEOUT is 64 cycles(149-155us)

    /* Frequency Offset Compensation Configuration */
    RADIOLIB_CC1101_REG_FOCCFG,
    0x18, // no frequency offset compensation, POST_K same as PRE_K, PRE_K is 4K, GATE is off

    /* Automatic Gain Control */
    RADIOLIB_CC1101_REG_AGCCTRL0,
    0x40, // 01 - Low hysteresis, small asymmetric dead zone, medium gain; 00 - 8 samples agc; 00 - Normal AGC, 00 - 4dB boundary
    RADIOLIB_CC1101_REG_AGCCTRL1,
    0x00, // 0; 0 - LNA 2 gain is decreased to minimum before decreasing LNA gain; 00 - Relative carrier sense threshold disabled; 0000 - RSSI to MAIN_TARGET
    RADIOLIB_CC1101_REG_AGCCTRL2,
    0x03, // 00 - DVGA all; 000 - MAX LNA+LNA2; 011 - MAIN_TARGET 24 dB

    /* Wake on radio and timeouts control */
    RADIOLIB_CC1101_REG_WORCTRL,
    0xFB, // WOR_RES is 2^15 periods (0.91 - 0.94 s) 16.5 - 17.2 hours

    /* Frontend configuration */
    RADIOLIB_CC1101_REG_FREND0,
    0x11, // Adjusts current TX LO buffer + high is PATABLE[1]
    RADIOLIB_CC1101_REG_FREND1,
    0xB6, //

    /* End load reg */
    0,
    0,

    //ook_async_patable[8]
    0x00,
    0xC0, // 12dBm 0xC0, 10dBm 0xC5, 7dBm 0xCD, 5dBm 0x86, 0dBm 0x50, -6dBm 0x37, -10dBm 0x26, -15dBm 0x1D, -20dBm 0x17, -30dBm 0x03
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
};

const uint8_t subghz_device_cc1101_preset_ook_650khz_async_regs[] = {
    // https://e2e.ti.com/support/wireless-connectivity/sub-1-ghz-group/sub-1-ghz/f/sub-1-ghz-forum/382066/cc1101---don-t-know-the-correct-registers-configuration

    /* GPIO GD0 */
    RADIOLIB_CC1101_REG_IOCFG0,
    0x0D, // GD0 as async serial data output/input

    /* FIFO and internals */
    RADIOLIB_CC1101_REG_FIFOTHR,
    0x07, // The only important bit is ADC_RETENTION

    /* Packet engine */
    RADIOLIB_CC1101_REG_PKTCTRL0,
    0x32, // Async, continious, no whitening

    /* Frequency Synthesizer Control */
    RADIOLIB_CC1101_REG_FSCTRL1,
    0x06, // IF = (26*10^6) / (2^10) * 0x06 = 152343.75Hz

    // Modem Configuration
    RADIOLIB_CC1101_REG_MDMCFG0,
    0x00, // Channel spacing is 25kHz
    RADIOLIB_CC1101_REG_MDMCFG1,
    0x00, // Channel spacing is 25kHz
    RADIOLIB_CC1101_REG_MDMCFG2,
    0x30, // Format ASK/OOK, No preamble/sync
    RADIOLIB_CC1101_REG_MDMCFG3,
    0x32, // Data rate is 3.79372 kBaud
    RADIOLIB_CC1101_REG_MDMCFG4,
    0x17, // Rx BW filter is 650.000kHz

    /* Main Radio Control State Machine */
    RADIOLIB_CC1101_REG_MCSM0,
    0x18, // Autocalibrate on idle-to-rx/tx, PO_TIMEOUT is 64 cycles(149-155us)

    /* Frequency Offset Compensation Configuration */
    RADIOLIB_CC1101_REG_FOCCFG,
    0x18, // no frequency offset compensation, POST_K same as PRE_K, PRE_K is 4K, GATE is off

    /* Automatic Gain Control */
    // RADIOLIB_CC1101_REG_AGCTRL0,0x40, // 01 - Low hysteresis, small asymmetric dead zone, medium gain; 00 - 8 samples agc; 00 - Normal AGC, 00 - 4dB boundary
    // RADIOLIB_CC1101_REG_AGCTRL1,0x00, // 0; 0 - LNA 2 gain is decreased to minimum before decreasing LNA gain; 00 - Relative carrier sense threshold disabled; 0000 - RSSI to MAIN_TARGET
    // RADIOLIB_CC1101_REG_AGCCTRL2, 0x03, // 00 - DVGA all; 000 - MAX LNA+LNA2; 011 - MAIN_TARGET 24 dB
    //MAGN_TARGET for RX filter BW =< 100 kHz is 0x3. For higher RX filter BW's MAGN_TARGET is 0x7.
    RADIOLIB_CC1101_REG_AGCCTRL0,
    0x91, // 10 - Medium hysteresis, medium asymmetric dead zone, medium gain ; 01 - 16 samples agc; 00 - Normal AGC, 01 - 8dB boundary
    RADIOLIB_CC1101_REG_AGCCTRL1,
    0x0, // 0; 0 - LNA 2 gain is decreased to minimum before decreasing LNA gain; 00 - Relative carrier sense threshold disabled; 0000 - RSSI to MAIN_TARGET
    RADIOLIB_CC1101_REG_AGCCTRL2,
    0x07, // 00 - DVGA all; 000 - MAX LNA+LNA2; 111 - MAIN_TARGET 42 dB

    /* Wake on radio and timeouts control */
    RADIOLIB_CC1101_REG_WORCTRL,
    0xFB, // WOR_RES is 2^15 periods (0.91 - 0.94 s) 16.5 - 17.2 hours

    /* Frontend configuration */
    RADIOLIB_CC1101_REG_FREND0,
    0x11, // Adjusts current TX LO buffer + high is PATABLE[1]
    RADIOLIB_CC1101_REG_FREND1,
    0xB6, //

    /* End load reg */
    0,
    0,

    //ook_async_patable[8]
    0x00,
    0xC0, // 12dBm 0xC0, 10dBm 0xC5, 7dBm 0xCD, 5dBm 0x86, 0dBm 0x50, -6dBm 0x37, -10dBm 0x26, -15dBm 0x1D, -20dBm 0x17, -30dBm 0x03
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
};

const uint8_t subghz_device_cc1101_preset_2fsk_dev2_38khz_async_regs[] = {

    /* GPIO GD0 */
    RADIOLIB_CC1101_REG_IOCFG0,
    0x0D, // GD0 as async serial data output/input

    /* Frequency Synthesizer Control */
    RADIOLIB_CC1101_REG_FSCTRL1,
    0x06, // IF = (26*10^6) / (2^10) * 0x06 = 152343.75Hz

    /* Packet engine */
    RADIOLIB_CC1101_REG_PKTCTRL0,
    0x32, // Async, continious, no whitening
    RADIOLIB_CC1101_REG_PKTCTRL1,
    0x04,

    // // Modem Configuration
    RADIOLIB_CC1101_REG_MDMCFG0,
    0x00,
    RADIOLIB_CC1101_REG_MDMCFG1,
    0x02,
    RADIOLIB_CC1101_REG_MDMCFG2,
    0x04, // Format 2-FSK/FM, No preamble/sync, Disable (current optimized)
    RADIOLIB_CC1101_REG_MDMCFG3,
    0x83, // Data rate is 4.79794 kBaud
    RADIOLIB_CC1101_REG_MDMCFG4,
    0x67, //Rx BW filter is 270.833333 kHz
    RADIOLIB_CC1101_REG_DEVIATN,
    0x04, //Deviation 2.380371 kHz

    /* Main Radio Control State Machine */
    RADIOLIB_CC1101_REG_MCSM0,
    0x18, // Autocalibrate on idle-to-rx/tx, PO_TIMEOUT is 64 cycles(149-155us)

    /* Frequency Offset Compensation Configuration */
    RADIOLIB_CC1101_REG_FOCCFG,
    0x16, // no frequency offset compensation, POST_K same as PRE_K, PRE_K is 4K, GATE is off

    /* Automatic Gain Control */
    RADIOLIB_CC1101_REG_AGCCTRL0,
    0x91, //10 - Medium hysteresis, medium asymmetric dead zone, medium gain ; 01 - 16 samples agc; 00 - Normal AGC, 01 - 8dB boundary
    RADIOLIB_CC1101_REG_AGCCTRL1,
    0x00, // 0; 0 - LNA 2 gain is decreased to minimum before decreasing LNA gain; 00 - Relative carrier sense threshold disabled; 0000 - RSSI to MAIN_TARGET
    RADIOLIB_CC1101_REG_AGCCTRL2,
    0x07, // 00 - DVGA all; 000 - MAX LNA+LNA2; 111 - MAIN_TARGET 42 dB

    /* Wake on radio and timeouts control */
    RADIOLIB_CC1101_REG_WORCTRL,
    0xFB, // WOR_RES is 2^15 periods (0.91 - 0.94 s) 16.5 - 17.2 hours

    /* Frontend configuration */
    RADIOLIB_CC1101_REG_FREND0,
    0x10, // Adjusts current TX LO buffer
    RADIOLIB_CC1101_REG_FREND1,
    0x56,

    /* End load reg */
    0,
    0,

    // 2fsk_async_patable[8]
    0xC0, // 10dBm 0xC0, 7dBm 0xC8, 5dBm 0x84, 0dBm 0x60, -10dBm 0x34, -15dBm 0x1D, -20dBm 0x0E, -30dBm 0x12
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
};

const uint8_t subghz_device_cc1101_preset_2fsk_dev47_6khz_async_regs[] = {

    /* GPIO GD0 */
    RADIOLIB_CC1101_REG_IOCFG0,
    0x0D, // GD0 as async serial data output/input

    /* Frequency Synthesizer Control */
    RADIOLIB_CC1101_REG_FSCTRL1,
    0x06, // IF = (26*10^6) / (2^10) * 0x06 = 152343.75Hz

    /* Packet engine */
    RADIOLIB_CC1101_REG_PKTCTRL0,
    0x32, // Async, continious, no whitening
    RADIOLIB_CC1101_REG_PKTCTRL1,
    0x04,

    // // Modem Configuration
    RADIOLIB_CC1101_REG_MDMCFG0,
    0x00,
    RADIOLIB_CC1101_REG_MDMCFG1,
    0x02,
    RADIOLIB_CC1101_REG_MDMCFG2,
    0x04, // Format 2-FSK/FM, No preamble/sync, Disable (current optimized)
    RADIOLIB_CC1101_REG_MDMCFG3,
    0x83, // Data rate is 4.79794 kBaud
    RADIOLIB_CC1101_REG_MDMCFG4,
    0x67, //Rx BW filter is 270.833333 kHz
    RADIOLIB_CC1101_REG_DEVIATN,
    0x47, //Deviation 47.60742 kHz

    /* Main Radio Control State Machine */
    RADIOLIB_CC1101_REG_MCSM0,
    0x18, // Autocalibrate on idle-to-rx/tx, PO_TIMEOUT is 64 cycles(149-155us)

    /* Frequency Offset Compensation Configuration */
    RADIOLIB_CC1101_REG_FOCCFG,
    0x16, // no frequency offset compensation, POST_K same as PRE_K, PRE_K is 4K, GATE is off

    /* Automatic Gain Control */
    RADIOLIB_CC1101_REG_AGCCTRL0,
    0x91, //10 - Medium hysteresis, medium asymmetric dead zone, medium gain ; 01 - 16 samples agc; 00 - Normal AGC, 01 - 8dB boundary
    RADIOLIB_CC1101_REG_AGCCTRL1,
    0x00, // 0; 0 - LNA 2 gain is decreased to minimum before decreasing LNA gain; 00 - Relative carrier sense threshold disabled; 0000 - RSSI to MAIN_TARGET
    RADIOLIB_CC1101_REG_AGCCTRL2,
    0x07, // 00 - DVGA all; 000 - MAX LNA+LNA2; 111 - MAIN_TARGET 42 dB

    /* Wake on radio and timeouts control */
    RADIOLIB_CC1101_REG_WORCTRL,
    0xFB, // WOR_RES is 2^15 periods (0.91 - 0.94 s) 16.5 - 17.2 hours

    /* Frontend configuration */
    RADIOLIB_CC1101_REG_FREND0,
    0x10, // Adjusts current TX LO buffer
    RADIOLIB_CC1101_REG_FREND1,
    0x56,

    /* End load reg */
    0,
    0,

    // 2fsk_async_patable[8]
    0xC0, // 10dBm 0xC0, 7dBm 0xC8, 5dBm 0x84, 0dBm 0x60, -10dBm 0x34, -15dBm 0x1D, -20dBm 0x0E, -30dBm 0x12
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
};

#endif // !FLIPPER_CONFIGS_H