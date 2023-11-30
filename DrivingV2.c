#include <stdio.h>
#include <stdlib.h>
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

int32_t speedLeft, speedRight, pwmLeft, pwmRight, absLeft, absRight, directionLeft, directionRight;
int32_t maxSpeed = 10000;
int32_t DZ = 100;

int DrivingV2(left, right)
{
    //left and right will go from 0 - 200
    //DZ will equal 100 the the dead zone being 100 +/- 5
    absLeft = abs( left - DZ );
    absRight = abs (right - DZ );


    //calculate clock cycles for PWM
    if ( absLeft <= 10 ){
        pwmLeft = 0;
    } else{
        pwmLeft = maxSpeed + 100 * (100 - absLeft);
    }
    if ( absRight <= 10 ){
        pwmRight = 0;
    } else{
        pwmRight = maxSpeed + 100 * (100 - absRight);
    }

    //determine rotation of stepper motors
    directionLeft = ( right > DZ ) ? 0<<5 : 1<<5;
    directionRight = ( left > DZ ) ? 1<<4 : 0<<4;

    /*
     * Call Wheels.c and provide necessary values.
     */
    WheelsV2(pwmRight, pwmLeft, directionLeft, directionRight);
	return 0;
}
