#pragma once
#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__

#include <Arduino.h>
#undef min
#undef max
#include "digital_in.h"

class MotorControl: public IPinInputNotifier {
public:
    MotorControl();

    enum MotorControlState {
        OFF,
        DEPLOY,
        RETRIEVE
    };

    virtual void notify_input(uint32_t pin, bool state);

    MotorControlState get_state();

    void set_output();

private:
    bool deploy = false;
    bool retrieve = false;
    MotorControlState motor_control_state = MotorControlState::OFF;
};

#endif