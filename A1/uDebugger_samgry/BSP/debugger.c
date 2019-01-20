#include <stdint.h>
#include "print.h"

static int counter = 10;
/*
 *
 * Part of a fault exception handler. Prints the given register values.
 * pc: the value of the program counter when the fault occurred.
 * lr: the value of the link register when the fault occurred.
 *
 */
void FaultPrint(uint32_t pc, uint32_t lr)
{
  if (counter > 0) {
    // TODO: Print an error message specifying the PC and LR values when the fault occurred
    // <Your code here>
    PrintString("Hard fault at PC=0x"); PrintHex(pc); PrintString(" and LR=0x"); PrintHex(lr);
    PrintString("\n");
    --counter;
  }
}
