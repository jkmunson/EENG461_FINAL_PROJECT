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
//#include "Lidar_recieve.h"
#include "radio.h"

/*
 * I have only included the most relevant files. The files such as Horn.h and Lights.h will need to be added to the include
 * list and to main should there be enough time.
 */
int32_t mode = 1;

void radio(void);

int main(void)
{
    /*
     * Configuration of the necessary peripherals.
     * Radio.c will enable interrupts once it is done configuring and will not need to be called again.
     */
    Wheels(0,0,0,0);
    radio();

    /*
     * Infinite loop that will repeatedly call Driving.c to update it.
     */
	while(1){
	    Driving(mode, throttle, turning);
	}
}
