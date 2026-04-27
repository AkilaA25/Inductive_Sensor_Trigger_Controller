/*
 * ldc_driver.h
 *
 *  Created on: Dec 14, 2025
 *      Author: akila
 */

#ifndef CODE_LDC_DRIVER_H_
#define CODE_LDC_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>
#include "filter.h"  // Add this

// LDC addy
#define LDC2114_I2C_ADDR (0x2A)

// important registers
#define LDC2114_REG_STATUS          0x00
#define LDC2114_REG_OUT             0x01
#define LDC2114_REG_DATA0_LSB       0x02
#define LDC2114_REG_DATA0_MSB       0x03
#define LDC2114_REG_RESET           0x0A
#define LDC2114_REG_EN              0x0C
#define LDC2114_REG_NP_SCAN_RATE    0x0D
#define LDC2114_REG_GAIN0           0x0E
#define LDC2114_REG_INTPOL          0x11
#define LDC2114_REG_OPOL_DPOL       0x1C


// masks to check bits later on
#define LDC2114_STATUS_RDY_TO_WRITE (1 << 5)
#define LDC2114_STATUS_CHIP_READY   (1 << 6)
#define LDC2114_CONFIG_MODE_ENABLE  (1)

// data structure for storing ldc info
typedef struct {
    int16_t data;
    uint8_t out_status;
    uint8_t status;
} LDC2114_Data;

// functions
bool LDC2114_Init(void);
bool LDC2114_ReadChannel0(LDC2114_Data *data);


#endif /* CODE_LDC_DRIVER_H_ */
