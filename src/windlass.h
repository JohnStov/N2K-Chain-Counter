#pragma once

#ifndef __WINDLASS_H__
#define __WINDLASS_H__

#include <Arduino.h>
#include "menu_item.h"


typedef uint32_t (*gypsy_circumference_getter)();
typedef uint32_t (*total_rode_getter)();

class Windlass {
public:
    Windlass(gypsy_circumference_getter get_gypsy_circumference, total_rode_getter get_total_rode)
    {
        this->get_gypsy_circumference = get_gypsy_circumference;
        this->get_total_rode = get_total_rode;
    }

    enum WindlassState {
        POWER_OFF,
        STOPPED,
        DEPLOYING,
        RETRIEVING,
        FREEFALLING
    };

    WindlassState get_state() {
        if (!power_on)
            return POWER_OFF;
        if (deploying)
            return DEPLOYING;
        if (retrieving)
            return RETRIEVING;
        if (get_chain_speed() > 0)
            return FREEFALLING;

        return POWER_OFF;
    }

    float get_chain_speed() { return chain_speed; }

    void register_gypsy_revolution() 
    {
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
            case DEPLOYING:
            default:
                if (rode_deployed < get_total_rode()) {
                    rode_deployed += get_gypsy_circumference();
                }
                break;
        }

        if (get_rode_deployed() != previous_rode_deployed)
            updated = true;
    }
    
    void calculate_chain_speed() {
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
            updated = true;
    }

    void set_power(bool power) { 
        WindlassState old_state = get_state();
        this->power_on = power; 
        if (old_state != get_state())
            updated = true; 
    }
    void set_deploying(bool deploying) 
    { 
        WindlassState old_state = get_state();
        this->deploying = deploying; 
        if (old_state != get_state())
            updated = true; 
    }

    void set_retrieving(bool retrieving) {
        WindlassState old_state = get_state();
        this->retrieving = retrieving;  
        updated = true; 
        if (old_state != get_state())
            updated = true; 
    }

    float get_rode_deployed() {
        return rode_deployed;
    }
    
    bool is_updated() {
        bool result = updated; 
        updated = false; 
        return result;
    } 

private:
    gypsy_circumference_getter get_gypsy_circumference;
    total_rode_getter get_total_rode;
    bool power_on = false;
    bool deploying = false;
    bool retrieving = false;
    uint32_t gypsy_movement_time[2] = { 0, 0 };
    float chain_speed;
    float rode_deployed = 0.0;
    bool updated = true;
};

class WindlassMenuItem : public MenuItem {
public:
    WindlassMenuItem(Windlass* windlass) {
        this->windlass = windlass;
    }
    
    virtual bool can_focus() { return false; }

    virtual void draw(LiquidCrystal_I2C* lcd) {
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
                strcpy(text, "Stopped");
                break;
            case Windlass::POWER_OFF:
            default:
                strcpy(text, "Power Off");
                break;
        }
        lcd->print(text);
    }

    virtual bool is_updated() { return windlass->is_updated(); }

private:
    Windlass* windlass;
};

class RodeDeployedMenuItem : public MenuItem {
public:
    RodeDeployedMenuItem(Windlass* windlass) {
        this->windlass = windlass;
    }
    
    virtual bool can_focus() { return false; }

    virtual void draw(LiquidCrystal_I2C* lcd) {
        char text[18];
        sprintf(text, "Rode Out:% 6.1fm", windlass->get_rode_deployed());
        lcd->print(text);
    }

    virtual bool is_updated() { return windlass->is_updated(); }

private:
    Windlass* windlass;
};

#endif