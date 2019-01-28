/*******************************************************************************
File name       : testAndSet.s
Description     : Assembly language function for test-and-set mutual exclusion lock
*******************************************************************************/   
    
    PUBLIC testAndSet
    
    SECTION .text:CODE:REORDER:NOROOT(2)
    THUMB
    
  
/*******************************************************************************
Function Name   : testAndSet
Description     : sets the lock for the test-and-set resource protection
C Prototype     : int testAndSet()
                : Where gTSLock is the value to lock print buffer 
Parameters      : R0: Address of gTSLock
Return value    : R0
*******************************************************************************/  
  
testAndSet
    MOVS R2, #1;  use for locking STREX
get_lock_loop
    LDREX R1, [R0]      //
    CMP   R1, #0        // is the lock free?
    BNE  get_lock_loop  // it is locked, retry again
    STREX R1, R2, [R0]  // Try set gTLock to 1 using STREX
    CMP   R1, #0        // check return status of STREX
    BNE   get_lock_loop // STREX was not successful. retry
    MOV   R0, R1	// R0 = previous value of #gTSLock
    DMB                 // Data Memory Barrier
    
    BX  LR              // Retrun
    
    END
