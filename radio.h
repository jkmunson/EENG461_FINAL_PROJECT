// .:*~*:._.:*~*:._.:*~*:._.:*~*:. useful info  .:*~*:._.:*~*:._.:*~*:._.:*~*:.
//
//  receiver pins:
//  BAT -->    N/A      +5V     GND
//  CHX -->    SIGNAL   +5V     GND
//
//  channel correlation:
//  right stick left/right  --> CH1 --> turning
//  right stick up/down     --> CH2
//  left stick up/down      --> CH3 --> throttle
//  left stick left/right   --> CH4
//
//  alternate functions:
//      left stick:     right stick:        function:
//       left              n/a               alt0
//       right             n/a               alt1
//       n/a               down              alt2
//       n/a               up                alt3
//
// .:*~*:._.:*~*:._.:*~*:._.:*~*:. Defines  .:*~*:._.:*~*:._.:*~*:._.:*~*:.

#define PortD_pins 0xF
#define WideTimerCCP 0x7777
#define SplitTimer 0x4
#define EdgeTrigCapModeCountUp 0x217
#define Prescalar 0xF
#define EventModeBothEdges 0xC0C
#define alt_func_upper_lim 1830 // toggles alt function when PDX pulse is high for defined micro-sec
#define alt_func_lower_lim 1230 // toggles alt function when PDX pulse is low for defined micro-sec

// .:*~*:._.:*~*:._.:*~*:._.:*~*:. Variables  .:*~*:._.:*~*:._.:*~*:._.:*~*:.

extern uint32_t r_edge[4];                         // rising edge clock time for {PD0, PD1, PD2, PD3}
extern uint32_t f_edge[4];                         // falling edge clock time for {PD0, PD1, PD2, PD3}
extern uint32_t clock_cycles[4];    // clock cycles from rising to falling edge {PD0, PD1, PD2, PD3}
extern uint32_t micro_sec[4];       // time in microseconds from rising to falling edge {PD0, PD1, PD2, PD3}
extern bool calc_cycles[4]; // determines when to calculate pulse width {PD0, PD1, PD2, PD3}

extern uint8_t throttle; // range from 0-20, used for wheel control
extern uint8_t turning;  // range from 0-20, used for wheel control

extern bool alternate_functions[4]; // indicates whether alternate function is enabled {alt1, alt2, , PD3}
extern uint8_t alt_func_count[4];
extern uint8_t alt_func_delay;

extern uint32_t t1;
extern uint32_t t2;
extern uint32_t t;
