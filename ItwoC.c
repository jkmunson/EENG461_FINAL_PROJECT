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

uint32_t angle;

void I2C1_int(void){
    I2CSlaveIntDisableEx(I2C1_BASE, I2C_SLAVE_INT_DATA);
    I2CSlaveIntClearEx(I2C1_BASE, I2C_SLAVE_INT_DATA);
    angle = I2CSlaveDataGet(I2C1_BASE);
    I2CSlaveIntEnableEx(I2C1_BASE, I2C_SLAVE_INT_DATA);
}

int ItwoC(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C1)){
    }
    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);
    GPIOPinConfigure(GPIO_PA7_I2C1SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), true);
    I2CSlaveEnable(I2C1_BASE);
    I2CSlaveInit(I2C1_BASE, 0x3B);
    I2CSlaveAddressSet(I2C1_BASE, 0x3B, false);
    I2CIntRegister(I2C1_BASE, I2C1_int);
    I2CSlaveIntEnableEx(I2C1_BASE, I2C_SLAVE_INT_DATA);
    return(0);
}
