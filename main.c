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
#include "Wheels.h"
#include "Driving.h"
#include "Lidar_recieve.h"
#include "Radio.h"

/*
 * The variable start will be used to decide which path the functions take when they are called. If start is 1 the
 * functions will take the necessary steps to configure. If start is 0 the functions will proceed with their intended
 * processes. So far this is only applicable to Wheels.c
 *
 * I have only included the most relevant files. The files such as Horn.h and Lights.h will need to be added to the include
 * list and to main should there be enough time.
 */

uint_8 start;

int main(void)
{
    /*
     * Configuration of the necessary peripherals.
     * Radio.c will enable interrupts once it is done configuring and will not need to be called again.
     */
    start = 1;
    Wheels(start);
    Radio();
    start = 0

    /*
     * Infinite loop that will repeatedly call Driving.c to update it.
     */
	while(1){
	    Driving(start);
	}
}
