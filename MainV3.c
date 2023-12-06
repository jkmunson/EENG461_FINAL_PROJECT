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
#include <DrivingV3.h>
#include <WheelsV2.h>
#include "radioV2.h"
#include "sonic_sensor.h"


void radioV2(void);
void sonic_sensor(void);

int main(void)
{
    WheelsV2(0,0,0,0);
    radioV2();
    sonic_sensor();

    while(1){
        DrivingV3(left_stick_val, right_stick_val, um_distance, alternate_functions[0]);
    }
}
