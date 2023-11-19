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

uint8_t start;
uint8_t mode;
int32_t spd;
int32_t dir;
int32_t driving_spd_l, driving_spd_r, driving_fwd_back_l, driving_fwd_back_r;

int Driving(start, mode, spd, dir)
{
    /*
     * This section is for inputs from the radio
     */

    /*
     * If the joystick for the radio is forward the spd value will be greater than or equal to 12 and max at 20
     */
    if (spd >= 12){
        /*
         * This values will set the GPIO pins going to the motor controller telling it to rotate forward.
         * One of these variables will need to be set to 0 depending on how the stepper motor/motor controllers are
         * placed on the car frame
         */
        driving_fwd_back_l = 16;
        driving_fwd_back_r = 32;
        /*
         * For forward motion the values from the radio are inversely proportional to the frequency of rotation.
         * For example: if spd == 12 then the driving speed will be set to 7, if spd == 18 the driving speed will be set
         * to 1. Driving speed will be multiplied by the period of the PWM needed to achieve 2 revolutions per minute(2500).
         * If driving speed is 7 then the period will be 7 * 2500, 7 times slower than 2 revolutions per minute.
         * If driving speed is 1 then the period will be 1 * 2500, the max speed at 2 revolutions per minute.
         */
        driving_spd_l = 19 - spd;
        driving_spd_r = 19 - spd;
        /*
         * To enable turning the period is increased for the stepper motors on the same side as the turning direction.
         * If dir is greater than or equal to 12 the car needs to turn right. To achieve this right side driving
         * speed is increased by dir scaled to a value from 2 - 10, increasing the period of the right side wheels
         * and decreasing the frequency of rotation.
         */
        if (dir >= 12){
            dir -= 10;
            driving_spd_r += dir;
        }
        /*
         * If dir is less than or equal to 9 the car needs to turn left. The amount of turn is inversely proportional
         * to the value of dir. Once the value of dir is corrected it is added to left side driving speed to slow
         * the frequency of rotation
         */
        else if (dir <= 9){
            dir = 11 - dir;
            driving_spd_l += dir;
        }
        /*
         * Scales the values of left and right side driving speeds by the max speed.
         */
        driving_spd_l = 2500 * driving_spd_l;
        driving_spd_r = 2500 * driving_spd_r;
    }
    /*
     * Repetition of above but for reverse direction. Variable spd is not inversely proportional to desired speed
     * for reverse direction.
     */
    else if(spd <= 9){
        driving_fwd_back_l = 0;
        driving_fwd_back_r = 0;
        driving_spd_l = spd;
        driving_spd_r = spd;
        if (dir >= 12){
            dir -= 10;
            driving_spd_r += dir;
        }
        else if (dir <= 9){
            dir = 11 - dir;
            driving_spd_l += dir;
        }
        driving_spd_l = 2500 * driving_spd_l;
        driving_spd_r = 2500 * driving_spd_r;
    }
    /*
     * Call Wheels.c and provide necessary values.
     */
    Wheels(start, driving_spd_l, driving_spd_r, driving_fwd_back_l, driving_fwd_back_r);
	return 0;
}
