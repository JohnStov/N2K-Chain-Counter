#pragma once

#ifndef __DIGITAL_IN_H__
#define __DIGITAL_IN_H__

#include <Arduino.h>
#undef min
#undef max
#include <vector>
#include <algorithm>

class IPinInputNotifier {
public:
    virtual void notify_input(uint32_t pin, bool state) = 0;
};

class InputPin {
public:
    InputPin(uint32_t pin, IPinInputNotifier* notifier);

    void scan();

private:
    uint32_t pin;
    IPinInputNotifier* notifier;
    uint32_t debounce_time = 0;
    int debounce_value;
};

class PinCollection {
public:
    PinCollection(const std::vector<InputPin*>& pins);

    void add_pin(InputPin* pin);

    void scan();

private:
    std::vector<InputPin*> pins;
};

#endif