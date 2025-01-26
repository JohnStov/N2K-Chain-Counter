#include <Arduino.h>
#undef min
#undef max
#include "motor_control.h"
#include "pin_usage.h"
#include <string>

MotorControl::MotorControl() {
    pinMode(DEPLOY_RELAY_PIN, OUTPUT);
    digitalWrite(DEPLOY_RELAY_PIN, HIGH);

    pinMode(RETRIEVE_RELAY_PIN, OUTPUT);
    digitalWrite(RETRIEVE_RELAY_PIN, HIGH);
}

void MotorControl::notify_input(uint32_t pin, bool state) {
    MotorControlState old_state = get_state();
    
    if (pin == RETRIEVE_BUTTON_PIN) {
        retrieve = !state;
    }

    if (pin == DEPLOY_BUTTON_PIN) {
        deploy = !state;
    }

    MotorControlState new_state = get_state();
    if (new_state != old_state) {
        std::string state_text = new_state == OFF ? "OFF" : new_state == RETRIEVE ? "RETRIEVE" : "DEPLOY"; 
        char text[40];
        sprintf(text, "time: %lu Pin: %lu state: %s\r\n", millis(), pin, state_text.c_str());
        Serial.write(text);
    }
}

MotorControl::MotorControlState MotorControl::get_state() {
    if (deploy)
        return DEPLOY;
    if (retrieve)
        return RETRIEVE;

    return OFF;
}

void MotorControl::set_output() {
    static MotorControlState old_state = OFF;
    MotorControlState state = get_state();
    if (old_state != state)
    { 
        switch (state)
        {
        case DEPLOY:
            Serial.write("DEPLOY\r\n");
            digitalWrite(DEPLOY_RELAY_PIN, LOW);
            digitalWrite(RETRIEVE_RELAY_PIN, HIGH);
            break;
        
        case RETRIEVE:
            Serial.write("RETRIEVE\r\n");
            digitalWrite(DEPLOY_RELAY_PIN, HIGH);
            digitalWrite(RETRIEVE_RELAY_PIN, LOW);
            break;

        default:
            Serial.write("OFF\r\n");
            digitalWrite(DEPLOY_RELAY_PIN, HIGH);
            digitalWrite(RETRIEVE_RELAY_PIN, HIGH);
            break;
        }
        old_state = state;
    }    
}
    

