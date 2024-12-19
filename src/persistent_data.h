#pragma once

#ifndef __PESRSISTENT_DATA_H__
#define __PESRSISTENT_DATA_H__

#include <Adafruit_FRAM_I2C.h>


class PersistentData {
public:
    void begin(uint8_t addr = 0x50, TwoWire *theWire = &Wire) {
        i2cfram.begin(addr, theWire);
    }

    const uint16_t RODE_LENGTH_PER_REVOLUTION_ADDR = 0x00;
    const uint16_t MOTOR_HOURS_ADDR = 0x04;
    const uint16_t RODE_LENGTH_DEPLOYED_ADDR = 0x08;
    const uint16_t TOTAL_SECONDS_ELAPSED_ADDR = 0x0C;

    bool set_rode_length_per_revolution(float value) { return write(RODE_LENGTH_PER_REVOLUTION_ADDR, value); }
    float get_rode_length_per_revolution() { return read<float>(RODE_LENGTH_PER_REVOLUTION_ADDR); }

    bool set_motor_hours(float value) { return write(MOTOR_HOURS_ADDR, value); }
    float get_motor_hours() { return read<float>(MOTOR_HOURS_ADDR); }

    bool set_rode_length_deployed(float value) { return write(RODE_LENGTH_DEPLOYED_ADDR, value); }
    float get_rode_length_deployed() { return read<float>(RODE_LENGTH_DEPLOYED_ADDR); }

    bool set_total_seconds_elapsed(float value) { return write(TOTAL_SECONDS_ELAPSED_ADDR, value); }
    uint32_t get_total_seconds_elapsed() { return read<float>(TOTAL_SECONDS_ELAPSED_ADDR); }


private:
    Adafruit_FRAM_I2C i2cfram;

    template<class T>
    bool write(uint16_t addr, T value) {
        return i2cfram.writeObject<T>(addr, value);
    }

    template<class T>
    T read(uint16_t addr) {
        T value; 
        i2cfram.readObject<T>(addr, value); 
        return value;
    }
};

#endif