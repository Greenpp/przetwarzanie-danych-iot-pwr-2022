#include <ArduinoJson.h>
#include <dht11.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "config.h"

static const u1_t PROGMEM appeui[8] = APPEUI;
static const u1_t PROGMEM deveui[8] = DEVEUI;
static const u1_t PROGMEM appkey[16] = APPKEY;

void os_getArtEui(u1_t *buf) { memcpy_P(buf, appeui, 8); }
void os_getDevEui(u1_t *buf) { memcpy_P(buf, deveui, 8); }
void os_getDevKey(u1_t *buf) { memcpy_P(buf, appkey, 16); }

dht11 DHT;
// size of 48 based on https://arduinojson.org/v6/assistant/
static StaticJsonDocument<48> mydata;
static osjob_t sendjob;
const unsigned TX_INTERVAL = 30; // seconds (might become longer due to duty cycle limitations)
// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 10,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = LMIC_UNUSED_PIN,
    .dio = {2, 3, LMIC_UNUSED_PIN},
};

void onEvent(ev_t ev)
{
    Serial.print(os_getTime());
    Serial.print(": ");
    switch (ev)
    {
    case EV_SCAN_TIMEOUT:
        Serial.println(F("EV_SCAN_TIMEOUT"));
        break;
    case EV_BEACON_FOUND:
        Serial.println(F("EV_BEACON_FOUND"));
        break;
    case EV_BEACON_MISSED:
        Serial.println(F("EV_BEACON_MISSED"));
        break;
    case EV_BEACON_TRACKED:
        Serial.println(F("EV_BEACON_TRACKED"));
        break;
    case EV_JOINING:
        Serial.println(F("EV_JOINING"));
        break;
    case EV_JOINED:
        Serial.println(F("EV_JOINED"));

        // Disable link check validation (automatically enabled
        // during join, but not supported by TTN at this time).
        LMIC_setLinkCheckMode(0);
        break;
    case EV_RFU1:
        Serial.println(F("EV_RFU1"));
        break;
    case EV_JOIN_FAILED:
        Serial.println(F("EV_JOIN_FAILED"));
        break;
    case EV_REJOIN_FAILED:
        Serial.println(F("EV_REJOIN_FAILED"));
        break;
    case EV_TXCOMPLETE:
        Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
        if (LMIC.txrxFlags & TXRX_ACK)
            Serial.println(F("Received ack"));
        if (LMIC.dataLen)
        {
            Serial.println(F("Received "));
            Serial.println(LMIC.dataLen);
            Serial.println(F(" bytes of payload"));
        }
        // Schedule next transmission
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_gather);
        break;
    case EV_LOST_TSYNC:
        Serial.println(F("EV_LOST_TSYNC"));
        break;
    case EV_RESET:
        Serial.println(F("EV_RESET"));
        break;
    case EV_RXCOMPLETE:
        // data received in ping slot
        Serial.println(F("EV_RXCOMPLETE"));
        break;
    case EV_LINK_DEAD:
        Serial.println(F("EV_LINK_DEAD"));
        break;
    case EV_LINK_ALIVE:
        Serial.println(F("EV_LINK_ALIVE"));
        break;
    default:
        Serial.println(F("Unknown event"));
        break;
    }
}

void update_data()
{
    // read data from sensor
    int chk = DHT.read(DHT11_PIN);

    // check if data is valid
    switch (chk)
    {
    case DHTLIB_OK:
        // Update data
        mydata["humidity"] = DHT.humidity;       // %
        mydata["temperature"] = DHT.temperature; // in Celsius
        mydata["type"] = "INFO";
        // Print data
        Serial.print("Humidity: ");
        Serial.print(DHT.humidity);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(DHT.temperature);
        Serial.print(" *C\t");
        Serial.println();
        break;
    case DHTLIB_ERROR_CHECKSUM:
        Serial.println(F("Checksum error"));
        break;
    case DHTLIB_ERROR_TIMEOUT:
        Serial.println(F("Time out error"));
        break;
    default:
        Serial.println(F("Unknown error"));
        break;
    }
}

void do_gather(osjob_t *j)
{
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND)
    {
        Serial.println(F("OP_TXRXPEND, not sending"));
    }
    else
    {
        // Update data
        update_data();
        // TODO check for data abnormality
        // TODO send only every nth time if normal
        // Stringify the data
        String json_str = mydata.as<String>();
        // Print the data
        Serial.println(json_str);
        // Create a buffer for the payload
        uint8_t payload[json_str.length()];
        // Copy the payload into the buffer
        memcpy(payload, json_str.c_str(), json_str.length());
        // Create a packet
        LMIC_setTxData2(1, payload, json_str.length(), 0);
        Serial.println(F("Packet queued"));
    }
}

void setup()
{
    Serial.begin(9600);
    Serial.println(F("Starting"));
    Serial.println("DHT Sensor initialized");
    Serial.print("LIBRARY VERSION: ");
    Serial.println(DHT11LIB_VERSION);

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    // Start job (sending automatically starts OTAA too)
    do_gather(&sendjob);
}

void loop()
{
    os_runloop_once();
}
