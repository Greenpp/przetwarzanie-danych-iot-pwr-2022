# Temperature and Humidity Sensor + The Things Network

## Setup

1. Install libraries

    - IBM LMIC framework 1.5.1
    - ArduinoJson 6.19.3
    - DHTlib 0.1.35

2. Update lmic config

    ```sh
    cp lmic-config.h ~/Arduino/libraries/IBM_LMIC_framework/src/lmic/config.h
    ```

3. Create config.h

    ```sh
    cp config.h.template config.h
    ```

4. Set values in config.h
