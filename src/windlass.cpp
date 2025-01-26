#include "windlass.h"
#include "pin_usage.h"

Windlass::Windlass(gypsy_circumference_getter get_gypsy_circumference, total_rode_getter get_total_rode)
{
    this->get_gypsy_circumference = get_gypsy_circumference;
    this->get_total_rode = get_total_rode;
}

Windlass::WindlassState Windlass::get_state() {
    if (get_chain_speed() > 0 && !deploying && !retrieving)
        return FREEFALLING;
    if (!power_on)
        return POWER_OFF;
    if (deploying)
        return DEPLOYING;
    if (retrieving)
        return RETRIEVING;

    return STOPPED;
}

float Windlass::get_chain_speed() { return chain_speed; }

void Windlass::calculate_chain_speed() {
    uint32_t now = millis();
    float old_chain_speed = chain_speed;

    if (gypsy_movement_time[0] == 0 || gypsy_movement_time[1] == 0) {
        chain_speed = 0;
    } else if (now - gypsy_movement_time[0] > 1000) {
        gypsy_movement_time[0] = 0;
        gypsy_movement_time[1] = 0;
        chain_speed = 0;
    } else {
        uint32_t elapsed_millis = gypsy_movement_time[0] - gypsy_movement_time[1];
        chain_speed = (float)get_gypsy_circumference() / (float)elapsed_millis * 1000.0;
    }

    if (old_chain_speed != chain_speed)
        state_updated = true;
}

float Windlass::get_rode_deployed() {
    return rode_deployed;
}

bool Windlass::is_state_updated() {
    bool result = state_updated; 
    state_updated = false; 
    return result;
} 

bool Windlass::is_rode_updated() {
    bool result = rode_updated; 
    rode_updated = false; 
    return result;
} 

void Windlass::notify_input(uint32_t pin, bool state) {
    if (pin == POWER_SENSE_PIN || pin == RETRIEVE_SENSE_PIN || pin == GYPSY_SENSE_PIN)
    {
        WindlassState old_state = get_state();

        if (pin == POWER_SENSE_PIN)
            power_on = state;
        else if (pin == DEPLOY_SENSE_PIN)
            deploying = state;
        else if (pin == RETRIEVE_SENSE_PIN)
            retrieving = state;

        if (old_state != get_state())
            state_updated = true;
    } else if (pin == GYPSY_SENSE_PIN) {
        float previous_rode_deployed = get_rode_deployed();
        
        gypsy_movement_time[1] = gypsy_movement_time[0];
        gypsy_movement_time[0] = millis(); 

        switch (get_state())
        {
            case RETRIEVING:
                if (rode_deployed > 0.0) {
                    rode_deployed -= get_gypsy_circumference();
                }
                break;
            default:
                if (rode_deployed < get_total_rode()) {
                    rode_deployed += get_gypsy_circumference();
                }
                break;
        }

        if (get_rode_deployed() != previous_rode_deployed)
            rode_updated = true;

    }
}

WindlassMenuItem::WindlassMenuItem(Windlass* windlass) {
    this->windlass = windlass;
}
    
bool WindlassMenuItem::can_focus() { return false; }

void WindlassMenuItem::draw(LiquidCrystal_I2C* lcd) {
    char text[19];
    switch (windlass->get_state())
    {
        case Windlass::FREEFALLING:
            sprintf(text, "Freefall %4.1fm/s", windlass->get_chain_speed());
            break;
        case Windlass::DEPLOYING:
            sprintf(text, "Deploy   %4.1fm/s", windlass->get_chain_speed());
            break;
        case Windlass::RETRIEVING:
            sprintf(text, "Retrieve %4.1fm/s", windlass->get_chain_speed());
            break;
        case Windlass::STOPPED:
            strcpy(text, "Stopped           ");
            break;
        case Windlass::POWER_OFF:
        default:
            strcpy(text, "Power Off         ");
            break;
    }
    lcd->print(text);
}

bool WindlassMenuItem::is_updated() { return windlass->is_state_updated(); }

RodeDeployedMenuItem::RodeDeployedMenuItem(Windlass* windlass) {
    this->windlass = windlass;
}
    
bool RodeDeployedMenuItem::can_focus() { return false; }

void RodeDeployedMenuItem::draw(LiquidCrystal_I2C* lcd) {
    char text[19];
    sprintf(text, "Rode Out:% 7.2fm", windlass->get_rode_deployed());
    lcd->print(text);
}

bool RodeDeployedMenuItem::is_updated() { 
    return windlass->is_rode_updated(); 
}