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

#define HIGH_LEVEL_TIME (30) /**< High level duration for pin switch mode in seconds */
#define LOW_LEVEL_TIME  (5)  /**< Low level duration for pin switch mode in seconds */

M5UnitMQ unitMQ; /**< Instance of the Unit MQ gas sensor class */

/* Sensor and system state variables */
static led_status_t ledStatus        = LED_WORK_STATUS_OFF;  /**< Current LED status */
static heat_mode_t heatMode          = HEAT_MODE_PIN_SWITCH; /**< Heating mode of the sensor */
static mq_adc_valid_tags_t validTags = VALID_TAG_INVALID;    /**< Validity tag for MQ sensor ADC data */
static uint8_t highLevelTime         = 0;                    /**< High-level time setting for pin switch heating */
static uint8_t lowLevelTime          = 0;                    /**< Low-level time setting for pin switch heating */
static uint16_t mqAdc12bit           = 0;                    /**< Raw 12-bit ADC reading from MQ sensor */
static uint16_t ntcAdc12bit          = 0;                    /**< Raw 12-bit ADC reading from NTC temperature sensor */
static uint16_t ntcResistance        = 0;                    /**< Calculated resistance of NTC sensor (Ohms) */
static uint16_t referenceVoltage     = 0;                    /**< Reference voltage value (mV) */
static uint16_t mqVoltage            = 0;                    /**< Calculated voltage of MQ sensor output (mV) */
static uint16_t ntcVoltage           = 0;                    /**< Calculated voltage of NTC sensor output (mV) */
float temperature                    = 0.0f;                 /**< Calculated temperature in Celsius */
static uint8_t firmwareVersion       = 0;                    /**< Firmware version of the Unit MQ sensor */

void setup()
{
    M5.begin();
    Serial.begin(115200);

    Serial.println("========== Unit MQ Initialization ==========");

    // Initialize the Unit MQ sensor with I2C settings.
    // Retry initialization until successful.
    while (!unitMQ.begin(&Wire, UNIT_MQ_I2C_BASE_ADDR, I2C_SDA, I2C_SCL, I2C_SPEED)) {
        Serial.println("[ERROR] Unit MQ initialization failed! Retrying...");
        delay(1000);
    }
    Serial.println("[INFO] Unit MQ initialization succeeded.");

    // Set heating mode to PIN SWITCH mode, which alternates heating between HIGH and LOW levels.
    unitMQ.setHeatMode(HEAT_MODE_PIN_SWITCH);
    Serial.println("[INFO] Heating mode set to PIN SWITCH mode.");

    // Configure the durations for high and low heating levels in PIN SWITCH mode.
    unitMQ.setPulseTime(HIGH_LEVEL_TIME, LOW_LEVEL_TIME);
    Serial.println("[INFO] Pin level switch time set to 30s HIGH and 5s LOW.");

    // Turn on the sensor LED indicator.
    unitMQ.setLEDState(LED_WORK_STATUS_ON);
    Serial.println("[INFO] LED status set to ON.");
}

void loop()
{
    // Get the current heating mode
    heatMode = unitMQ.getHeatMode();

    // Read the current LED status from the sensor.
    ledStatus = unitMQ.getLEDState();

    // Retrieve validity tag indicating whether MQ sensor data is reliable.
    validTags = unitMQ.getValidTags();

    // Get the firmware version from the sensor.
    firmwareVersion = unitMQ.getFirmwareVersion();

    // Always read NTC-related sensor data regardless of MQ data validity.
    ntcAdc12bit      = unitMQ.getNTCADC12bit();                  // Raw ADC for NTC sensor
    referenceVoltage = unitMQ.getReferenceVoltage();             // Reference voltage for ADC
    ntcVoltage       = unitMQ.getNTCVoltage();                   // Calculated voltage across NTC
    ntcResistance    = unitMQ.getNTCResistance();                // Calculated resistance of NTC thermistor
    temperature      = unitMQ.getNTCTemperature(ntcResistance);  // Convert resistance to temperature

    Serial.println("======== Unit MQ Status ========");
    Serial.printf("Firmware Version   : %d\n", firmwareVersion);
    Serial.printf("Heating Mode       : %s\n", heatMode == HEAT_MODE_CONTINUOUS ? "CONTINUOUS" : "SWITCH");
    Serial.printf("LED Status         : %s\n", ledStatus == LED_WORK_STATUS_ON ? "ON" : "OFF");

    // Print detailed NTC sensor parameters
    Serial.printf("NTC ADC (12-bit)   : %u\n", ntcAdc12bit);
    Serial.printf("Reference Voltage  : %u mV\n", referenceVoltage);
    Serial.printf("NTC Voltage        : %u mV\n", ntcVoltage);
    Serial.printf("NTC Resistance     : %u Ohm\n", ntcResistance);
    Serial.printf("Temperature        : %.2f °C\n", temperature);

    // If MQ sensor data is valid, read and print MQ ADC and voltage values.
    if (validTags == VALID_TAG_VALID) {
        mqAdc12bit = unitMQ.getMQADC12bit();
        mqVoltage  = unitMQ.getMQVoltage();

        Serial.printf("Valid Tags         : 0x%02X (MQ data valid)\n", validTags);
        Serial.printf("MQ ADC (12-bit)    : %u\n", mqAdc12bit);
        Serial.printf("MQ Voltage         : %u mV\n", mqVoltage);
    } else {
        // If MQ data is invalid, log a warning and skip readings.
        Serial.printf("Valid Tags         : 0x%02X (MQ data invalid)\n", validTags);
        Serial.println("[WARNING] MQ data invalid. Skipping MQ readings.");
    }

    Serial.println("================================\n");

    delay(1000);  // Wait for 1 second before next update
}