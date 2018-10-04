/*
	Generic joystick driver (C) 2017-2018 
	Dimitrisv22@gmail.com 
	Git : https://github.com/DimitrisVlachos
*/

#include "generic_joy_driver.hpp"
//#include "../trace/trace.hpp"
template <typename T>
static inline void make_room(std::vector<T>& v,const size_t r) {
	if ((v.size() > r))
		return;

	T tmp = T();
	v.push_back( tmp );

    while (v.size() <= r)
		v.push_back( tmp );
}
	
generic_joystick_driver_c::generic_joystick_driver_c()  : m_joy_controller(nullptr){

}
generic_joystick_driver_c::~generic_joystick_driver_c() {

}
	
void generic_joystick_driver_c::set_button_value(const uint32_t joy,const uint32_t button,const bool pressed) {
        make_room(m_joys[joy].m_buttons,button);
        m_joys[joy].m_buttons[button].pressed = pressed;
        m_joys[joy].m_buttons[button].used = true;
}

void generic_joystick_driver_c::set_axis_value(const uint32_t joy,const uint32_t axis,const int32_t value) {
        make_room(m_joys[joy].m_axis_value,axis);
        m_joys[joy].m_axis_value[axis].used = true;
        if (!m_joys[joy].m_axis_value[axis].range_set) {
            m_joys[joy].m_axis_value[axis].value = value;
            return;
        }
        const int32_t r_min = m_joys[joy].m_axis_value[axis].r_min;
        const int32_t r_max = m_joys[joy].m_axis_value[axis].r_max;

        if (value < 0) {
            if (value >= r_min)
                m_joys[joy].m_axis_value[axis].value = 0;
            else
                m_joys[joy].m_axis_value[axis].value = value;
        } else {
            if (value <= r_max)
                m_joys[joy].m_axis_value[axis].value = 0;
            else
                m_joys[joy].m_axis_value[axis].value = value;
        }

}

void generic_joystick_driver_c::set_hat_value(const uint32_t joy,const uint32_t hat,const int32_t value) {
        make_room(m_joys[joy].m_hat_value,hat);
        m_joys[joy].m_hat_value[hat].value = value;
        m_joys[joy].m_hat_value[hat].used = true;
}
	
void generic_joystick_driver_c::close() {
    if (m_joy_controller != nullptr) {
        if (SDL_JoystickGetAttached(m_joy_controller)) {
            SDL_JoystickClose(m_joy_controller);
        }
        m_joy_controller = nullptr;
    }
}

bool generic_joystick_driver_c::open(const size_t which_joy) {
    //trace_log("Starting JOY...\n");
    memset(&m_event,0,sizeof(m_event));

    if( SDL_Init( SDL_INIT_JOYSTICK ) < 0 ) {
       //trace_log( "Could not initialize JOY SUBSYS! Error: %s\n", SDL_GetError() );
        return false ;
    }

    const int32_t n_joy = (int32_t)SDL_NumJoysticks();
    if( n_joy < 1 ) {
        //trace_log( "Warning: No joysticks connected!\n" );
        return false;
    }

    m_joys.clear();
    m_joys.reserve( (size_t)n_joy );
    m_joys.push_back(joy_ctx_t());
    make_room(m_joys[0].m_buttons,12-1);
    make_room(m_joys[0].m_axis_value,4-1);
    make_room(m_joys[0].m_hat_value,1-1);

    m_joy_controller = SDL_JoystickOpen( which_joy );

    if( m_joy_controller == nullptr ) {
        //trace_log( "Warning: Unable to open game controller! Error: %s\n", SDL_GetError() );
        SDL_Quit();
        return false;
    }

    return true;
}

bool generic_joystick_driver_c::set_axis_range(const uint32_t joy,const uint32_t axis,const int32_t r_min,const int32_t r_max) {
    if (m_joys.size() <= joy) return false;
    m_joys[joy].m_axis_value[axis].r_min = r_min;
    m_joys[joy].m_axis_value[axis].r_max = r_max;
    m_joys[joy].m_axis_value[axis].range_set = (r_min == r_max) ? false : true;
    return true;
}

uint32_t generic_joystick_driver_c::get_hat_count(const uint32_t joy) const {
    if (m_joys.size() <= joy) return 0;
    return (uint32_t)m_joys[joy].m_hat_value.size();
}

uint32_t generic_joystick_driver_c::get_button_count(const uint32_t joy) const {
    if (m_joys.size() <= joy) return 0;
    return (uint32_t)m_joys[joy].m_buttons.size();
}

uint32_t generic_joystick_driver_c::get_axis_count(const uint32_t joy) const {
    if (m_joys.size() <= joy) return 0;
    return (uint32_t)m_joys[joy].m_axis_value.size();
}

bool generic_joystick_driver_c::get_button_pressed_state(const uint32_t joy,const uint32_t id) const {
    if (m_joys.size() <= joy)
        return false;
    if (m_joys[joy].m_buttons.size() <= id)
        return false;
    if (!m_joys[joy].m_buttons[id].used)
        return false;
    return m_joys[joy].m_buttons[id].pressed ;
}

int32_t generic_joystick_driver_c::get_axis_value(const uint32_t joy,const uint32_t axis) const  {
    if (m_joys.size() <= joy)
        return false;
    if (m_joys[joy].m_axis_value.size() <= axis)
        return false;
    if (!m_joys[joy].m_axis_value[axis].used)
        return 0;
    return m_joys[joy].m_axis_value[axis].value;
}

int32_t generic_joystick_driver_c::get_hat_value(const uint32_t joy,const uint32_t hat) const  {
    if (m_joys.size() <= joy)
        return false;
    if (m_joys[joy].m_hat_value.size() <= hat)
        return false;
    if (!m_joys[joy].m_hat_value[hat].used)
        return 0;
    return m_joys[joy].m_hat_value[hat].value;
}

void generic_joystick_driver_c::override_axis_value(const uint32_t joy,const uint32_t axis,const int32_t value) {
    if (m_joys.size() <= joy)
        return ;
    make_room(m_joys[joy].m_axis_value,axis);
    set_axis_value(joy,axis,value);
}

void generic_joystick_driver_c::update() {
    while( SDL_PollEvent(&m_event) ) {
        switch (m_event.type) {
            case SDL_JOYBUTTONDOWN: {
            const auto btn = m_event.jbutton.button;
            set_button_value(0,btn,true);
            break;
        }
        case SDL_JOYBUTTONUP: {
            const auto btn = m_event.jbutton.button;
            set_button_value(0,btn,false);
            break;
        }
        case SDL_JOYAXISMOTION: {
            const auto axis = m_event.jaxis.axis;
            const auto val = m_event.jaxis.value;
            set_axis_value(0,axis,val);
            break;
        }
        case SDL_JOYHATMOTION: {
            const auto which = m_event.jhat.hat;
            const auto val = m_event.jhat.value;
            set_hat_value(0,which,val);
            break;
        }
        }
    }
}
