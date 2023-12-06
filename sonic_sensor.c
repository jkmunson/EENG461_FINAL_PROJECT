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
#include "sonic_sensor.h"
#include "radioV2.h"

uint32_t um_distance;
uint32_t cm_distance;

void capture_edge_time(int pin);

void WTimer5AIntHandler(void){
    TimerIntClear(WTIMER5_BASE, TIMER_CAPA_EVENT); // acknowledge interrupt

    capture_edge_time(6);

    um_distance = micro_sec[4] * 170;
    cm_distance = um_distance / 10000;
}

void sonic_sensor(void)
{
    // INITIALIZATIONS -------------------------
    // Peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);                                            // Enable port E
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);                                             // Enable PWM1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER5);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_WTIMER5))
    {
    }

    // PWM to trigger ultra-sonic sensor ----------------------------------
    GPIOPinConfigure(GPIO_PE5_M1PWM3);
    GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_5);

    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    PWMGenEnable(PWM1_BASE, PWM_GEN_1);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC | PWM_GEN_MODE_DBG_RUN);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_1, PWM_PERIOD);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_3, PULSE_WIDTH);
    PWMOutputState(PWM1_BASE, PWM_OUT_3_BIT, true);

    // Input Capture ----------------------------------------------------------------------------------------

    // Configure PD4
    GPIO_PORTD_AFSEL_R |= GPIO_PIN_6;      // alternate functions PD4
    GPIO_PORTD_DEN_R |= GPIO_PIN_6;        // digital enable PD0-PD3
    GPIO_PORTD_PCTL_R |= WTACCP0_PD6;        // port mux control for WTACCP0 at PD6
    GPIO_PORTD_DR2R_R |= GPIO_PIN_6;       // 2ma drive PD6

    TimerDisable(WTIMER5_BASE, TIMER_A);

    // Wide timer 5A
    WTIMER5_CFG_R |= SplitTimer;           // splits timer 2 for 32-bit
    WTIMER5_TAMR_R |= EdgeTrigCapModeCountUp;        // timer a mode reg: edge triggered, capture mode, count up
    WTIMER5_TAPR_R |= Prescalar;          // wide timer 2A prescaler: add 4 bits to clock
    WTIMER5_CTL_R |= EventModeBothEdges;         // wide timer 2 A+B event mode "both edges"

    // Register Wide Timer 4 interrupts
    TimerIntRegister(WTIMER5_BASE, TIMER_A, WTimer5AIntHandler);
    TimerIntEnable(WTIMER5_BASE, TIMER_CAPA_EVENT);
    TimerIntClear(WTIMER5_BASE, TIMER_CAPA_EVENT);

    // Enable timers
    TimerEnable(WTIMER5_BASE, TIMER_A);
    return(0);
} // end main
