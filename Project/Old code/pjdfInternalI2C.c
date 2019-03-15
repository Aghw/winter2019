/*
    pjdfInternalI2C.c
    The implementation of the internal PJDF interface pjdfInternal.h targetted for the
    Inter-Integrated Circuit (I2C)

    Developed for University of Washington embedded systems programming certificate
    
    2018/12 Nick Strathy wrote/arranged it after a framework by Paul Lever
*/

#include "bsp.h"
#include "pjdf.h"
#include "pjdfInternal.h"
#include "pjdfCtrlI2c.h"
#include "Adafruit_FT6206.h"

// Control registers etc for I2C hardware
typedef struct _PjdfContextI2C
{
    I2C_TypeDef *i2cMemMap; // Memory mapped register block for an I2C interface
    uint32_t i2CDevAddr;
} PjdfContextI2c;

static PjdfContextI2c i2c1Context = { PJDF_I2C1 };



// OpenI2C
// No special action required to open I2C device
static PjdfErrCode OpenI2C(DriverInternal *pDriver, INT8U flags)
{
    // Nothing to do
    return PJDF_ERR_NONE;
}

// CloseI2C
// No special action required to close I2C device
static PjdfErrCode CloseI2C(DriverInternal *pDriver)
{
    // Nothing to do
    return PJDF_ERR_NONE;
}

// ReadI2C
// Reads data from the peripheral device over the I2C interface.
//
// pDriver: pointer to an initialized I2C device
// pBuffer: on entry the first byte contains the starting address on the 
//     peripheral to read from. After reading, contains the bytes that were read.
// pCount: the number of bytes to read.
// Returns: PJDF_ERR_NONE if there was no error, otherwise an error code.
static PjdfErrCode ReadI2C(DriverInternal *pDriver, void* pBuffer, INT32U* pCount)
{
//	<your code here>
        uint8_t* data = (uint8_t *) pBuffer;
        
        PjdfContextI2c *pContext = (PjdfContextI2c*) pDriver->deviceContext;
        if (pContext == NULL) while(1);

        I2C_start(I2C1, FT6206_ADDR<<1, I2C_Direction_Transmitter);
        I2C_read_ack(I2C1);  
        I2C_stop(I2C1);
        I2C_start(I2C1, FT6206_ADDR<<1, I2C_Direction_Receiver);
        
        return PJDF_ERR_NONE;
//        I2C_TypeDef *i2c;
//        int iread = 0;
////      PjdfErrCode err = 0;
////      uint8_t i2cdat[16];
////      
////      I2C_start(I2C1, FT6206_ADDR<<1, I2C_Direction_Transmitter);
////      I2C_write(I2C1, (uint8_t)0);  
////      I2C_stop(I2C1);
////      I2C_start(I2C1, FT6206_ADDR<<1, I2C_Direction_Receiver);
////      
////      uint8_t i;
////      for (i=0; i<15; i++)
////        i2cdat[i] = I2C_read_ack(I2C1);
////      i2cdat[i] = I2C_read_nack(I2C1);
//       
//       PjdfContextI2c *pContext = (PjdfContextI2c*) pDriver->deviceContext;
//        if (pContext == NULL) while(1);
////        I2C_ReceiveData(pContext->i2cMemMap,  *pCount);
//        
////        int iread = 0;
//          i2c = pContext->i2cMemMap;
//        for (int i = 0; i < *pCount; i++) {
//            while(!I2C_GetFlagStatus(i2c, I2C_FLAG_TXE)); 
////            I2C_SendData(i2c, (uint8_t) pBuffer[i]);
////            I2C_SendData(spi, buffer[i]);
//            while(!I2C_GetFlagStatus(i2c, SPI_I2S_FLAG_RXNE));
////            pBuffer[iread++] = I2C_ReceiveData(i2c);
//        }
//        return PJDF_ERR_NONE;
//    
////    
////        // enable acknowledge of received data
////        I2C_AcknowledgeConfig(I2C1, ENABLE);
////        // wait until one byte has been received
////        while( !I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) );
////        // read data from I2C data register and return data byte
////        uint8_t data = I2C_ReceiveData(I2C1);
//        
////        return err;
}


// WriteI2C
// Writes the contents of the buffer to the peripheral device over the I2C interface.
//
// pDriver: pointer to an initialized I2C device
// pBuffer: the data to write to the device. On entry, the first byte contains
//     the address to write to on the peripheral device. The following bytes contain
//     the data to write.
// pCount: the number of bytes to write NOT including the address in the first byte.
// Returns: PJDF_ERR_NONE if there was no error, otherwise an error code.
static PjdfErrCode WriteI2C(DriverInternal *pDriver, void* pBuffer, INT32U* pCount)
{
//	<your code here>
    uint8_t* data = (uint8_t *) pBuffer;
    
    PjdfContextI2c *pContext = (PjdfContextI2c*) pDriver->deviceContext;
    if (pContext == NULL) while(1);
    I2C_SendData(pContext->i2cMemMap,  *data);
   
    I2C_start(I2C1, FT6206_ADDR<<1, I2C_Direction_Transmitter);
    I2C_write(I2C1, *data);
    I2C_stop(I2C1);
    
    return PJDF_ERR_NONE;
    
//    
//    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
//    I2C_SendData(I2C1, *data);
//    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
//    return err;
}

// IoctlI2C
// Handles the request codes defined in pjdfCtrlI2c.h
static PjdfErrCode IoctlI2C(DriverInternal *pDriver, INT8U request, void* pArgs, INT32U* pSize)
{
    INT8U osErr;
    PjdfContextI2c *pContext = (PjdfContextI2c*) pDriver->deviceContext;
    if (pContext == NULL) while(1);
    switch (request)
    {
    case PJDF_CTRL_I2C_SET_DEVICE_ADDRESS: // Set the I2C device address for subsequent IO
        pContext->i2CDevAddr = ((uint8_t*)pArgs)[0];
        break;
    default:
        while(1);
        break;
    }
    return PJDF_ERR_NONE;
}


// Initializes the given I2C driver.
PjdfErrCode InitI2C(DriverInternal *pDriver, char *pName)
{   
    if (strcmp (pName, pDriver->pName) != 0) while(1); // pName should have been initialized in driversInternal[] declaration
    
    // Initialize semaphore for serializing operations on the device 
    pDriver->sem = OSSemCreate(1); 
    if (pDriver->sem == NULL) while (1);  // not enough semaphores available
    pDriver->refCount = 0; // initial number of Open handles to the device
    
    // We may choose to handle multiple hardware instances of the I2C interface
    // each of which gets its own DriverInternal struct. Here we initialize 
    // the context of the I2C hardware instance specified by pName.
    if (strcmp(pName, PJDF_DEVICE_ID_I2C1) == 0)
    {
        pDriver->maxRefCount = 1; // Maximum refcount allowed for the device
        pDriver->deviceContext = (void*) &i2c1Context;
        BspI2C1_init(); // init I2C1 hardware
    }
  
    // Assign implemented functions to the interface pointers
    pDriver->Open = OpenI2C;
    pDriver->Close = CloseI2C;
    pDriver->Read = ReadI2C;
    pDriver->Write = WriteI2C;
    pDriver->Ioctl = IoctlI2C;
    
    pDriver->initialized = OS_TRUE;
    return PJDF_ERR_NONE;
}

// This is I2C PJDF driver intialization
void BspI2C1_init(void){
    GPIO_InitTypeDef GPIO_InitStruct;
    I2C_InitTypeDef I2C_InitStruct;
    
    // enable APB1 peripheral clock for I2C1
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    
    // enable clock for SCL and SDA pins
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    // Initialize GPIO_InitStruct (declared above) as follows
    // Set pins 8 and 9.
    // Set mode to alternate function (AF).
    // Set speed to 50 MHz
    // Set OType to open drain (OD).
    // Set pull-up/pull-down to pull up.
    // Call GPIO_Init() to initialize GPIOB with GPIO_InitStruct
    GPIO_InitStruct.GPIO_Pin = I2C_SCL | I2C_SDA;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // Connect I2C1 pins to AF:
    // Call GPIO_PinAFConfig once to set up pin 8 (SCL), once to set up pin 9 (SDA)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
    
    // configure I2C1
    // Initialze I2C_InitStruct (declared above) as follows:
    // set clock speed to 100000 (100 kHz)
    // set mode to I2C mode
    // set duty cycle to I2C_DutyCycle_2
    // set own address to 0
    // set Ack to disabled
    // set acknowledged address to 7 bits
    // Then call I2C_Init() to initialize I2C1 with I2C_InitStruc
    I2C_InitStruct.I2C_ClockSpeed = 100000;
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_OwnAddress1 = 0x00;
    I2C_InitStruct.I2C_Ack = I2C_Ack_Disable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &I2C_InitStruct);
    
    // Call I2C_Cmd() to enable I2C1
    I2C_Cmd(I2C1, ENABLE);
}

