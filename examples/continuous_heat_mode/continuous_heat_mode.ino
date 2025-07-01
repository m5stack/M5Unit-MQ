/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
 * @Hardwares: Core + Unit MQ
 * @Dependent Library:
 * M5Unified@^0.2.2: https://github.com/m5stack/M5Unified
 * M5GFX@^0.2.3: https://github.com/m5stack/M5GFX
 * M5Unit-MQ: https://github.com/m5stack/M5Unit-MQ.git
 */

#include <M5Unified.h>
#include "m5_unit_mq.hpp"

#define I2C_SDA   (21)     /**< I2C SDA pin number */
#define I2C_SCL   (22)     /**< I2C SCL pin number */
#define I2C_SPEED (400000) /**< I2C bus speed in Hz */

M5UnitMQ unitMQ; /**< Instance of the Unit MQ gas sensor */

/* Sensor status and data variables */
static led_status_t ledStatus        = LED_WORK_STATUS_OFF;  /**< Current LED state */
static heat_mode_t heatMode          = HEAT_MODE_CONTINUOUS; /**< Current heating mode */
static mq_adc_valid_tags_t validTags = VALID_TAG_INVALID;    /**< MQ ADC data validity flag */
static uint8_t highLevelTime         = 0;                    /**< High-level heating time (not used here) */
static uint8_t lowLevelTime          = 0;                    /**< Low-level heating time (not used here) */
static uint16_t mqADC12bit           = 0;                    /**< MQ sensor raw ADC value (12-bit) */
static uint16_t ntcADC12bit          = 0;                    /**< NTC sensor raw ADC value (12-bit) */
static uint16_t ntcResistance        = 0;                    /**< Calculated NTC resistance in Ohms */
static uint16_t referenceVoltage     = 0;                    /**< Reference voltage in millivolts */
static uint16_t mqVoltage            = 0;                    /**< Calculated MQ sensor voltage in millivolts */
static uint16_t ntcVoltage           = 0;                    /**< Calculated NTC sensor voltage in millivolts */
float temperature                    = 0.0f;                 /**< Calculated temperature in degrees Celsius */
static uint8_t firmwareVersion       = 0;                    /**< Sensor firmware version */

/**
 * @brief Arduino setup function, runs once at startup.
 *
 * Initializes the M5 hardware and Unit MQ sensor with I2C configuration.
 * Retries sensor initialization until successful.
 * Sets sensor heating mode and turns on the sensor LED.
 */
void setup()
{
    M5.begin();
    Serial.begin(115200);

    Serial.println("========== Unit MQ Initialization ==========");

    // Initialize Unit MQ sensor on I2C bus, retry until success
    while (!unitMQ.begin(&Wire, UNIT_MQ_I2C_BASE_ADDR, I2C_SDA, I2C_SCL, I2C_SPEED)) {
        Serial.println("[ERROR] Unit MQ initialization failed! Retrying...");
        delay(1000);
    }
    Serial.println("[INFO] Unit MQ initialization succeeded.");

    // Set the heating mode to continuous heating
    unitMQ.setHeatMode(HEAT_MODE_CONTINUOUS);
    Serial.println("[INFO] Heating mode set to CONTINUOUS.");

    // Turn on the sensor's LED indicator
    unitMQ.setLEDState(LED_WORK_STATUS_ON);
    Serial.println("[INFO] LED status set to ON.");
}

/**
 * @brief Arduino main loop function, runs repeatedly.
 *
 * Reads sensor data including LED status, valid tags, firmware version,
 * NTC sensor ADC and temperature, and MQ sensor ADC and voltage if valid.
 * Prints all the relevant data to Serial for monitoring.
 */
void loop()
{
    // Get the current heating mode
    heatMode = unitMQ.getHeatMode();

    // Retrieve current LED state
    ledStatus = unitMQ.getLEDState();

    // Retrieve validity tag for MQ sensor data
    validTags = unitMQ.getValidTags();

    // Retrieve sensor firmware version
    firmwareVersion = unitMQ.getFirmwareVersion();

    // Always read NTC sensor data and calculate temperature
    ntcADC12bit      = unitMQ.getNTCADC12bit();
    referenceVoltage = unitMQ.getReferenceVoltage();
    ntcVoltage       = unitMQ.getNTCVoltage();
    ntcResistance    = unitMQ.getNTCResistance();
    temperature      = unitMQ.getNTCTemperature(ntcResistance);

    // Print sensor status and readings
    Serial.println("======== Unit MQ Status ========");
    Serial.printf("Firmware Version   : %d\n", firmwareVersion);
    Serial.printf("Heating Mode       : %s\n", heatMode == HEAT_MODE_CONTINUOUS ? "CONTINUOUS" : "SWITCH");
    Serial.printf("LED Status         : %s\n", ledStatus == LED_WORK_STATUS_ON ? "ON" : "OFF");
    Serial.printf("NTC ADC (12-bit)   : %u\n", ntcADC12bit);
    Serial.printf("Reference Voltage  : %u mV\n", referenceVoltage);
    Serial.printf("NTC Voltage        : %u mV\n", ntcVoltage);
    Serial.printf("NTC Resistance     : %u Ohm\n", ntcResistance);
    Serial.printf("Temperature        : %.2f °C\n", temperature);

    // Print MQ sensor readings only if data is valid
    if (validTags == VALID_TAG_VALID) {
        mqADC12bit = unitMQ.getMQADC12bit();
        mqVoltage  = unitMQ.getMQVoltage();

        Serial.printf("Valid Tags         : 0x%02X (MQ data valid)\n", validTags);
        Serial.printf("MQ ADC (12-bit)    : %u\n", mqADC12bit);
        Serial.printf("MQ Voltage         : %u mV\n", mqVoltage);
    } else {
        Serial.printf("Valid Tags         : 0x%02X (MQ data invalid)\n", validTags);
        Serial.println("[WARNING] MQ data invalid. Skipping MQ readings.");
    }

    Serial.println("================================\n");

    delay(1000);  // Delay 1 second before next reading
}