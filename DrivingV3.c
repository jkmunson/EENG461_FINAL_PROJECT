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

bool stop = false;
int32_t speedLeft, speedRight, pwmLeft, pwmRight, absLeft, absRight, directionLeft, directionRight;
int32_t maxSpeed = 100;
int32_t DZ = 100;

int DrivingV3(left, right, distance, alternate_function)
{
    //left and right will go from 0 - 200
    //DZ will equal 100 the the dead zone being 100 +/- 5
    absLeft = abs( left - DZ );
    absRight = abs (right - DZ );


    if (distance <= 300000){
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 1<<7);
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, 0<<6);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0<<1);
        pwmLeft  = 0;
        pwmRight = 0;
        stop = true;
    }

    if (alternate_function == true) {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0<<7);
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, 0<<6);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 1<<1);
        stop = false;
    }else if (stop == false & alternate_function == false){
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0<<7);
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, 1<<6);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0<<1);
    }

    if ( stop == true){
        pwmLeft  = 0;
        pwmRight = 0;
    }
    else{
        //calculate clock cycles for PWM
        if ( absLeft <= 10 ){
            pwmLeft = 0;
        } else{
            pwmLeft = maxSpeed + 10 * (100 - absLeft);
        }
        if ( absRight <= 10 ){
            pwmRight = 0;
        } else{
            pwmRight = maxSpeed + 10 * (100 - absRight);
        }
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
