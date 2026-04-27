/*
 * pwm_driver.c
 *
 *  Created on: Dec 15, 2025
 *      Author: akila
 */



#include "pwm_driver.h"
#include "ti_msp_dl_config.h"
#include <stdio.h>

/* Private variables */
static uint16_t gPwmPeriod = 0;

/*
 * Initialize PWM
 * Call this after SYSCFG_DL_init()
 */
bool PWM_Init(void)
{
    // Get the timer period from configuration
    gPwmPeriod = DL_Timer_getLoadValue(PWM_INST);

    if (gPwmPeriod == 0) {
        printf("ERROR: PWM timer period is 0\n");
        return false;
    }

    return true;
}

/*
 * Convert sensor value (0-1024) to PWM duty cycle
 * sensor_value: Raw sensor reading from LDC2114
 */
void PWM_SetDutyCycleFromSensor(int16_t sensor_value)
{
    int32_t shifted;
    uint16_t duty_cycle;

    // Clamp sensor value to valid range
    if (sensor_value < PWM_MIN_SENSOR_VALUE) {
        sensor_value = PWM_MIN_SENSOR_VALUE;
    } else if (sensor_value > PWM_MAX_SENSOR_VALUE) {
        sensor_value = PWM_MAX_SENSOR_VALUE;
    }

    shifted = (int32_t)sensor_value - PWM_MIN_SENSOR_VALUE;

    // Map sensor value (0-1024) to duty cycle (0-period)
    // duty_cycle = (sensor_value * period) / 1024
    duty_cycle = ((uint32_t)shifted * gPwmPeriod) / PWM_RANGE;

    // Set the duty cycle
    DL_Timer_setCaptureCompareValue(PWM_INST, duty_cycle, DL_TIMER_CC_0_INDEX);

}

/*
 * Start PWM output
 */
void PWM_Start(void)
{
    DL_Timer_startCounter(PWM_INST);
}

/*
 * Stop PWM output
 */
void PWM_Stop(void)
{
    DL_Timer_stopCounter(PWM_INST);
}

