/*
 * pwm.h
 *
 *  Created on: Dec 15, 2025
 *      Author: akila
 */

#ifndef CODE_PWM_DRIVER_H_
#define CODE_PWM_DRIVER_H_


#include <stdint.h>
#include <stdbool.h>

/* PWM Configuration */
#define PWM_MAX_SENSOR_VALUE    2048    // Maximum expected sensor reading
#define PWM_MIN_SENSOR_VALUE    0       // Minimum expected sensor reading
#define PWM_RANGE    2048       // full range


/* PWM Functions */
bool PWM_Init(void);
void PWM_SetDutyCycleFromSensor(int16_t sensor_value);
void PWM_Start(void);
void PWM_Stop(void);


#endif /* CODE_PWM_DRIVER_H_ */
