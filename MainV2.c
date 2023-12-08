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
#include <driverlib/i2c.h>
#include <DrivingV2.h>
#include <WheelsV2.h>
#include "radioV2.h"
#include <UART.h>

//This program drives a car via radio signals from a controller and a LIDAR system.
//The signals are used to control 17HE12-1204S stepper motors via DM332T motor controller.
//The DM332T motor controller manipulates the stepper motors via a PWM pulse minimum 7.5 us.
//The pulse causes the stepper motor to perform one step and the rate of the steps is determined by the period.
//The period is set in WheelsV2.
//DrivingV2.c takes the values provided by the signals coming from the controller and the LIDAR, modifies them
//to be a usable period value and passes those values to WheelsV2.
//The code uses the Tiva library.
//
//
//Included files:
//          MainV2.c
//          WheelsV2.c, WheelsV2.h
//          radioV2.c, radioV2.h
//          DrivingV2.c, DrivingV2.h
//          UART.c, UART.h

void radioV2(void);

int main(void)
{
    //Call WheelsV2, radioV2,and UART to configure
    WheelsV2(0,0,0,0);
    radioV2();
    UART();

    while(1){
        //Call DrivingV2 to update speed and rotation direction for motors
        DrivingV2(left_stick_val, right_stick_val, alternate_functions[0]);
    }
}
