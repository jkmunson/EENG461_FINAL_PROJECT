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

int32_t drivingSpeedLeft, drivingSpeedRight, drivingFwdBackLeft, drivingFwdBackRight; //variables sent to Wheels.c
int32_t drivingMaxSpeed = 2500; //clock cycles the rpms are scaled by, the lower the clock cycles the higher the rpms
int32_t drivingSpeedAdjustment, drivingTurnAdjustment; //variables used for calculating driving speed left/right

//input "mode" is not used, would be integrated should LIDAR be added to RC car
int Driving(mode, throttle, turning)
{
    //forward on the RC stick corresponds to values 12-18
    //checks if RC is forward position
    if (throttle >= 12){
        //set left side stepper motors to rotate CCW
        drivingFwdBackLeft = 1<<4;
        //set right side stepper motors to rotate CW
        drivingFwdBackRight = 0<<6;
        //set speed adjustment
        drivingSpeedAdjustment = 19 - throttle;
    }

    //backwards on the RC stick corresponds to values 9-2
    //checks if RC is backward position
    else if(throttle <= 9){
        //set left side stepper motors to rotate CW
        drivingFwdBackLeft = 0<<4;
        //set right side stepper motors to rotate CCW
        drivingFwdBackRight = 1<<5;
        //set speed adjustment
        drivingSpeedAdjustment = throttle - 1;
    }

    //if RC stick is in the dead zone, set all values to 0
    else{
        drivingFwdBackLeft = 0<<4;
        drivingFwdBackRight = 0<<6;
        drivingSpeedAdjustment = 0;
    }

    //set Turn Adjustment value, turning right is from 12-20, turning left is from 0-9
    drivingTurnAdjustment = (turning >= 12) ? turning - 11 : (turning <= 9) ? 10 - turning : 0;
    //clock cycle for PWM is max speed scaled by the summation of speed adjustment and turn adjustment
    //speed adjustment will be the same for both right and left side
    //turn adjustment will increase how much max speed is scaled by, increasing the number of clock cycles for the PWM period and decreasing rpms
    drivingSpeedLeft = drivingMaxSpeed * (drivingSpeedAdjustment + ((turning <= 9) ? drivingTurnAdjustment : 0));
    drivingSpeedRight = drivingMaxSpeed * (drivingSpeedAdjustment + ((turning >= 12) ? drivingTurnAdjustment : 0));

    //Call Wheels.c and provide necessary values.
    Wheels(drivingSpeedLeft, drivingSpeedRight, drivingFwdBackLeft, drivingFwdBackRight);
	return 0;
}
