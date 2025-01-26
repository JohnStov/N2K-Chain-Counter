#include <Arduino.h>
#undef min
#undef max
#include "digital_in.h"

template<uint32_t pin>
class InterruptPin {
public:
    InterruptPin(IPinInputNotifier* input_notifier){
        InterruptPin<pin>::notifier = input_notifier;
        InterruptPin<pin>::handle_interrupts = false;
        pinMode(pin, INPUT);
        attachInterrupt(digitalPinToInterrupt(pin), handler, FALLING);
    }

    void start() { InterruptPin<pin>::handle_interrupts = true; }
    void stop() { InterruptPin<pin>::handle_interrupts = false; }

private:
    static void handler() {
        if (InterruptPin<pin>::handle_interrupts)
            InterruptPin<pin>::notifier->notify_input(true);
    }

    static IPinInputNotifier* notifier;
    static bool handle_interrupts;
};

template<uint32_t pin>
IPinInputNotifier* InterruptPin<pin>::notifier;

template<uint32_t pin>
bool InterruptPin<pin>::handle_interrupts;
