/*
 *SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 *SPDX-License-Identifier: MIT
 */

#ifndef _M5_UNIT_MQ_H_
#define _M5_UNIT_MQ_H_

#include "Arduino.h"
#include "Wire.h"

// #define UNIT_MQ_DEBUG Serial // The corresponding serial port must be
// initialized before use This macro definition can be annotated without sending
// and receiving data prints Define the serial port you want to use, e.g.,
// Serial1 or Serial2
#if defined UNIT_MQ_DEBUG
#define serialPrint(...)   UNIT_MQ_DEBUG.print(__VA_ARGS__)
#define serialPrintln(...) UNIT_MQ_DEBUG.println(__VA_ARGS__)
#define serialPrintf(...)  UNIT_MQ_DEBUG.printf(__VA_ARGS__)
#define serialFlush()      UNIT_MQ_DEBUG.flush()
#else
#endif

/**
 * @brief Default I2C address of the Unit MQ device.
 */
#define UNIT_MQ_I2C_BASE_ADDR (0x11)

/**
 * @brief Minimum allowed I2C address value.
 */
#define UNIT_MQ_I2C_ADDR_MIN (0x08)

/**
 * @brief Maximum allowed I2C address value.
 */
#define UNIT_MQ_I2C_ADDR_MAX (0x77)

/**
 * @brief Register address for configuring the operating mode.
 */
#define UNIT_MQ_MODE_CONTROL_REG (0x00)

/**
 * @brief Register address for controlling the LED state.
 */
#define UNIT_MQ_LED_CONTROL_REG (0x01)

/**
 * @brief Register address for setting the heating high-level pulse duration.
 */
#define UNIT_MQ_HIGH_PULSE_CONTROL_REG (0x10)

/**
 * @brief Register address for setting the heating low-level pulse duration.
 */
#define UNIT_MQ_LOW_PULSE_CONTROL_REG (0x11)

/**
 * @brief Register address containing the 8-bit MQ sensor ADC value.
 */
#define UNIT_MQ_MQ_8B_ADC_REG (0x20)

/**
 * @brief Register address containing the 12-bit MQ sensor ADC value.
 */
#define UNIT_MQ_MQ_12B_ADC_REG (0x30)

/**
 * @brief Register address indicating the validity of MQ ADC data.
 */
#define UNIT_MQ_TAGS_REG (0x40)

/**
 * @brief Register address containing the 8-bit NTC temperature sensor ADC value.
 */
#define UNIT_MQ_NTC_8B_ADC_REG (0x50)

/**
 * @brief Register address containing the 12-bit NTC temperature sensor ADC value.
 */
#define UNIT_MQ_NTC_12B_ADC_REG (0x60)

/**
 * @brief Register address containing the measured NTC resistance value.
 */
#define UNIT_MQ_NTC_RESISTANCE_REG (0x70)

/**
 * @brief Register address containing the internal reference voltage in millivolts.
 */
#define UNIT_MQ_INTERNAL_VOLTAGE_REG (0x80)

/**
 * @brief Register address containing the measured voltage from the MQ sensor channel.
 */
#define UNIT_MQ_MQ_VOLTAGE_REG (0x82)

/**
 * @brief Register address containing the measured voltage from the NTC sensor channel.
 */
#define UNIT_MQ_NTC_VOLTAGE_REG (0x84)

/**
 * @brief Register address containing the firmware version number.
 */
#define UNIT_MQ_FIRMWARE_VERSION_REG (0xFE)

/**
 * @brief Register address used to read or set the device I2C address.
 */
#define UNIT_MQ_I2C_ADDRESS_REG (0xFF)

/**
 * @brief Reference resistance value (10 kΩ) used for NTC temperature calculation.
 */
#define RESISTANCE_REFERENCE (10000.0f)

/**
 * @brief B constant of the thermistor, used for Steinhart-Hart equation calculation.
 */
#define THERMISTOR_B_CONSTANT (3950.0f)

/**
 * @brief Constant representing absolute zero temperature (0 °C in Kelvin).
 */
#define ABSOLUTE_ZERO_TEMP (273.15f)

/**
 * @brief Absolute temperature equivalent to 25 °C (in Kelvin).
 */
#define ABSOLUTE_TEMP_AT_25C (ABSOLUTE_ZERO_TEMP + 25.0f)

/**
 * @brief Enum for heart operation modes.
 *
 * This enum defines different modes for heart operation.
 */
typedef enum {
    HEAT_MODE_OFF        = 0,  // 0: OFF mode (default on power-up)
    HEAT_MODE_CONTINUOUS = 1,  // 1: Continuous heating mode
    HEAT_MODE_PIN_SWITCH = 2   // 2: Pin level switch mode
} heat_mode_t;

/**
 * @brief Enum for LED work status.
 *
 * This enum defines the different work status of the LED.
 */
typedef enum {
    LED_WORK_STATUS_OFF = 0,  // 0: LED OFF (default on power-up)
    LED_WORK_STATUS_ON  = 1   // 1: LED ON
} led_status_t;

/**
 * @brief Valid tags for MQ ADC value.
 *
 * This enum defines whether the MQ ADC value is valid or not.
 */
typedef enum {
    VALID_TAG_INVALID = 0,  // The ADC value is invalid.
    VALID_TAG_VALID   = 1   // The ADC value is invalid.
} mq_adc_valid_tags_t;

class M5UnitMQ {
public:
    /**
     * @brief Initializes the device with optional I2C settings.
     *
     * This function configures the I2C communication settings, allowing the user
     * to specify custom SDA and SCL pins as well as the I2C speed. If no
     * parameters are provided, default values are used. The device is initialized
     * using the provided I2C settings, and it returns a success flag.
     *
     * @param wire   Pointer to the TwoWire object for I2C communication (default
     * is &Wire).
     * @param addr   The I2C address of the device (default is -1, meaning use the
     * default address).
     * @param sda    The SDA pin number (default is -1, meaning use the default
     * SDA pin).
     * @param scl    The SCL pin number (default is -1, meaning use the default
     * SCL pin).
     * @param speed  The I2C bus speed in Hz (default is 4000000L).
     *
     * @return True if initialization was successful, false otherwise.
     */
    bool begin(TwoWire *wire = &Wire, uint8_t addr = -1, uint8_t sda = -1, uint8_t scl = -1, uint32_t speed = 4000000L);

    /**
     * @brief Sets the heat mode.
     *
     * Sets the heat mode of the device.
     *
     * @param mode The desired heat mode to set.
     */
    void setHeatMode(heat_mode_t mode);

    /**
     * @brief Gets the current heat mode.
     *
     * Retrieves the current heat mode of the device.
     *
     * @return The current heat mode.
     */
    heat_mode_t getHeatMode(void);

    /**
     * @brief Sets the LED state.
     *
     * Sets the status of the LED indicator.
     *
     * @param status The desired LED state.
     */
    void setLEDState(led_status_t status);

    /**
     * @brief Gets the current LED state.
     *
     * Retrieves the current status of the LED indicator.
     *
     * @return The current LED state.
     */
    led_status_t getLEDState(void);

    /**
     * @brief Sets the pulse level times.
     *
     * Sets the high and low level pulse times for the heating cycle.
     *
     * @param highLevelTime High level pulse time in seconds.
     * @param lowLevelTime  Low level pulse time in seconds.
     */
    void setPulseTime(uint8_t highLevelTime, uint8_t lowLevelTime);

    /**
     * @brief Gets the pulse level times.
     *
     * Retrieves the configured high and low level pulse times.
     *
     * @param highLevelTime Pointer to store the high level time in seconds.
     * @param lowLevelTime  Pointer to store the low level time in seconds.
     */
    void getPulseLevelTime(uint8_t *highLevelTime, uint8_t *lowLevelTime);

    /**
     * @brief Gets the 8-bit ADC value from the MQ sensor.
     *
     * Retrieves the 8-bit ADC value from the MQ sensor.
     *
     * @return The 8-bit ADC value.
     */
    uint8_t getMQADC8bit(void);

    /**
     * @brief Gets the 12-bit ADC value from the MQ sensor.
     *
     * Retrieves the 12-bit ADC value from the MQ sensor.
     *
     * @return The 12-bit ADC value.
     */
    uint16_t getMQADC12bit(void);

    /**
     * @brief Gets the ADC validity tags.
     *
     * Retrieves the status indicating whether the ADC readings are valid.
     *
     * @return Valid tags as an mq_adc_valid_tags_t value.
     */
    mq_adc_valid_tags_t getValidTags(void);

    /**
     * @brief Gets the 8-bit ADC value from the NTC sensor.
     *
     * Retrieves the 8-bit ADC value from the NTC sensor.
     *
     * @return The 8-bit ADC value.
     */
    uint8_t getNTCADC8bit(void);

    /**
     * @brief Gets the 12-bit ADC value from the NTC sensor.
     *
     * Retrieves the 12-bit ADC value from the NTC sensor.
     *
     * @return The 12-bit ADC value.
     */
    uint16_t getNTCADC12bit(void);

    /**
     * @brief Gets the NTC thermistor resistance.
     *
     * Retrieves the measured resistance of the NTC thermistor in ohms.
     *
     * @return The resistance in ohms.
     */
    uint16_t getNTCResistance(void);

    /**
     * @brief Calculates the temperature from NTC resistance.
     *
     * Calculates the temperature in degrees Celsius using the specified NTC resistance.
     *
     * @param ntcResistance The resistance in ohms.
     *
     * @return The calculated temperature in degrees Celsius.
     */
    float getNTCTemperature(uint16_t ntcResistance);

    /**
     * @brief Gets the internal reference voltage.
     *
     * Retrieves the internal reference voltage used by the device.
     *
     * @return The reference voltage in millivolts.
     */
    uint16_t getReferenceVoltage(void);

    /**
     * @brief Gets the voltage from the MQ sensor channel.
     *
     * Reads the current voltage of the MQ sensor channel in millivolts.
     *
     * @return The MQ sensor channel voltage in millivolts.
     */
    uint16_t getMQVoltage(void);

    /**
     * @brief Gets the voltage from the NTC sensor channel.
     *
     * Reads the current voltage of the NTC sensor channel in millivolts.
     *
     * @return The NTC sensor channel voltage in millivolts.
     */
    uint16_t getNTCVoltage(void);

    /**
     * @brief Gets the firmware version.
     *
     * Retrieves the firmware version number of the device.
     *
     * @return The firmware version number.
     */
    uint8_t getFirmwareVersion(void);

    /**
     * @brief Sets the I2C address of the device.
     *
     * Sets a new I2C address for the device. The valid range is 0x08 to 0x77.
     * If the provided address is out of range, it will be clamped to the closest valid value.
     *
     * Note: This operation writes to flash memory and may take more than 20 ms.
     *
     * @param addr The new I2C address.
     *
     * @return The newly set I2C address.
     */
    uint8_t setI2CAddress(uint8_t addr);

    /**
     * @brief Gets the current I2C address.
     *
     * Retrieves the current I2C address of the device.
     *
     * @return The current I2C address.
     */
    uint8_t getI2CAddress(void);

private:
    TwoWire *_wire;
    uint8_t _addr;
    uint8_t _scl;
    uint8_t _sda;
    uint32_t _speed;

    // Mutex flag for indicating whether the mutex is locked.
    bool isMutexLocked = false;  // Mutex semaphore.

    /**
     * @brief Writes multiple bytes to a specified register.
     *
     * This function writes a sequence of bytes from the provided buffer
     * to the device located at the specified I2C address and register.
     *
     * @param addr   The I2C address of the device.
     * @param reg    The register address where the data will be written.
     * @param buffer A pointer to the data buffer that contains the bytes to be
     * written.
     * @param length The number of bytes to write from the buffer.
     */
    void writeBytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length);

    /**
     * @brief Reads multiple bytes from a specified register.
     *
     * This function reads a sequence of bytes from the device located at
     * the specified I2C address and register into the provided buffer.
     *
     * @param addr   The I2C address of the device.
     * @param reg    The register address from which the data will be read.
     * @param buffer A pointer to the data buffer where the read bytes will be
     * stored.
     * @param length The number of bytes to read into the buffer.
     */
    void readBytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length);

    /**
     * @brief Acquires a mutex lock.
     *
     * This function attempts to acquire a mutex lock to ensure thread-safe access
     * to shared resources. It should be paired with a corresponding call to
     * releaseMutex() to prevent deadlocks.
     */
    void acquireMutex();

    /**
     * @brief Releases a mutex lock.
     *
     * This function releases a previously acquired mutex lock, allowing other
     * threads to access shared resources. It should only be called after
     * successfully acquiring the mutex with acquireMutex().
     */
    void releaseMutex();
};

#endif