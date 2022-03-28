#ifndef _lmic_config_h_
#define _lmic_config_h_

#define CFG_eu868 1
#define CFG_sx1276_radio 1
#define US_PER_OSTICK_EXPONENT 4
#define US_PER_OSTICK (1 << US_PER_OSTICK_EXPONENT)
#define OSTICKS_PER_SEC (1000000 / US_PER_OSTICK)
#define LMIC_DEBUG_LEVEL 2
#define LMIC_PRINTF_TO Serial
#define LMIC_FAILURE_TO Serial
#define DISABLE_PING
#define DISABLE_BEACONS
#define USE_IDEETRON_AES

#endif // _lmic_config_h_
