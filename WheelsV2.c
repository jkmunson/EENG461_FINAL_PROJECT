#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <driverlib/pin_map.h>
#include <driverlib/timer.h>
#include <driverlib/systick.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>
#include <driverlib/adc.h>
#include <driverlib/pwm.h>
#include <WheelsV2.h>

uint8_t wheelsRun = 0;

void Wheels_Configure(){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)){
    }
//    SysCtlPWMClockSet(SYSCTL_PWMDIV_8); //Set PWM clock to 2MHz

    /*
     * Configure GPIO pins A4, A5 as GPIO output for direction control
     */
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_4 | GPIO_PIN_5, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD);

    /*
     * Configure GPIO pins B4, B6 as PWM for speed control
     */
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_6);
    GPIOPinConfigure(GPIO_PB4_M0PWM2);
    GPIOPinConfigure(GPIO_PB6_M0PWM0);

    /*
     * Configure PWM0 GEN1
     */
    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC | PWM_GEN_MODE_DBG_RUN);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, 200000);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 80);
    PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);

    /*
     * Configure PWM0 GEN0
     */
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC | PWM_GEN_MODE_DBG_RUN);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 200000);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 80);
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    wheelsRun = 1;
}

int WheelsV2(int wheels_spd_right, int wheels_spd_left, int wheels_dir_right, int wheels_dir_left)
{
    if (wheelsRun == 1){

        //setting B4 period, 50% duty cycle
        PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, wheels_spd_right);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, wheels_spd_right / 2);
        PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, true);
        //setting A5 pin for direction
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, wheels_dir_right);

        //setting B6 period, 50% duty cycle
        PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, wheels_spd_left);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, wheels_spd_left / 2);
        PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
        //setting A4 pin for direction
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, wheels_dir_left);

    }
    else{
        Wheels_Configure();
    }
    return(0);
}
