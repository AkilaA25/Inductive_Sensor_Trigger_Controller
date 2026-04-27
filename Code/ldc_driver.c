/*
 * ldc_driver.c
 *
 *  Created on: Dec 14, 2025
 *      Author: akila
 */


#include "ldc_driver.h"
#include "i2c_driver.h"
#include "ti_msp_dl_config.h"
#include <stdio.h>

// functions
static bool LDC2114_WriteRegister(uint8_t reg, uint8_t value);
static bool LDC2114_ReadRegister(uint8_t reg, uint8_t *value);
static bool LDC2114_ReadMultipleRegisters(uint8_t startReg, uint8_t *data, uint8_t len);
static bool LDC2114_EnterConfigMode(void);
static bool LDC2114_ExitConfigMode(void);

/*
 *  Initiate:
 *      - wait for startup
 *      - enter congif mode
 *      - turn on ch0
 *      - make other changes (to be done later on)
 *      - leave config mode
 */
bool LDC2114_Init(void)
{
    uint8_t status;
    uint8_t data[3];

    // wait for LDC to turn on
    delay_cycles(10000000);

    // get status register 0
    if (!LDC2114_ReadRegister(LDC2114_REG_STATUS, &status)) return false;

    // checks:
    if (!(status & LDC2114_STATUS_CHIP_READY)) {
        return false;
    }
    // to be added to later... idk wat tho

    // config
    if (!LDC2114_WriteRegister(LDC2114_REG_RESET, LDC2114_CONFIG_MODE_ENABLE)) return false;
    if (!LDC2114_WriteRegister(LDC2114_REG_EN, 0x01)) return false;
    if (!LDC2114_WriteRegister(LDC2114_REG_NP_SCAN_RATE, 0x00)) return false;
    if (!LDC2114_WriteRegister(LDC2114_REG_GAIN0, 0x22)) return false;
    if (!LDC2114_WriteRegister(0x20, 0xAF)) return false;
    if (!LDC2114_WriteRegister(0x1E, 0x00)) return false;
    if (!LDC2114_WriteRegister(0x17, 0x01)) return false;
    if (!LDC2114_WriteRegister(LDC2114_REG_INTPOL, 0x05)) return false;
    if (!LDC2114_WriteRegister(LDC2114_REG_OPOL_DPOL, 0xFF)) return false;
    if (!LDC2114_WriteRegister(LDC2114_REG_RESET, 0x00)) return false;

    if (!LDC2114_ReadMultipleRegisters(LDC2114_REG_STATUS, data, 3)) return false;

    // wait for read
    delay_cycles(5000000);

    printf("Init Status: ");
    for (int i=0; i<3; i++){
        printf("0x%02X ", data[i]);
    }
    printf("\n");

    return true;
}


bool LDC2114_ReadChannel0(LDC2114_Data *data)
{
    uint8_t rxData[3];

    if (!LDC2114_ReadRegister(LDC2114_REG_STATUS, &data->status)) {
        return false;
    }
    if (!LDC2114_ReadMultipleRegisters(LDC2114_REG_OUT, rxData, 3)) {
        return false;
    }

    data->out_status = rxData[0];
    data->data = (int16_t)(((rxData[2] & 0x0F) << 8) | rxData[1]);
    if (data->data & 0x0800) {
        data->data |= 0xF000;
    }
    return true;
}


// write to a register
// when writing or reading make sure that the first value sent is the register and then the actual value
static bool LDC2114_WriteRegister(uint8_t reg, uint8_t value)
{
    gTxPacket[0] = reg;
    gTxPacket[1] = value;
    gTxLen = 2;

    I2C_WriteBytes(LDC2114_I2C_ADDR, (const uint8_t *)gTxPacket, gTxLen);

    return (gI2cControllerStatus == I2C_STATUS_TX_COMPLETE);
}

// read ldc code
static bool LDC2114_ReadRegister(uint8_t reg, uint8_t *value){
    gTxPacket[0] = reg;
    gTxLen = 1;
    I2C_WriteBytes(LDC2114_I2C_ADDR, (const uint8_t *)gTxPacket, gTxLen);

    if (gI2cControllerStatus != I2C_STATUS_TX_COMPLETE) {
        return false;
    }

    gRxLen = 1;
    I2C_ReadBytes(LDC2114_I2C_ADDR, gRxLen);

    if (gI2cControllerStatus == I2C_STATUS_RX_COMPLETE) {
        *value = gRxPacket[0];
        return true;
    }

    return false;
}

// read hella registers
static bool LDC2114_ReadMultipleRegisters(uint8_t startReg, uint8_t *data, uint8_t len)
{
    gTxPacket[0] = startReg;
    gTxLen = 1;
    I2C_WriteBytes(LDC2114_I2C_ADDR, (const uint8_t *)gTxPacket, gTxLen);


    if (gI2cControllerStatus != I2C_STATUS_TX_COMPLETE) {
        return false;
    }

    gRxLen = len;
    I2C_ReadBytes(LDC2114_I2C_ADDR, gRxLen);

    if (gI2cControllerStatus == I2C_STATUS_RX_COMPLETE) {
        for (uint8_t i = 0; i < len; i++) {
            data[i] = gRxPacket[i];
        }
        return true;
    }

    return false;
}
