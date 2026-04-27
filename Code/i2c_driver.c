/*
 * i2c_driver.c
 *
 *  Created on: Dec 14, 2025
 *      Author: akila
 */


#include "i2c_driver.h"
#include <stdio.h>

// globals
volatile uint8_t gTxPacket[I2C_TX_MAX_PACKET_SIZE];
volatile uint32_t gTxLen, gTxCount;
volatile uint8_t gRxPacket[I2C_RX_MAX_PACKET_SIZE];
volatile uint32_t gRxLen, gRxCount;
volatile enum I2cControllerStatus gI2cControllerStatus;

// write
void I2C_WriteBytes(uint8_t addr, const uint8_t *data, uint32_t len)
{
    gI2cControllerStatus = I2C_STATUS_IDLE;
    gTxCount = 0;

    // fills i2c fifo and the return is the amount filled
    gTxCount = DL_I2C_fillControllerTXFIFO(I2C_INST, data, len);

    // if more bytes to send enable interrupt which refills the fifo
    if (gTxCount < len) {
        DL_I2C_enableInterrupt(I2C_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
    } else {
        DL_I2C_disableInterrupt(I2C_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
    }

    // wait till a previous transfer is done
    gI2cControllerStatus = I2C_STATUS_TX_STARTED;
    while (!(DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

    // start sending out the i2c signal
    DL_I2C_startControllerTransfer(I2C_INST, addr, DL_I2C_CONTROLLER_DIRECTION_TX, len);

    // status update from hardware when buffer is empty
    while ((gI2cControllerStatus != I2C_STATUS_TX_COMPLETE) &&
           (gI2cControllerStatus != I2C_STATUS_ERROR)) {
        __WFE();
    }

    while (DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);   // wait for stop and bus clear
}

// i2c read function
// write the register to read from outside of this function with writebyte()
void I2C_ReadBytes(uint8_t addr, uint32_t len)
{
    gI2cControllerStatus = I2C_STATUS_IDLE;
    gRxCount = 0;

    //clear
    for (uint32_t i = 0; i < I2C_RX_MAX_PACKET_SIZE; i++) {
        gRxPacket[i] = 0;
    }

    gI2cControllerStatus = I2C_STATUS_RX_STARTED;
    while (!(DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));    // wait till the end of another transaction omfg

    DL_I2C_startControllerTransfer(I2C_INST, addr, DL_I2C_CONTROLLER_DIRECTION_RX, len);    //read

    while (gI2cControllerStatus != I2C_STATUS_RX_COMPLETE) {    // wait till read ends
        __WFE();
    }

    while (DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);   //clear
}

// i2c intterupt handler
void I2C_INST_IRQHandler(void){

    switch (DL_I2C_getPendingInterrupt(I2C_INST)) { //priority based interrupt triggering
        case DL_I2C_IIDX_CONTROLLER_RX_DONE:
            gI2cControllerStatus = I2C_STATUS_RX_COMPLETE;
            break;

        case DL_I2C_IIDX_CONTROLLER_TX_DONE:    // managed by hardare trigger
            DL_I2C_disableInterrupt(I2C_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
            gI2cControllerStatus = I2C_STATUS_TX_COMPLETE;
            break;

        case DL_I2C_IIDX_CONTROLLER_RXFIFO_TRIGGER:
            gI2cControllerStatus = I2C_STATUS_RX_INPROGRESS;

            while (DL_I2C_isControllerRXFIFOEmpty(I2C_INST) != true) {  //read till the amount requested is read
                if (gRxCount < gRxLen) {
                    gRxPacket[gRxCount++] = DL_I2C_receiveControllerData(I2C_INST);
                } else {
                    DL_I2C_receiveControllerData(I2C_INST);
                }
            }
            break;

        case DL_I2C_IIDX_CONTROLLER_TXFIFO_TRIGGER:
            gI2cControllerStatus = I2C_STATUS_TX_INPROGRESS;
            // as space is created in the transfer fifo, it fills it
            if (gTxCount < gTxLen) {
                gTxCount += DL_I2C_fillControllerTXFIFO(I2C_INST, (const uint8_t *)&gTxPacket[gTxCount], gTxLen - gTxCount);
            }
            break;

        case DL_I2C_IIDX_CONTROLLER_ARBITRATION_LOST:
            printf("CONTROLLER ARBRITRATION LOST");
            break;

        case DL_I2C_IIDX_CONTROLLER_NACK:
            if ((gI2cControllerStatus == I2C_STATUS_RX_STARTED) ||
                (gI2cControllerStatus == I2C_STATUS_TX_STARTED)) {
                gI2cControllerStatus = I2C_STATUS_ERROR;
            }
            break;

        default:
            break;
    }
}
