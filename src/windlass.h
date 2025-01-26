#pragma once

#ifndef __WINDLASS_H__
#define __WINDLASS_H__

#include <Arduino.h>
#undef min
#undef max
#include "menu_item.h"
#include "digital_in.h"


typedef float (*gypsy_circumference_getter)();
typedef float (*total_rode_getter)();

class Windlass {
public:
    Windlass(gypsy_circumference_getter get_gypsy_circumference, total_rode_getter get_total_rode);

    enum WindlassState {
        POWER_OFF,
        STOPPED,
        DEPLOYING,
        RETRIEVING,
        FREEFALLING
    };

    WindlassState get_state();
    float get_chain_speed();
    void calculate_chain_speed();
    float get_rode_deployed();
    bool is_state_updated();
    bool is_rode_updated();

    class Notifier : public IPinInputNotifier {
    public:
        Notifier(Windlass* windlass, bool* state_variable);
        virtual void notify_input(bool state);

    private:
        Windlass* windlass;
        bool* state_variable;
    };

    class GypsyRotationNotifier : public IPinInputNotifier {
    public:
        GypsyRotationNotifier(Windlass* windlass);
        virtual void notify_input(bool state);
    
    private:
        Windlass* windlass;
    };

    IPinInputNotifier* get_power_notifier() { return powerNotifier; }
    IPinInputNotifier* get_deploy_notifier() { return deployNotifier; }
    IPinInputNotifier* get_retrieve_notifier() { return retrieveNotifier; }
    IPinInputNotifier* get_gypsy_notifier() { return gypsyNotifier; }

private:
    gypsy_circumference_getter get_gypsy_circumference;
    total_rode_getter get_total_rode;
    Notifier* powerNotifier;
    Notifier* deployNotifier;
    Notifier* retrieveNotifier;
    GypsyRotationNotifier* gypsyNotifier;

    bool power_on = false;
    bool deploying = false;
    bool retrieving = false;
    uint32_t gypsy_movement_time[2] = { 0, 0 };
    float chain_speed;
    float rode_deployed = 0.0;
    bool state_updated = true;
    bool rode_updated = true;
};

class WindlassMenuItem : public MenuItem {
public:
    WindlassMenuItem(Windlass* windlass);
    
    virtual bool can_focus();
    virtual void draw(LiquidCrystal_I2C* lcd);
    virtual bool is_updated();

private:
    Windlass* windlass;
};

class RodeDeployedMenuItem : public MenuItem {
public:
    RodeDeployedMenuItem(Windlass* windlass);
    
    virtual bool can_focus();
    virtual void draw(LiquidCrystal_I2C* lcd);
    virtual bool is_updated();

private:
    Windlass* windlass;
};

#endif