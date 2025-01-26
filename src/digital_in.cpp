#include "digital_in.h"
#include <string>

InputPin::InputPin(uint32_t pin, IPinInputNotifier* notifier) {
    this->pin = pin;
    this->notifier = notifier;

    pinMode(pin, INPUT);
}

void InputPin::scan() {
    if (debounce_time == 0)
    {
        debounce_time = millis();
        debounce_value = digitalRead(pin);
    }
    else if (millis() - debounce_time > 10)
    {
        int value = digitalRead(pin);
        if (value == debounce_value)
            notifier->notify_input(pin, value == HIGH);
        debounce_time = 0;
    }
}

PinCollection::PinCollection(const std::vector<InputPin*>& pins) 
{
    this->pins = pins;
}

void PinCollection::add_pin(InputPin* pin) {
    pins.push_back(pin);
}

void PinCollection::scan() {
    std::for_each(pins.begin(), pins.end(), [](InputPin* &pin){ pin->scan(); });
}

