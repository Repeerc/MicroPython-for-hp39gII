/*
 * A simple user interface for this project
 *
 * Copyright 2020 Creep_er
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#define asm __asm

#include "portmacro.h"

#include "exception.h"

//#include "memory_map.h"

#include "mmu.h"
#include "uart_debug.h"


//#include "startup_info.h"

#include "FreeRTOS.h"
#include "task.h"


#include <stdio.h>
unsigned int faultAddress;
unsigned int insAddress;
unsigned int FSR;

extern void flush_tlb();


void *swi_jump_table[255];

void __handler_und(void) __attribute__((naked));
void __handler_und() {
    asm volatile("mov r0,lr");
    asm volatile("str r0,%0"
                 : "=m"(insAddress)); //取出异常指令的地址

    uartdbg_printf("undefined abortion at: %x\n", insAddress);

    unsigned int *addr = (((unsigned int *)insAddress) - 2);
    for(int i = 0; i< 16; i++){
        uartdbg_printf("%x :  %x\n",addr,*addr);
        addr++;
    }
    switch_mode(SVC_MODE);
    uartdbg_print_regs();
    switch_mode(SYS_MODE);
    uartdbg_print_regs();

    //dump_vm_spaces();
    while (1)
        ;
}
volatile uint32_t swi_n;
extern volatile uint32_t *pxCurrentTCB;
extern unsigned int fault_count;
void __handler_pabort(void) __attribute__((naked));
void __handler_pabort() {

    //asm volatile("ldr sp,=ABT_STACK_ADDR");
    //asm volatile("ldr sp,=ABT_STACK_ADDR");

    portSAVE_CONTEXT_ASM;
    
    asm volatile("mov r0,lr");
    //asm volatile("sub r0,#4");
    asm volatile("str r0,%0"
                 : "=m"(insAddress)); //取出异常指令的地址


    
    
    
    uartdbg_printf("Bad instruction at:%x\n",insAddress);

    
    vTaskSuspendAll();
/*
       printf("Bad instruction at:%08X\n",insAddress);
       
        printf("<< %s >> killed.\n", pcTaskGetName(NULL));
       */
        vTaskDelete(NULL);

    
    xTaskResumeAll();
    portRESTORE_CONTEXT_ASM;

}
 

unsigned int fault_sp;
void __handler_dabort(void) __attribute__((naked));
void __handler_dabort() {



    asm volatile("sub lr,lr,#4");
    portSAVE_CONTEXT_ASM;


    asm volatile("mov r0,lr");
    asm volatile("str r0,%0"
                 : "=m"(insAddress)); //取出异常指令的地址
    asm volatile("mrc p15, 0, r0, c6, c0, 0");
    asm volatile("str r0,%0"
                 : "=m"(faultAddress));        //取出异常指令访问的地址
    asm volatile("mrc p15, 0, r0, c5, c0, 0"); // D
    asm volatile("str r0,%0"
                 : "=m"(FSR));

    vTaskSuspendAll();
    

 /*
        printf("The instruction at 0x%08X referenced memory at 0x%08X, ", insAddress, faultAddress);
        printf("FSR:%08X\n",FSR);
        printf("The memory could not be read.\n");
        printf("<< %s >> killed.\n", pcTaskGetName(NULL));
*/
        vTaskDelete(NULL);


    xTaskResumeAll();
    portRESTORE_CONTEXT_ASM;
}


 
void __handler_swi(void) __attribute__((naked));
void __handler_swi(void) {
   asm volatile("add lr,lr,#4");
   portSAVE_CONTEXT_ASM;

    __asm__ volatile ("":::"memory");
    
    vTaskSwitchContext();
   
    portRESTORE_CONTEXT_ASM;
}

void install_swi_service(unsigned int swi_num, void *service) {
    swi_jump_table[swi_num] = service;
}

void exception_install(exception_type type, unsigned int *exception_handler_addr) {
    unsigned int *exception_table_base = (unsigned int *)EXCEPTION_VECTOR_TABLE_BASE_ADDR;
    //resule = FFFFFE+(jmp_addr/4)-(offset/4)	现场编译跳转指令 // B xx
    exception_table_base[type] = 0xEA000000 | ((0xFFFFFE + (((unsigned int)(exception_handler_addr))/4)-(((unsigned int)&exception_table_base[type])/4))&0x00FFFFFF);

    //exception_table_base[type] = 0xE59FF018; //ldr pc,[pc,#0x18];
    //exception_table_base[type + (0x20 / 4)] = (unsigned int)exception_handler_addr;

    flush_tlb();
}

extern void __handler_swi_asm(void);
extern unsigned int *tlb_base;

void exception_init() {

    unsigned *exception_handler_addr = (unsigned int *)EXCEPTION_VECTOR_TABLE_BASE_ADDR;
    for (int i = 0; i < 0x1C; i++) {
        exception_handler_addr[i] = 0; //清空异常向量表
    }
    exception_install(EXCEPTION_UND, (unsigned int *)&__handler_und);
    exception_install(EXCEPTION_SWI, (unsigned int *)&__handler_swi);
    exception_install(EXCEPTION_PABORT, (unsigned int *)&__handler_pabort);
    exception_install(EXCEPTION_DABORT, (unsigned int *)&__handler_dabort);
 
    asm volatile("mrc p15, 0, r0, c1, c0, 0");
    asm volatile ("bic r0,r0,#0x2000"); 				//设置使用低端向量表
    //asm volatile("orr r0,r0,#0x2000"); //设置使用高端向量表
    asm volatile("mcr p15, 0, r0, c1, c0, 0");

}
