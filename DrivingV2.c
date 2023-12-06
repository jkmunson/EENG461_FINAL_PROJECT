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

int32_t pwmLeft, pwmRight, absLeft, absRight, directionLeft, directionRight, angleScaled;
int32_t maxSpeed = 10000;
int32_t DZ = 100;

int DrivingV2(left, right, angle, function){
    if ( function == true ){
        absLeft = abs( left - DZ );
        if ( absLeft <= 10 ){
            pwmLeft = 0;
            pwmRight = 0;
            directionRight = 0<<5;
            directionLeft = 0<<4;
        }
        else{
            if ( angle < 90 ){
                pwmLeft = maxSpeed + 100 * ( 100 - absLeft + angle );
                pwmRight = maxSpeed + 100 * ( 100 - absLeft );
                directionRight = ( left > DZ ) ? 0<<5 : 1<<5;
                directionLeft = 1<<4;
            }
            else if ( angle < 180 ){
                angleScaled = 180 - angle;
                pwmLeft = maxSpeed + 100 * ( 100 - absLeft + angleScaled );
                pwmRight = maxSpeed + 100 * ( 100 - absLeft );
                directionRight = ( left > DZ ) ? 0<<5 : 1<<5;
                directionLeft = 0<<4;
            }
            else if ( angle < 270 ){
                angleScaled = angle - 180;
                pwmRight = maxSpeed + 100 * ( 100 - absLeft + angleScaled );
                pwmLeft = maxSpeed + 100 * ( 100 - absLeft );
                directionRight = 0<<5;
                directionLeft = ( left > DZ ) ? 1<<4 : 0<<4;
            }
            else{
                angleScaled = 360 - angle;
                pwmRight = maxSpeed + 100 * ( 100 - absLeft + angleScaled );
                pwmLeft = maxSpeed + 100 * ( 100 - absLeft );
                directionRight = 1<<5;
                directionLeft = ( left > DZ ) ? 1<<4 : 0<<4;
            }
        }
    }

    else {
        //left and right will go from 0 - 200
        //DZ will equal 100 and the dead zone being 100 +/- 5
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
        directionRight = ( right > DZ ) ? 0<<5 : 1<<5;
        directionLeft = ( left > DZ ) ? 1<<4 : 0<<4;
    }
    /*
     * Call Wheels.c and provide necessary values.
     */
    WheelsV2(pwmRight, pwmLeft, directionRight, directionLeft);
    return 0;
}
