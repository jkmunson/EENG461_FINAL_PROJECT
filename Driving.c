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
#include "Wheels.h"

int32_t drivingSpeedLeft, drivingSpeedRight, drivingFwdBackLeft, drivingFwdBackRight;
int32_t drivingMaxSpeed = 2500;
int32_t drivingSpeedAdjustment;
int32_t drivingTurnAdjustment;

int Driving(mode, speed, direction)
{
    /*
     * This section is for inputs from the radio
     */
    if (speed >= 12){
        drivingFwdBackLeft = 16;
        drivingFwdBackRight = 0;
        drivingSpeedAdjustment = 19 - speed;
    }
    else if(speed <= 9){
        drivingFwdBackLeft = 0;
        drivingFwdBackRight = 32;
        drivingSpeedAdjustment = speed - 1;
    }
    else{
        drivingFwdBackLeft = 0;
        drivingFwdBackRight = 0;
        drivingSpeedAdjustment = 0;
    }
    drivingTurnAdjustment = (direction >= 12) ? direction - 10 : (direction <= 9) ? 10 - direction : 0;
    drivingSpeedLeft = drivingMaxSpeed * (drivingSpeedAdjustment + ((direction <= 9) ? drivingTurnAdjustment : 0));
    drivingSpeedRight = drivingMaxSpeed * (drivingSpeedAdjustment + ((direction >= 12) ? drivingTurnAdjustment : 0));
    /*
     * Call Wheels.c and provide necessary values.
     */
    Wheels(drivingSpeedLeft, drivingSpeedRight, drivingFwdBackLeft, drivingFwdBackRight);
	return 0;
}
