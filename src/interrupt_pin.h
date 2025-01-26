#include <Arduino.h>
#undef min
#undef max
#include "digital_in.h"

template<uint32_t pin>
class InterruptPin {
public:
    InterruptPin(IPinInputNotifier* input_notifier){
        InterruptPin<pin>::notifier = input_notifier;
        pinMode(pin, INPUT);
        attachInterrupt(digitalPinToInterrupt(pin), handler, FALLING);
    }

private:
    static void handler() {
        InterruptPin<pin>::notifier->notify_input(true);
    }

    static IPinInputNotifier* notifier;
};

template<uint32_t pin>
IPinInputNotifier* InterruptPin<pin>::notifier;

