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
 * processes.
 *
 * Wheels.c is not called in main as it will be called by Driving.c when Driving.c has finished its tasks and is ready
 * to pass on values to Wheels.c
 *
 * I have only included the most relevant files. The files such as Horn.h and Lights.h will need to be added to the include
 * list and to main should there be enough time.
 */

uint_8 start;

int main(void)
{
    /*
     * Configuration of the necessary peripherals
     */
    start = 1;
    Driving(start);
    Lidar_recieve(start);
    Radio(start);
    start = 0

    /*
     * Infinite loop that will repeatedly call the functions to check for changes. This method may need to be changed for
     * Lidar_recieve.c and Radio.c as time capture and I2C communications may take longer than this loop will allow.
     */
	while(1){
	    Lidar_recieve(start);
	    Radio(start);
	    Driving(start);
	}
}
