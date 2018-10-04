/*
	Generic joystick driver (C) 2017-2018 
	Dimitrisv22@gmail.com 
	Git : https://github.com/DimitrisVlachos
*/
#ifndef _generic_joy_driver_hpp_
#define _generic_joy_driver_hpp_
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <SDL/SDL.h>

class generic_joystick_driver_c {
private:
    struct axis_ctx_t {
        axis_ctx_t() : value(0),r_min(0) ,r_max(0), used(false),range_set(false) {}
        int32_t value;
        int32_t r_min,r_max;
        bool used;
        bool range_set;
    };
    struct button_ctx_t {
        bool pressed;
        bool used;
        button_ctx_t() : pressed(false),used(false) {}
    };
    struct hat_ctx_t {
       int32_t value;
       bool used;
       hat_ctx_t() : value(0),used(false) {}
    };
    struct joy_ctx_t {
        std::vector<button_ctx_t> m_buttons;
        std::vector<axis_ctx_t> m_axis_value;
        std::vector<hat_ctx_t> m_hat_value;
    };
    SDL_Joystick* m_joy_controller ;
    SDL_Event m_event;
    std::vector<joy_ctx_t> m_joys;

private:
	void set_button_value(const uint32_t joy,const uint32_t button,const bool pressed);
	void set_axis_value(const uint32_t joy,const uint32_t axis,const int32_t value) ;
	void set_hat_value(const uint32_t joy,const uint32_t hat,const int32_t value);
	
public:
    generic_joystick_driver_c() ;
    ~generic_joystick_driver_c() ;
    void close() ;
    bool open(const size_t which_joy) ;
    bool set_axis_range(const uint32_t joy,const uint32_t axis,const int32_t r_min,const int32_t r_max) ;
    uint32_t get_hat_count(const uint32_t joy) const ;
    uint32_t get_button_count(const uint32_t joy) const;
    uint32_t get_axis_count(const uint32_t joy) const ;
    bool get_button_pressed_state(const uint32_t joy,const uint32_t id) const ;
    int32_t get_axis_value(const uint32_t joy,const uint32_t axis) const;
    int32_t get_hat_value(const uint32_t joy,const uint32_t hat) const ;

    void override_axis_value(const uint32_t joy,const uint32_t axis,const int32_t value);

    void update();
};
#endif
