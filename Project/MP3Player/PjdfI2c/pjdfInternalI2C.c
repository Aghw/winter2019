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
//#include "pjdfCtrlI2c.h"
#include "Adafruit_FT6206.h"

// Control registers etc for I2C hardware
typedef struct _PjdfContextI2C
{
    I2C_TypeDef *i2cMemMap; // Memory mapped register block for an I2C interface
    uint32_t i2CDevAddr;
} PjdfContextI2c;

static PjdfContextI2c i2c1Context = { PJDF_I2C1, };



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
        uint16_t* data = (uint16_t *) pBuffer;
//        uint8_t val;

        PjdfContextI2c *pContext = (PjdfContextI2c*) pDriver->deviceContext;
        if (pContext == NULL) while(1);
//        I2C_GetBuffer(pContext->i2cMemMap, (INT8U*) pBuffer, *pCount);
        
        I2C_start(I2C1, FT6206_ADDR<<1, I2C_Direction_Transmitter);
        I2C_write(I2C1, *data);  
        I2C_stop(I2C1);
        I2C_start(I2C1, FT6206_ADDR<<1, I2C_Direction_Receiver);
        
//        Adafruit_FT6206 ts = Adafruit_FT6206();  // The touch controller
//        int X, Y;
////        ts.begin();
////        TS_Point p = TS_Point();
//          X = map(p.y, TS_MINX, TS_MAXX, ts.height(), 0);
//          Y = map(p.x, TS_MINY, TS_MAXY, 0, ts.width());
//    
//          if (p.x != 0 || p.y != 0)
//          {
//              val = 0;
//          }
//          else {
//            if(p.x >= (CIRC_X0_COORDINATES - CIRC_R0_COORDINATES) && 
//               p.x <= (CIRC_X0_COORDINATES + CIRC_R0_COORDINATES + 1) && 
//               p.y >= (CIRC_Y0_COORDINATES - CIRC_R0_COORDINATES) && 
//               p.y <= (CIRC_Y0_COORDINATES + CIRC_R0_COORDINATES + 1) ) 
//              val = 1;
//          }
        
        return PJDF_ERR_NONE;
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
//    uint16_t* data = (uint16_t *) pBuffer;
    
    PjdfContextI2c *pContext = (PjdfContextI2c*) pDriver->deviceContext;
    if (pContext == NULL) while(1);

//    I2C_SendData(pContext->i2cMemMap,  *data);
   
//    I2C_start(I2C1, FT6206_ADDR<<1, I2C_Direction_Transmitter);
//    I2C_write(I2C1, *data);
//    I2C_stop(I2C1);
//    
    return PJDF_ERR_NONE;
}

// IoctlI2C
// Handles the request codes defined in pjdfCtrlI2c.h
static PjdfErrCode IoctlI2C(DriverInternal *pDriver, INT8U request, void* pArgs, INT32U* pSize)
{
//    INT8U osErr;
    PjdfContextI2c *pContext = (PjdfContextI2c*) pDriver->deviceContext;
    if (pContext == NULL) while(1);
    switch (request)
    {
    case PJDF_CTRL_I2C_SET_DEVICE_ADDRESS: // Set the I2C device address for subsequent IO
        
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
    I2C1_init();
    
}

