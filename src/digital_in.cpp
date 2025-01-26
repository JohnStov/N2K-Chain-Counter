#include "digital_in.h"

InputPin::InputPin(uint32_t pin, IPinInputNotifier* notifier) {
    this->pin = pin;
    this->notifier = notifier;
    pinMode(pin, INPUT);
}

void InputPin::scan() {
    int result = digitalRead(pin);
    notifier->notify_input(result == HIGH);
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

