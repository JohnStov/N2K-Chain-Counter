#include "windlass.h"

Windlass::Windlass(gypsy_circumference_getter get_gypsy_circumference, total_rode_getter get_total_rode)
{
    this->get_gypsy_circumference = get_gypsy_circumference;
    this->get_total_rode = get_total_rode;
    powerNotifier = new Notifier(this, &(this->power_on));
    deployNotifier = new Notifier(this, &(this->deploying));
    retrieveNotifier = new Notifier(this, &(this->retrieving));
    gypsyNotifier = new GypsyRotationNotifier(this);
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
        float cm_per_ms = (float)get_gypsy_circumference() / (float)elapsed_millis;
        chain_speed = cm_per_ms * 10.0; // divide by 100 for m_per_ms then multiply by 1000 for m_per_s
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

Windlass::Notifier::Notifier(Windlass* windlass, bool* state_variable) {
    this->windlass = windlass;
    this->state_variable = state_variable;
}

void Windlass::Notifier::notify_input(bool state) {
    WindlassState old_state = windlass->get_state();
    *state_variable = state; 
    if (old_state != windlass->get_state())
        windlass->state_updated = true; 
}
Windlass::GypsyRotationNotifier::GypsyRotationNotifier(Windlass* windlass) {
    this->windlass = windlass;
}

void Windlass::GypsyRotationNotifier::notify_input(bool state) {
    float previous_rode_deployed = windlass->get_rode_deployed();
    
    windlass->gypsy_movement_time[1] = windlass->gypsy_movement_time[0];
    windlass->gypsy_movement_time[0] = millis(); 

    switch (windlass->get_state())
    {
        case RETRIEVING:
            if (windlass->rode_deployed > 0.0) {
                windlass->rode_deployed -= windlass->get_gypsy_circumference();
            }
            break;
        default:
            if (windlass->rode_deployed < windlass->get_total_rode()) {
                windlass->rode_deployed += windlass->get_gypsy_circumference();
            }
            break;
    }

    if (windlass->get_rode_deployed() != previous_rode_deployed)
        windlass->rode_updated = true;
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
            sprintf(text, "Freefall %4.1f m/s", windlass->get_chain_speed());
            break;
        case Windlass::DEPLOYING:
            sprintf(text, "Deploy   %4.1f m/s", windlass->get_chain_speed());
            break;
        case Windlass::RETRIEVING:
            sprintf(text, "Retrieve %4.1f m/s", windlass->get_chain_speed());
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
    char text[18];
    sprintf(text, "Rode Out:% 6.1fm", windlass->get_rode_deployed());
    lcd->print(text);
}

bool RodeDeployedMenuItem::is_updated() { 
    return windlass->is_rode_updated(); 
}