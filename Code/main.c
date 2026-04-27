#include "ti_msp_dl_config.h"
#include "i2c_driver.h"
#include "ldc_driver.h"
#include "pwm_driver.h"
#include <stdio.h>

LDC2114_Data gLDC2114Data;
MovingAverageFilter gSensorFilter;

int main(void) {
    SYSCFG_DL_init();
    NVIC_EnableIRQ(I2C_INST_INT_IRQN);
    DL_SYSCTL_disableSleepOnExit();

    PWM_Init();
    delay_cycles(32000000);   // LDC power-up
    LDC2114_Init();
    Filter_Init(&gSensorFilter, 8);
    PWM_Start();

    while (1) {
        if (LDC2114_ReadChannel0(&gLDC2114Data)) {
            int16_t filtered = Filter_Update(&gSensorFilter, gLDC2114Data.data);
            printf("Raw: %d, Filtered: %d, Out: 0x%02X\n", gLDC2114Data.data, filtered, gLDC2114Data.out_status);
            if (gLDC2114Data.out_status & 0x01) {
                PWM_SetDutyCycleFromSensor(filtered);
            } else {
                PWM_SetDutyCycleFromSensor(0);
            }
        }
        delay_cycles(1600000);
    }
}
