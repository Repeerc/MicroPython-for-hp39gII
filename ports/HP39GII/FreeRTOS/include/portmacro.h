/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */


#ifndef PORTMACRO_H
#define PORTMACRO_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	uint32_t
#define portBASE_TYPE	long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif
/*-----------------------------------------------------------*/

/* Hardware specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT			8
#define portYIELD()					__asm ( "SWI 0" )
#define portNOP()					__asm ( "NOP" )
#define portYIELD_FROM_ISR(x)        vTaskSwitchContext()
/*-----------------------------------------------------------*/

volatile uint32_t saved_sp;
volatile uint32_t tsave_reg1;
volatile uint32_t tsave_reg2;
#define portRESTORE_CONTEXT()                                           \
{                                                                       \
extern volatile void * volatile pxCurrentTCB;                           \
extern volatile uint32_t ulCriticalNesting;                    \
                                                                        \
    /* Set the LR to the task stack. */                                 \
    __asm volatile (                                                    \
    "LDR        R0, =pxCurrentTCB                               \n\t"   \
    "LDR        R0, [R0]                                        \n\t"   \
    "LDR        LR, [R0]                                        \n\t"   \
                                                                        \
    /* The critical nesting depth is the first item on the stack. */    \
    /* Load it into the ulCriticalNesting variable. */                  \
    "LDR        R0, =ulCriticalNesting                          \n\t"   \
    "LDMFD  LR!, {R1}                                           \n\t"   \
    "STR    R1, [R0]                                            \n\t"   \
                                                                        \
    /* Get the SPSR from the stack. */                                  \
    "LDMFD  LR!, {R0}                                           \n\t"   \
    "MSR    SPSR, R0                                            \n\t"   \
                                                                        \
    /* Restore all system mode registers for the task. */               \
    "LDMFD  LR, {R0-R14}^                                       \n\t"   \
    "NOP                                                        \n\t"   \
                                                                        \
    /* Restore the return address. */                                   \
    "LDR       LR, [LR, #+60]                                   \n\t"   \
                                                                        \
    /* And return - correcting the offset in the LR to obtain the */    \
    /* correct address. */                                              \
    "SUBS   PC, LR, #4                                          \n\t"   \
    );                                                                  \
    ( void ) ulCriticalNesting;                                         \
    ( void ) pxCurrentTCB;                                              \
}
/*-----------------------------------------------------------*/

#define portSAVE_CONTEXT()                                              \
{                                                                       \
extern volatile void * volatile pxCurrentTCB;                           \
extern volatile uint32_t ulCriticalNesting;                    \
                                                                        \
    /* Push R0 as we are going to use the register. */                  \
    __asm volatile (                                                    \
    "STMDB  SP!, {R0}                                           \n\t"   \
                                                                        \
    /* Set R0 to point to the task stack pointer. */                    \
    "STMDB  SP,{SP}^                                            \n\t"   \
    "NOP                                                        \n\t"   \
    "SUB    SP, SP, #4                                          \n\t"   \
    "LDMIA  SP!,{R0}                                            \n\t"   \
                                                                        \
    /* Push the return address onto the stack. */                       \
    "STMDB  R0!, {LR}                                           \n\t"   \
                                                                        \
    /* Now we have saved LR we can use it instead of R0. */             \
    "MOV    LR, R0                                              \n\t"   \
                                                                        \
    /* Pop R0 so we can save it onto the system mode stack. */          \
    "LDMIA  SP!, {R0}                                           \n\t"   \
                                                                        \
    /* Push all the system mode registers onto the task stack. */       \
    "STMDB  LR,{R0-LR}^                                         \n\t"   \
    "NOP                                                        \n\t"   \
    "SUB    LR, LR, #60                                         \n\t"   \
                                                                        \
    /* Push the SPSR onto the task stack. */                            \
    "MRS    R0, SPSR                                            \n\t"   \
    "STMDB  LR!, {R0}                                           \n\t"   \
                                                                        \
    "LDR    R0, =ulCriticalNesting                              \n\t"   \
    "LDR    R0, [R0]                                            \n\t"   \
    "STMDB  LR!, {R0}                                           \n\t"   \
                                                                        \
    /* Store the new top of stack for the task. */                      \
    "LDR    R0, =pxCurrentTCB                                   \n\t"   \
    "LDR    R0, [R0]                                            \n\t"   \
    "STR    LR, [R0]                                            \n\t"   \
    );                                                                  \
    ( void ) ulCriticalNesting;                                         \
    ( void ) pxCurrentTCB;                                              \
}


#define portSAVE_CONTEXT_ASM                                            \
    /* Push R0 as we are going to use the register. */                  \
    __asm volatile (                                                    \
    "STMDB  SP!, {R0}                                           \n\t"   \
                                                                        \
    /* Set R0 to point to the task stack pointer. */                    \
    "STMDB  SP,{SP}^                                            \n\t"   \
    "NOP                                                        \n\t"   \
    "SUB    SP, SP, #4                                          \n\t"   \
    "LDMIA  SP!,{R0}                                            \n\t"   \
                                                                        \
    /* Push the return address onto the stack. */                       \
    "STMDB  R0!, {LR}                                           \n\t"   \
                                                                        \
    /* Now we have saved LR we can use it instead of R0. */             \
    "MOV    LR, R0                                              \n\t"   \
                                                                        \
    /* Pop R0 so we can save it onto the system mode stack. */          \
    "LDMIA  SP!, {R0}                                           \n\t"   \
                                                                        \
    /* Push all the system mode registers onto the task stack. */       \
    "STMDB  LR,{R0-LR}^                                         \n\t"   \
    "NOP                                                        \n\t"   \
    "SUB    LR, LR, #60                                         \n\t"   \
                                                                        \
    /* Push the SPSR onto the task stack. */                            \
    "MRS    R0, SPSR                                            \n\t"   \
    "STMDB  LR!, {R0}                                           \n\t"   \
                                                                        \
    "LDR    R0, =ulCriticalNesting                              \n\t"   \
    "LDR    R0, [R0]                                            \n\t"   \
    "STMDB  LR!, {R0}                                           \n\t"   \
                                                                        \
    /* Store the new top of stack for the task. */                      \
    "LDR    R0, =pxCurrentTCB                                   \n\t"   \
    "LDR    R0, [R0]                                            \n\t"   \
    "STR    LR, [R0]                                            \n\t"   \
    )                                                                  \

#define portRESTORE_CONTEXT_ASM                                         \
    /* Set the LR to the task stack. */                                 \
    __asm volatile (                                                    \
    "LDR        R0, =pxCurrentTCB                               \n\t"   \
    "LDR        R0, [R0]                                        \n\t"   \
    "LDR        LR, [R0]                                        \n\t"   \
                                                                        \
    /* The critical nesting depth is the first item on the stack. */    \
    /* Load it into the ulCriticalNesting variable. */                  \
    "LDR        R0, =ulCriticalNesting                          \n\t"   \
    "LDMFD  LR!, {R1}                                           \n\t"   \
    "STR    R1, [R0]                                            \n\t"   \
                                                                        \
    /* Get the SPSR from the stack. */                                  \
    "LDMFD  LR!, {R0}                                           \n\t"   \
    "MSR    SPSR, R0                                            \n\t"   \
                                                                        \
    /* Restore all system mode registers for the task. */               \
    "LDMFD  LR, {R0-R14}^                                       \n\t"   \
    "NOP                                                        \n\t"   \
                                                                        \
    /* Restore the return address. */                                   \
    "LDR       LR, [LR, #+60]                                   \n\t"   \
                                                                        \
    /* And return - correcting the offset in the LR to obtain the */    \
    /* correct address. */                                              \
    "SUBS   PC, LR, #4                                          \n\t"   \
    )                                                                  \


/*-----------------------------------------------------------*/


extern void vTaskSwitchContext( void );

/* Critical section handling. */
/*
 * The interrupt management utilities can only be called from ARM mode.  When
 * THUMB_INTERWORK is defined the utilities are defined as functions in
 * portISR.c to ensure a switch to ARM mode.  When THUMB_INTERWORK is not
 * defined then the utilities are defined as macros here - as per other ports.
 */

#ifdef THUMB_INTERWORK

	extern void vPortDisableInterruptsFromThumb( void ) __attribute__ ((naked));
	extern void vPortEnableInterruptsFromThumb( void ) __attribute__ ((naked));

	#define portDISABLE_INTERRUPTS()	vPortDisableInterruptsFromThumb()
	#define portENABLE_INTERRUPTS()		vPortEnableInterruptsFromThumb()

#else

	#define portDISABLE_INTERRUPTS()											\
		__asm volatile (															\
			"STMDB	SP!, {R0}		\n\t"	/* Push R0.						*/	\
			"MRS	R0, CPSR		\n\t"	/* Get CPSR.					*/	\
			"ORR	R0, R0, #0xC0	\n\t"	/* Disable IRQ, FIQ.			*/	\
			"MSR	CPSR, R0		\n\t"	/* Write back modified value.	*/	\
			"LDMIA	SP!, {R0}			" )	/* Pop R0.						*/

	#define portENABLE_INTERRUPTS()												\
		__asm volatile (															\
			"STMDB	SP!, {R0}		\n\t"	/* Push R0.						*/	\
			"MRS	R0, CPSR		\n\t"	/* Get CPSR.					*/	\
			"BIC	R0, R0, #0xC0	\n\t"	/* Enable IRQ, FIQ.				*/	\
			"MSR	CPSR, R0		\n\t"	/* Write back modified value.	*/	\
			"LDMIA	SP!, {R0}			" )	/* Pop R0.						*/

#endif /* THUMB_INTERWORK */


extern void vTaskEnterCritical( void );
extern void vTaskExitCritical( void );

#define portENTER_CRITICAL()	vTaskEnterCritical();//	vPortEnterCritical();
#define portEXIT_CRITICAL()		vTaskExitCritical();//	vPortExitCritical();
/*-----------------------------------------------------------*/

/* Task utilities. */
#define portEND_SWITCHING_ISR( xSwitchRequired ) 	\
{													\
extern void vTaskSwitchContext( void );				\
													\
	if( xSwitchRequired ) 							\
	{												\
		vTaskSwitchContext();						\
	}												\
}
/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void * pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void * pvParameters )


#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1

	/* Store/clear the ready priorities in a bit map. */
	#define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
	#define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

	/*-----------------------------------------------------------*/

	#define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31UL - ( uint32_t ) __builtin_clz( uxReadyPriorities ) )

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */




#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */


