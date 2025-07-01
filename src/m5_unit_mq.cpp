/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#include "m5_unit_mq.hpp"
#include "math.h"

static const char *TAG = "M5UnitMQ";

void M5UnitMQ::writeBytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length)
{
    _wire->beginTransmission(addr);
    _wire->write(reg);
    for (int i = 0; i < length; i++) {
        _wire->write(*(buffer + i));
    }
    _wire->endTransmission();
#if defined UNIT_MQ_DEBUG
    Serial.print(TAG);
    Serial.print("Write bytes: [");
    Serial.print(addr);
    Serial.print(", ");
    Serial.print(reg);
    Serial.print(", ");
    for (int i = 0; i < length; i++) {
        Serial.print(buffer[i]);
        if (i < length - 1) {
            Serial.print(", ");
        }
    }
    Serial.println("]");
#else
#endif
}

void M5UnitMQ::readBytes(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length)
{
    uint8_t index = 0;
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(addr, length);
    for (int i = 0; i < length; i++) {
        buffer[index++] = _wire->read();
    }
#if defined UNIT_MQ_DEBUG
    Serial.print(TAG);
    Serial.print("Read bytes: [");
    Serial.print(addr);
    Serial.print(", ");
    Serial.print(reg);
    Serial.print(", ");
    for (int i = 0; i < length; i++) {
        Serial.print(buffer[i]);
        if (i < length - 1) {
            Serial.print(", ");
        }
    }
    Serial.println("]");
#else
#endif
}

void M5UnitMQ::acquireMutex()
{
    while (isMutexLocked) {
        delay(1);  // Busy wait, can be optimized
    }
    isMutexLocked = true;
}

void M5UnitMQ::releaseMutex()
{
    isMutexLocked = false;
}

bool M5UnitMQ::begin(TwoWire *wire, uint8_t addr, uint8_t sda, uint8_t scl, uint32_t speed)
{
    _wire  = wire;
    _addr  = addr;
    _sda   = sda;
    _scl   = scl;
    _speed = speed;
    _wire->begin(_sda, _scl);
    _wire->setClock(_speed);
    delay(10);
    _wire->beginTransmission(_addr);
    uint8_t error = _wire->endTransmission();
    if (error == 0) {
        return true;
    } else {
        return false;
    }
}

void M5UnitMQ::setHeatMode(heat_mode_t mode)
{
    acquireMutex();
    uint8_t reg = UNIT_MQ_MODE_CONTROL_REG;
    writeBytes(_addr, reg, (uint8_t *)&mode, 1);
    releaseMutex();
}

heat_mode_t M5UnitMQ::getHeatMode(void)
{
    acquireMutex();
    uint8_t tempValue;
    uint8_t reg = UNIT_MQ_MODE_CONTROL_REG;
    readBytes(_addr, reg, (uint8_t *)&tempValue, 1);
    releaseMutex();
    return (heat_mode_t)tempValue;
}

void M5UnitMQ::setLEDState(led_status_t status)
{
    acquireMutex();
    uint8_t reg = UNIT_MQ_LED_CONTROL_REG;
    writeBytes(_addr, reg, (uint8_t *)&status, 1);
    releaseMutex();
}

led_status_t M5UnitMQ::getLEDState(void)
{
    acquireMutex();
    uint8_t tempValue;
    uint8_t reg = UNIT_MQ_LED_CONTROL_REG;
    readBytes(_addr, reg, (uint8_t *)&tempValue, 1);
    releaseMutex();
    return (led_status_t)tempValue;
}

void M5UnitMQ::setPulseTime(uint8_t highLevelTime, uint8_t lowLevelTime)
{
    acquireMutex();
    uint8_t tempValue[2] = {0};
    tempValue[0]         = highLevelTime;
    tempValue[1]         = lowLevelTime;
    uint8_t reg          = UNIT_MQ_HIGH_PULSE_CONTROL_REG;
    writeBytes(_addr, reg, (uint8_t *)&tempValue, 2);
    releaseMutex();
}

void M5UnitMQ::getPulseLevelTime(uint8_t *highLevelTime, uint8_t *lowLevelTime)
{
    acquireMutex();
    uint8_t tempValue[2] = {0};
    uint8_t reg          = UNIT_MQ_HIGH_PULSE_CONTROL_REG;
    readBytes(_addr, reg, (uint8_t *)&tempValue, 2);
    *highLevelTime = tempValue[0];
    *lowLevelTime  = tempValue[1];
    releaseMutex();
}

uint8_t M5UnitMQ::getMQADC8bit(void)
{
    acquireMutex();
    uint8_t tempValue = 0;
    uint8_t reg       = UNIT_MQ_MQ_8B_ADC_REG;
    readBytes(_addr, reg, (uint8_t *)&tempValue, 1);
    releaseMutex();
    return tempValue;
}

uint16_t M5UnitMQ::getMQADC12bit(void)
{
    acquireMutex();
    uint16_t tempValue = 0;
    uint8_t reg        = UNIT_MQ_MQ_12B_ADC_REG;
    readBytes(_addr, reg, (uint8_t *)&tempValue, 2);
    releaseMutex();
    return tempValue;
}

mq_adc_valid_tags_t M5UnitMQ::getValidTags(void)
{
    acquireMutex();
    uint8_t tempValue = 0;
    uint8_t reg       = UNIT_MQ_TAGS_REG;
    readBytes(_addr, reg, (uint8_t *)&tempValue, 1);
    releaseMutex();
    return (mq_adc_valid_tags_t)tempValue;
}

uint8_t M5UnitMQ::getNTCADC8bit(void)
{
    acquireMutex();
    uint8_t tempValue = 0;
    uint8_t reg       = UNIT_MQ_NTC_8B_ADC_REG;
    readBytes(_addr, reg, (uint8_t *)&tempValue, 1);
    releaseMutex();
    return tempValue;
}

uint16_t M5UnitMQ::getNTCADC12bit(void)
{
    acquireMutex();
    uint16_t tempValue = 0;
    uint8_t reg        = UNIT_MQ_NTC_12B_ADC_REG;
    readBytes(_addr, reg, (uint8_t *)&tempValue, 2);
    releaseMutex();
    return tempValue;
}

uint16_t M5UnitMQ::getNTCResistance(void)
{
    acquireMutex();
    uint16_t tempValue = 0;
    uint8_t reg        = UNIT_MQ_NTC_RESISTANCE_REG;
    readBytes(_addr, reg, (uint8_t *)&tempValue, 2);
    releaseMutex();
    return tempValue;
}

float M5UnitMQ::getNTCTemperature(uint16_t ntcResistance)
{
    float Rt        = (float)ntcResistance;
    float tempValue = Rt / RESISTANCE_REFERENCE;
    tempValue       = log(tempValue);
    tempValue /= THERMISTOR_B_CONSTANT;
    tempValue += (1 / ABSOLUTE_TEMP_AT_25C);
    tempValue = 1 / tempValue;
    tempValue -= ABSOLUTE_ZERO_TEMP;
    return tempValue;
}

uint16_t M5UnitMQ::getReferenceVoltage(void)
{
    acquireMutex();
    uint16_t tempValue = 0;
    uint8_t reg        = UNIT_MQ_INTERNAL_VOLTAGE_REG;
    readBytes(_addr, reg, (uint8_t *)&tempValue, 2);
    releaseMutex();
    return tempValue;
}

uint16_t M5UnitMQ::getMQVoltage(void)
{
    acquireMutex();
    uint16_t tempValue = 0;
    uint8_t reg        = UNIT_MQ_MQ_VOLTAGE_REG;
    readBytes(_addr, reg, (uint8_t *)&tempValue, 2);
    releaseMutex();
    return tempValue;
}

uint16_t M5UnitMQ::getNTCVoltage(void)
{
    acquireMutex();
    uint16_t tempValue = 0;
    uint8_t reg        = UNIT_MQ_NTC_VOLTAGE_REG;
    readBytes(_addr, reg, (uint8_t *)&tempValue, 2);
    releaseMutex();
    return tempValue;
}

uint8_t M5UnitMQ::getFirmwareVersion(void)
{
    acquireMutex();
    uint8_t tempValue = 0;
    uint8_t reg       = UNIT_MQ_FIRMWARE_VERSION_REG;
    readBytes(_addr, reg, (uint8_t *)&tempValue, 1);
    releaseMutex();
    return tempValue;
}

uint8_t M5UnitMQ::setI2CAddress(uint8_t addr)
{
    acquireMutex();
    addr        = constrain(addr, UNIT_MQ_I2C_ADDR_MIN, UNIT_MQ_I2C_ADDR_MAX);
    uint8_t reg = UNIT_MQ_I2C_ADDRESS_REG;
    writeBytes(_addr, reg, (uint8_t *)&addr, 1);
    _addr = addr;
    releaseMutex();
    return _addr;
}

uint8_t M5UnitMQ::getI2CAddress(void)
{
    acquireMutex();
    uint8_t tempValue;
    uint8_t reg = UNIT_MQ_I2C_ADDRESS_REG;
    readBytes(_addr, reg, (uint8_t *)&tempValue, 1);
    releaseMutex();
    return tempValue;
}