
#include "stdout.h"


extern stdout_t uart;

void printHex(uint32_t val, uint8_t digits)
{
  for(int16_t i = 0; i < digits; i++){
    uint8_t shift = ((digits - 1) * 4) - (i * 4);
    uint8_t byte = (val >> shift) & 0x0f;
    uart.xputchar(((char*)"0123456789ABCDEF")[byte]);
  }
}

void DumpStack(uint32_t *sp, uint32_t len)
{
  uart.xputs("\nStack:\n");
  for(uint8_t i = 0; i < len; ){
    printHex((uint32_t)sp, 8);
    uart.xputs(":   ");
    uint8_t line = (len < 4) ? len : 4;
    for(uint8_t j = 0; j < line; j++, i++, sp++){
      printHex((uint32_t)(*sp), 8);
      uart.xputs("    ");
    }
    uart.xputchar('\n');
  }
}

__attribute__ ((naked)) 
void stackTrace(uint32_t *stack){
    uint32_t r0 = stack[0];
    uint32_t r1 = stack[1];
    uint32_t r2 = stack[2];
    uint32_t r3 = stack[3];
    uint32_t r12 = stack[4];
    uint32_t lr = stack[5];
    uint32_t pc = stack[6];
    uint32_t psr = stack[7];

    uart.init();
    uart.xputs("\nHard fault");

    uart.xputs("\nR0  = ");
    printHex( r0, 8);
    uart.xputs("\nR1  = ");
    printHex( r1, 8);
    uart.xputs("\nR2  = ");
    printHex( r2, 8);
    uart.xputs("\nR3  = ");
    printHex( r3, 8);
    uart.xputs("\nR12 = ");
    printHex( r12, 8);
    uart.xputs("\nLR  = ");
    printHex( lr, 8);
    uart.xputs("\nPC  = ");
    printHex( pc, 8);
    uart.xputs("\nPSR = ");
    printHex( psr, 8);
    
    uart.xputchar('\n');

    DumpStack(stack + 8, 16);
}
/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  asm volatile ("TST LR, #4");
  asm volatile ("ITE EQ");
  asm volatile ("MRSEQ R0, MSP");
  asm volatile ("MRSNE R0, PSP");
  asm volatile ("B stackTrace");
}