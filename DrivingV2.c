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
#include <driverlib/uart.h>
#include <WheelsV2.h>
#include <radioV2.h>

int32_t pwmLeft, pwmRight, absLeft, absRight, directionLeft, directionRight;
int32_t maxSpeed = 10000;
int32_t DZ = 100;

int DrivingV2(left, right, function){
    static int8_t angle;

    //Check if alternate function is enabled, alternate function enables LIDAR control
    if ( function == true ){
        absLeft = abs( left - DZ );

        //Get data from UART
        do{
            UARTRxErrorClear(UART0_BASE);
            int8_t received = UARTCharGet(UART0_BASE);
            if (UARTRxErrorGet(UART0_BASE)) continue;
            angle = received;
        }while(UARTCharsAvail(UART0_BASE));

        //printf("angle %d", angle);
        //If left stick is in the dead zone set all values for WheelsV2 to 0
        if ( absLeft <= 10 ){
            pwmLeft = 0;
            pwmRight = 0;
            directionRight = 0<<5;
            directionLeft = 0<<4;
        }
        else{

            //Will turn in place toward the left at a speed determined by the controllers left stick
            if ( angle <= -85 ){
                pwmLeft = maxSpeed + 100 * ( 100 - absLeft );
                pwmRight = maxSpeed + 100 * ( 100 - absLeft );
                directionRight = 1<<5;
                directionLeft = 1<<4;
            }

            //Will turn in place toward the right at a speed determined by the controllers left stick
            else if ( angle >= 85 ){
                pwmLeft = maxSpeed + 100 * ( 100 - absLeft );
                pwmRight = maxSpeed + 100 * ( 100 - absLeft );
                directionRight = 1<<5;
                directionLeft = 1<<4;
            }

            //If angle received is less than -9 reduce the speed of the left side wheels
            else if ( angle < -9 ){
                pwmLeft = maxSpeed + 100 * ( 100 - absLeft + 3 * abs(angle) );
                pwmRight = maxSpeed + 100 * ( 100 - absLeft );
                directionRight = 0<<5;
                directionLeft = 1<<4;
            }

            //If angle received is greater than 9 reduce the speed of the right side wheels
            else if ( angle > 9 ){
                pwmLeft = maxSpeed + 100 * ( 100 - absLeft );
                pwmRight = maxSpeed + 100 * ( 100 - absLeft + 3 * abs(angle) );
                directionRight = 0<<5;
                directionLeft = 1<<4;
            }

            //Else left and right side will have the same speed
            else{
                pwmLeft = maxSpeed + 100 * ( 100 - absLeft );
                pwmRight = maxSpeed + 100 * ( 100 - absLeft );
                directionRight = 0<<5;
                directionLeft = 1<<4;
            }
        }
    }

    else {
        //Normal function is tank control via controller

        //left and right will go from 0 - 200
        //DZ will equal 100 and the dead zone being 100 +/- 5
        absLeft = abs( left - DZ );
        absRight = abs( right - DZ );


        //calculate clock cycles for PWM period
        if ( absLeft <= 10 ){
            pwmLeft = 0;
        } else{
            pwmLeft = maxSpeed + 100 * ( 100 - absLeft );
        }
        if ( absRight <= 10 ){
            pwmRight = 0;
        } else{
            pwmRight = maxSpeed + 100 * ( 100 - absRight );
        }

        //determine rotation of stepper motors
        directionRight = ( right > DZ ) ? 0<<5 : 1<<5;
        directionLeft = ( left > DZ ) ? 1<<4 : 0<<4;
    }
    //printf("left speed %d right speed %d left dir %d right dir %d alt0 %d\n",
           //pwmLeft, pwmRight, directionLeft, directionRight, alternate_functions[0]);
    //Call WheelsV2 and provide necessary values
    WheelsV2(pwmRight, pwmLeft, directionRight, directionLeft);
    return 0;
}
