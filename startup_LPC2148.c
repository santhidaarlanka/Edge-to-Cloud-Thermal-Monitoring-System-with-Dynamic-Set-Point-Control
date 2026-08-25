/*
 * startup_LPC2148.c - Pure C Startup & Bootstrapper for NXP LPC2148 (ARM7TDMI-S)
 * Replaces assembly Startup.s to enforce 100% C language codebase architecture.
 */

#include <lpc21xx.h>

// Forward declaration of main entry point
extern int main(void);

// Reset & Exception Handlers in C
void Reset_Handler(void);
void Default_Handler(void);

// Weak aliases for IRQ/FIQ routines
void UNDEF_Routine(void) __attribute__((weak, alias("Default_Handler")));
void SWI_Routine(void)   __attribute__((weak, alias("Default_Handler")));
void DABT_Routine(void)  __attribute__((weak, alias("Default_Handler")));
void PABT_Routine(void)  __attribute__((weak, alias("Default_Handler")));
void FIQ_Routine(void)   __attribute__((weak, alias("Default_Handler")));

// Vector Table definition in C
void (* const vector_table[])(void) __attribute__((section(".vectors"))) = {
    Reset_Handler,  /* 0x00: Reset Vector */
    UNDEF_Routine,  /* 0x04: Undefined Instruction Vector */
    SWI_Routine,    /* 0x08: Software Interrupt Vector */
    PABT_Routine,   /* 0x0C: Prefetch Abort Vector */
    DABT_Routine,   /* 0x10: Data Abort Vector */
    0,              /* 0x14: Reserved / ISP Checksum */
    0,              /* 0x18: Hardware IRQ (Managed by VIC) */
    FIQ_Routine     /* 0x1C: Hardware FIQ */
};

// Default Exception Handler
void Default_Handler(void)
{
    while (1);
}

// Microcontroller C System Bootstrapper
void Reset_Handler(void)
{
    // Memory Accelerator Module (MAM) Initialization
    MAMCR = 0x00;   // Disable MAM before changing timing
    MAMTIM = 0x03;  // Set MAM fetch cycles to 3 CCLKs for 60MHz operation
    MAMCR = 0x02;   // Fully enable MAM functionality

    // Peripheral Bus Clock Setup (VPBDIV: PCLK = CCLK / 4 = 15MHz)
    VPBDIV = 0x00;

    // Jump directly to application main loop in C
    main();

    // Emergency loop if main ever returns
    while (1);
}
