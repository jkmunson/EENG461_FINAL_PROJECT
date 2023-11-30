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
#include <DrivingV2.h>
#include <WheelsV2.h>

#include "radioV2.h"

void radioV2(void);

int main(void)
{
    WheelsV2(0,0,0,0);
    radioV2();

    while(1){
        DrivingV2(left_stick_val, right_stick_val);
    }
}
