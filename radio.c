#include<stdint.h>
#include<stdbool.h>
#include<math.h>
#include<inc/hw_memmap.h>
#include<inc/hw_gpio.h>
#include<inc/tm4c123gh6pm.h>
#include<inc/hw_timer.h>
#include<driverlib/gpio.h>
#include<driverlib/sysctl.h>
#include<driverlib/timer.h>
#include<driverlib/adc.h>
#include<driverlib/pwm.h>
#include"driverlib/pin_map.h"

#include "radio.h"

uint32_t r_edge[4];                         // rising edge clock time for {PD0, PD1, PD2, PD3}
uint32_t f_edge[4];                         // falling edge clock time for {PD0, PD1, PD2, PD3}
uint32_t clock_cycles[4] = {0, 0, 0, 0};    // clock cycles from rising to falling edge {PD0, PD1, PD2, PD3}
uint32_t micro_sec[4] = {0, 0, 0, 0};       // time in microseconds from rising to falling edge {PD0, PD1, PD2, PD3}
bool calc_cycles[4] = {false, false, false, false}; // determines when to calculate pulse width {PD0, PD1, PD2, PD3}

uint8_t throttle = 0; // range from 0-20, used for wheel control
uint8_t turning = 0;  // range from 0-20, used for wheel control

bool alternate_functions[4] = {false, false, false, false}; // indicates whether alternate function is enabled {alt1, alt2, , PD3}
uint8_t alt_func_count[4] = {0, 0, 0, 0};
uint8_t alt_func_delay = 2;

uint32_t t1 = 0;
uint32_t t2 = 0;
uint32_t t = 69;

void capture_edge_time(int pin);
void assign_val(int pin);
void handle_alt_func(void);

// Interrupts ----------------------------------------------------------------------------------------
void Timer0AIntHandler(void){
    TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

//    printf("PD0 %d    PD1 %d    PD2 %d    PD3 %d    turning %d    throttle %d    t %d    alt0 %d    alt1 %d    alt2 %d    alt3 %d\n",
//           micro_sec[0], micro_sec[1], micro_sec[2], micro_sec[3], turning, throttle, t,
//           alternate_functions[0], alternate_functions[1], alternate_functions[2], alternate_functions[3]);

    handle_alt_func();

    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

void Timer1AIntHandler(void){
    TimerIntDisable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    t = t1 - t2; // Record edge triggered ISR time (in 80Mhz clock cycles)

    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
}

void WTimer2AIntHandler(void){
    TimerIntClear(WTIMER2_BASE, TIMER_CAPA_EVENT); // acknowledge interrupt

    t1 = TimerValueGet(TIMER1_BASE, TIMER_A);
    capture_edge_time(0);
    assign_val(0);
    t2 = TimerValueGet(TIMER1_BASE, TIMER_A);
}

void WTimer2BIntHandler(void){
    TimerIntClear(WTIMER2_BASE, TIMER_CAPB_EVENT); // acknowledge interrupt

    capture_edge_time(1);
}

void WTimer3AIntHandler(void){
    TimerIntClear(WTIMER3_BASE, TIMER_CAPA_EVENT); // acknowledge interrupt

    capture_edge_time(2);
    assign_val(2);
}

void WTimer3BIntHandler(void){
    TimerIntClear(WTIMER3_BASE, TIMER_CAPB_EVENT); // acknowledge interrupt

    capture_edge_time(3);
}

// Functions ----------------------------------------------------------------------------------------

void capture_edge_time(int pin){
    uint8_t x;
    uint32_t timer_base;
    uint32_t timer;

    if (pin==0){
        x = GPIO_PIN_0;
        timer_base = WTIMER2_BASE;
        timer = TIMER_A;
    } else if (pin==1){
        x = GPIO_PIN_1;
        timer_base = WTIMER2_BASE;
        timer = TIMER_B;
    } else if (pin==2){
        x = GPIO_PIN_2;
        timer_base = WTIMER3_BASE;
        timer = TIMER_A;
    } else {
        x = GPIO_PIN_3;
        timer_base = WTIMER3_BASE;
        timer = TIMER_B;
    }

    bool PDX_val = GPIOPinRead(GPIO_PORTD_BASE, x);
    if (PDX_val){ // assumes pin still high from rising edge
        r_edge[pin] = TimerValueGet(timer_base, timer);
        calc_cycles[pin] = false; // wait until falling edge time to update before calculating high time
    } else {
        f_edge[pin] = TimerValueGet(timer_base, timer);
        calc_cycles[pin] = true; // Falling edge detected, calculate pulse width duration
    }

    if (calc_cycles[pin]){
        if (f_edge[pin] > r_edge[pin]){ // Disregard if clock wraps around
            clock_cycles[pin] = f_edge[pin] - r_edge[pin];
            micro_sec[pin] = clock_cycles[pin] * .0125;
        }
    }
}

void assign_val(int pin){
//    uint32_t inc = 40;
    uint32_t base = 1100;
    uint8_t i;
    for (i=0; i<21;i++){
        if (micro_sec[pin] < (base+40*i)){
            if (pin == 0){
                turning = i;
                break;
            } else {
                throttle = i;
                break;
            }
        }
    }
}

void handle_alt_func(void){
    // check left stick left/right
    if (micro_sec[3] < alt_func_lower_lim){
        alt_func_count[0]++;
    } else if (micro_sec[3] > alt_func_upper_lim){
        alt_func_count[1]++;
    }

    // check right stick up/down
    if (micro_sec[1] < alt_func_lower_lim){
        alt_func_count[2]++;
    } else if (micro_sec[1] > alt_func_upper_lim){
        alt_func_count[3]++;
    }

    uint8_t i;
    for (i=0; i<4; i++){
        if (alt_func_count[i] == alt_func_delay){
            if (alternate_functions[i]){
                alternate_functions[i] = false;
                alt_func_count[i] = 0;
                break;
            } else {
                alternate_functions[i] = true;
                alt_func_count[i] = 0;
                break;
            }
        }
    }
}

int radio(void)
{
// Initializations ----------------------------------------------------------------------------------------

    // Enable Peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER3);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_WTIMER3))
    {
    }

    // Set system clock to 80Mhz frequency
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Configure Timer0A for periodic 1 second interrupts
    TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0AIntHandler);
    TimerLoadSet(TIMER0_BASE, TIMER_A, 80000000);
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);

    // Timer1A (extra) for calculating CCP ISR time
    TimerConfigure(TIMER1_BASE,TIMER_CFG_PERIODIC);
    TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1AIntHandler);
    TimerLoadSet(TIMER1_BASE, TIMER_A, 8000000);
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER1_BASE, TIMER_A);

// Input Capture ----------------------------------------------------------------------------------------

    // Configure PD0-PD3
    GPIO_PORTD_AFSEL_R |= PortD_pins;      // alternate functions PD0-PD3
    GPIO_PORTD_DEN_R |= PortD_pins;        // digital enable PD0-PD3
    GPIO_PORTD_PCTL_R |= WideTimerCCP;     // port mux control for WT(A/B)CCP(0/1) at PD0-PD3
    GPIO_PORTD_DR2R_R |= PortD_pins;       // 2ma drive PD0-PD3

    // Configure WT2CCP0, WT2CCP1, WT3CCP0, WT3CCP1 --> lines 199 to 217
    TimerDisable(WTIMER2_BASE, TIMER_A | TIMER_B);
    TimerDisable(WTIMER3_BASE, TIMER_A | TIMER_B);

    // Wide timer 2A and 2B
    WTIMER2_CFG_R |= SplitTimer;           // splits timer 2 for 32-bit
    WTIMER2_TAMR_R |= EdgeTrigCapModeCountUp;        // timer a mode reg: edge triggered, capture mode, count up
    WTIMER2_TBMR_R |= EdgeTrigCapModeCountUp;        // timer b mode reg: edge triggered, capture mode, count up
    WTIMER2_TAPR_R |= Prescalar;          // wide timer 2A prescaler: add 4 bits to clock
    WTIMER2_TBPR_R |= Prescalar;          // wide timer 2B prescaler: add 4 bits to clock
    WTIMER2_CTL_R |= EventModeBothEdges;         // wide timer 2 A+B event mode "both edges"

    // Wide timer 3A and 3B
    WTIMER3_CFG_R |= SplitTimer;           // splits timer 2 for 32-bit
    WTIMER3_TAMR_R |= EdgeTrigCapModeCountUp;        // timer a mode reg: edge triggered, capture mode, count up
    WTIMER3_TBMR_R |= EdgeTrigCapModeCountUp;        // timer b mode reg: edge triggered, capture mode, count up
    WTIMER3_TAPR_R |= Prescalar;          // wide timer 3A prescaler: add 4 bits to clock
    WTIMER3_TBPR_R |= Prescalar;          // wide timer 3B prescaler: add 4 bits to clock
    WTIMER3_CTL_R |= EventModeBothEdges;         // wide timer 3 A+B event mode "both edges"

    // Register Wide Timer 2 interrupts
    TimerIntRegister(WTIMER2_BASE, TIMER_A, WTimer2AIntHandler);
    TimerIntEnable(WTIMER2_BASE, TIMER_CAPA_EVENT);
    TimerIntClear(WTIMER2_BASE, TIMER_CAPA_EVENT);
    TimerIntRegister(WTIMER2_BASE, TIMER_B, WTimer2BIntHandler);
    TimerIntEnable(WTIMER2_BASE, TIMER_CAPB_EVENT);
    TimerIntClear(WTIMER2_BASE, TIMER_CAPB_EVENT);
    // Register Wide Timer 3 interrupts
    TimerIntRegister(WTIMER3_BASE, TIMER_A, WTimer3AIntHandler);
    TimerIntEnable(WTIMER3_BASE, TIMER_CAPA_EVENT);
    TimerIntClear(WTIMER3_BASE, TIMER_CAPA_EVENT);
    TimerIntRegister(WTIMER3_BASE, TIMER_B, WTimer3BIntHandler);
    TimerIntEnable(WTIMER3_BASE, TIMER_CAPB_EVENT);
    TimerIntClear(WTIMER3_BASE, TIMER_CAPB_EVENT);

    // Enable timers
    TimerEnable(WTIMER2_BASE, TIMER_A | TIMER_B);
    TimerEnable(WTIMER3_BASE, TIMER_A|TIMER_B);

    // ----- IDLE LOOP -----
//    while(1)
//    {
//
//    }
    return(0);
} // end main


