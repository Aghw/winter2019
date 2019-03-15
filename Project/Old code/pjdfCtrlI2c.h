/*
    pjdfCtrlI2C.h
    I2C control definitions exposed to applications

    Developed for University of Washington embedded systems programming certificate
    
    2018/12 Nick Strathy wrote/arranged it after a framework by Paul Lever
*/

#ifndef __PJDFCTRLI2C_H__
#define __PJDFCTRLI2C_H__


// Control definitions for I2C

#define PJDF_CTRL_I2C_SET_DEVICE_ADDRESS  0x01   // Set the I2C device address for subsequent IO

// I2C methods
void BspI2C1_init();

//typedef INT8S PjdfErrCode;
//
//struct _DriverInternal; // forward declaration
//typedef struct _DriverInternal DriverInternal; // forward declaration
//
//// Device interface to be implemented by developers
//struct _DriverInternal
//{
//    char *pName;     // name used by applications and internally to identify the device
//    
//    // Method for initializing the device driver before exposing it to applications
//    PjdfErrCode (*Init)(DriverInternal *pDriver, char *pName);
//    
//    BOOLEAN initialized; // true if Init() ran successfully otherwise false.
//    OS_EVENT *sem;  // semaphore to serialize operations on the device 
//    INT8U refCount; // current number of Open handles to the device
//    INT8U maxRefCount; // Maximum Open handles allowed for the device
//    void *deviceContext; // device dependent data
//    
//    // Device-specific methods for operating on the device
//    PjdfErrCode (*Open)(DriverInternal *pDriver, INT8U flags);
//    PjdfErrCode (*Close)(DriverInternal *pDriver);
//    PjdfErrCode (*Read)(DriverInternal *pDriver, void* pBuffer, INT32U* pCount);
//    PjdfErrCode (*Write)(DriverInternal *pDriver, void* pBuffer, INT32U* pCount);
//    PjdfErrCode (*Ioctl)(DriverInternal *pDriver, INT8U request, void* pArgs, INT32U* pSize);
//};
//
//
//// Generic API methods exposed to applications for operating on devices
//static PjdfErrCode OpenI2C(DriverInternal *pDriver, INT8U flags);
//static PjdfErrCode CloseI2C(DriverInternal *pDriver);
//static PjdfErrCode ReadI2C(DriverInternal *pDriver, void* pBuffer, INT32U* pCount);
//static PjdfErrCode WriteI2C(DriverInternal *pDriver, void* pBuffer, INT32U* pCount);
//static PjdfErrCode IoctlI2C(DriverInternal *pDriver, INT8U request, void* pArgs, INT32U* pSize);
//PjdfErrCode InitI2C(DriverInternal *pDriver, char *pName);


#endif