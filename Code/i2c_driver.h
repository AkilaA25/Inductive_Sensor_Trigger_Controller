/*
 * i2c_driver.h
 *
 *  Created on: Dec 14, 2025
 *      Author: akila
 */

#ifndef CODE_I2C_DRIVER_H_
#define CODE_I2C_DRIVER_H_

#include "ti_msp_dl_config.h"
#include <stdint.h>
#include <stdbool.h>

// set the packets to 16 since there are 2 bytes that the ldc stores
#define I2C_TX_MAX_PACKET_SIZE (16)
#define I2C_RX_MAX_PACKET_SIZE (16)

// i2c status
enum I2cControllerStatus {
    I2C_STATUS_IDLE = 0,
    I2C_STATUS_TX_STARTED,
    I2C_STATUS_TX_INPROGRESS,
    I2C_STATUS_TX_COMPLETE,
    I2C_STATUS_RX_STARTED,
    I2C_STATUS_RX_INPROGRESS,
    I2C_STATUS_RX_COMPLETE,
    I2C_STATUS_ERROR,
};

// globals
extern volatile uint8_t gTxPacket[I2C_TX_MAX_PACKET_SIZE];
extern volatile uint32_t gTxLen, gTxCount;
extern volatile uint8_t gRxPacket[I2C_RX_MAX_PACKET_SIZE];
extern volatile uint32_t gRxLen, gRxCount;
extern volatile enum I2cControllerStatus gI2cControllerStatus;

// important functions
void I2C_WriteBytes(uint8_t addr, const uint8_t *data, uint32_t len);
void I2C_ReadBytes(uint8_t addr, uint32_t len);



#endif /* CODE_I2C_DRIVER_H_ */
