/*
    bspI2c.h

    Board support for controlling I2C interfaces on NUCLEO-F401RE MCU

    Developed for University of Washington embedded systems programming certificate
    
    2016/2 Nick Strathy wrote/arranged it
*/

#ifndef ___BSPI2C_H
#define ___BSPI2C_H


#include "stm32f4xx.h"
#include "stm32f4xx_i2c.h"

#define PJDF_I2C1 I2C1 // Address of I2C memory mapped register block
#define I2C_SDA GPIO_Pin_9
#define I2C_SCL GPIO_Pin_8

void I2C1_init(void);
uint8_t I2C_read_ack(I2C_TypeDef* I2Cx);
uint8_t I2C_read_nack(I2C_TypeDef* I2Cx);
void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
void I2C_stop(I2C_TypeDef* I2Cx);
void I2C_write(I2C_TypeDef* I2Cx, uint8_t data);
void BspI2C1_init(void); // PJ Device Format driver initialization

#endif
