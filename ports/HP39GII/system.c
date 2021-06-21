/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include "regsuartdbg.h"
#include "regsapbh.h"
#include "regsuartdbg.h"
#include "regsclkctrl.h"
#include "regspower.h"
#include "regsdigctl.h"


#include "mmu.h"
#include "exception.h"
#include "irq.h"
#include "display.h"
#include "keyboard.h"
#include "clkgen.h"
#include "uart_debug.h"
#include "raw_flash.h"
#include "flash.h"

#include "FreeRTOSConfig.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "portable.h"

#include "usb_task.h"


extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss, _heap_start, _heap_stop;

void REPL_main(void);

static void stmp3770_init(void);
void vPortDefineHeapRegions( const HeapRegion_t * const pxHeapRegions );

extern unsigned int SVC_STACK_ADDR;


static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
static StaticTask_t IdleTaskTCB;
static StackType_t TimerTaskStack[configMINIMAL_STACK_SIZE];
static StaticTask_t TimerTaskTCB;
//空闲任务所需内存
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, 
                                StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
		*ppxIdleTaskTCBBuffer=&IdleTaskTCB;
		*ppxIdleTaskStackBuffer=IdleTaskStack; 
		*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;
}
//定时器任务所需内存
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, 
StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
		*ppxTimerTaskTCBBuffer=&TimerTaskTCB;
		*ppxTimerTaskStackBuffer=TimerTaskStack; 
		*pulTimerTaskStackSize=configMINIMAL_STACK_SIZE;
}


void printTaskList() {
    struct mallinfo mallocInfo;
    
    char *tasklist_buf = pvPortMalloc(2048);

    memset(tasklist_buf, 0, 2048);

    vTaskList(tasklist_buf);
    printf("=======================================================\n");
    printf("任务名                 任务状态   优先级   剩余栈   任务序号\n");
    printf("%s\n", tasklist_buf);
    printf("任务名                运行计数           CPU使用率\n");
    vTaskGetRunTimeStats(tasklist_buf);
    printf("%s", tasklist_buf);
    printf("任务状态:  X-运行  R-就绪  B-阻塞  S-挂起  D-删除\r\n\r\n");

    mallocInfo = mallinfo();
    printf("total space allocated from system: %d\r\n", mallocInfo.arena);
    printf("number of non-inuse chunks: %d\r\n", mallocInfo.ordblks);
    printf("number of mmapped regions: %d\r\n", mallocInfo.hblks);
    printf("total space in mmapped regions: %d\r\n", mallocInfo.hblkhd);
    printf("total allocated space: %d\r\n", mallocInfo.uordblks);
    printf("total non-inuse space: %d\r\n", mallocInfo.fordblks);
    printf("top-most, releasable (via malloc_trim) space: %d\r\n", mallocInfo.keepcost);
    printf("剩余内存：%ld Bytes\r\n",((uint32_t)&_heap_stop - (uint32_t)&_heap_start) - (uint32_t)mallocInfo.uordblks);
    printf("熵：%d\n",HW_DIGCTL_ENTROPY_RD());
    vPortFree(tasklist_buf);
}

void vTaskMPythonREPL(void *pvParameters) {
    
    REPL_main();
    
    vTaskDelete(NULL);
    for (;;) {
    }
}

void vTask2(void *pvParameters) {

    for (;;) {
        vTaskDelay(12000);
        printTaskList();
       
    }

}

void vBootstrap_task(void *pvParameters){



    vTaskDelay(50);
    NAND_init();
    vTaskDelay(50);
    flash_init();
    vTaskDelay(500);
    
    usb_task_init();
    
    xTaskCreate(vTaskMPythonREPL, "MPythonREPL", configMINIMAL_STACK_SIZE*4, NULL, 3, NULL);
    
    xTaskCreate(vTask2, "Dump", configMINIMAL_STACK_SIZE, NULL, 3, NULL);

/*
    while(1){
        vTaskDelay(1000);
    }*/
    vTaskDelete(NULL);
}

void boot(){
	
    set_stack(&SVC_STACK_ADDR);
    stack_init();
    switch_mode(SVC_MODE);
    // Zero out .bss section.
    memset(&_sbss, 0, (char *)&_ebss - (char *)&_sbss);


    // Copy .data section from flash to RAM.
    //memcpy(&_sdata, &_sidata, (char *)&_edata - (char *)&_sdata);

    // Initialise the cpu and peripherals.
    stmp3770_init();
    LCD_clear_buffer();
    LCD_dma_flush_buffer();

   

    // Now that there is a basic system up and running, call the main application code.

    xTaskCreate(vBootstrap_task, "BootStrap", configMINIMAL_STACK_SIZE * 2, NULL, 3, NULL);


    vTaskStartScheduler();

        

    uartdbg_printf("HALT\r\n");
    // This function must not return.
    for (;;) {
    }
	
}




// Set up the STMP3770 MCU.
static void stmp3770_init(void) {


    exception_init();
    irq_init();  

    BF_SETV(POWER_VDDDCTRL,TRG,26); // Set core voltage = 1.45 V

    PLL_enable(1);
    
    
    HCLK_set_div(0, 4);   //120 MHz
    CPUCLK_set_div(0, 1); //480 MHz
    CPUCLK_set_gating(0);
    CPUCLK_set_bypass(0);


	BF_CS1(CLKCTRL_HBUS, SLOW_DIV, 1);	
	BF_CS1(CLKCTRL_HBUS, APBHDMA_AS_ENABLE, 1);
	BF_CS1(CLKCTRL_HBUS, APBXDMA_AS_ENABLE, 1);

	BF_CS1(CLKCTRL_HBUS, TRAFFIC_JAM_AS_ENABLE, 1);
	BF_CS1(CLKCTRL_HBUS, TRAFFIC_AS_ENABLE, 1);
    BF_CS1(CLKCTRL_HBUS, AUTO_SLOW_MODE, 1);


    BF_CLR(APBH_CTRL0,SFTRST);
    BF_CLR(APBH_CTRL0,CLKGATE);
    BF_SET(APBH_CTRL0,SFTRST);
    while(!BF_RD(APBH_CTRL0,CLKGATE));
    BF_CLR(APBH_CTRL0,SFTRST);
    BF_CLR(APBH_CTRL0,CLKGATE);    
    
    BF_CS1(CLKCTRL_FRAC, CLKGATEIO, 0);     
    BF_CLR(CLKCTRL_CLKSEQ, BYPASS_GPMI);    //Enable GPMI (NAND FLASH) Clock, Select ref_io path to generate the GPMI clock domain(PLL)

    BF_CS1(CLKCTRL_FRAC, CLKGATEPIX, 0);
    BF_CLR(CLKCTRL_CLKSEQ, BYPASS_PIX);     //Enable display Clock

    BF_SET(CLKCTRL_PIX, CLKGATE);
    BF_CS1(CLKCTRL_PIX, DIV, 20); //480 / 20 = 24MHz

    BF_CLR(CLKCTRL_PIX, CLKGATE);
    //BF_CS2(APBH_CTRL0, SFTRST, 0, CLKGATE, 0); //Enalbe APBH DMA


    enable_interrupts();

    LCD_init();
    

}


// Write a character out to the UART.
static inline void uart_write_char(int c) {
   // int loop = 0;
    while (HW_UARTDBGFR_RD() & BM_UARTDBGFR_TXFF) {
        /*loop++;
        if (loop > 100000)
            return;*/
    }

    if (!(HW_UARTDBGFR_RD() & BM_UARTDBGFR_TXFF))
        HW_UARTDBGDR_WR(c);
}

_ssize_t _write_r (struct _reent *r, int fd, const void *buf, size_t nbytes){
    vTaskSuspendAll();
    int i = 0;
    char *ptr = (char *)buf;

    while (*ptr && (i < nbytes)) {
            uartdbg_putc(*ptr);
            i++;
            ptr++;
        }

    xTaskResumeAll();

    return i;
}

/*
int _write_r(int *r0, const void *r1, size_t r2){
    char *ptr = (char *)r1;

    while((r2 > 0) && *ptr){
        uart_write_char(*ptr++);
        r2--;
    }
	return 0;
}*/
/*
// Send string of given length to stdout, converting \n to \r\n.
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    while (len--) {
        if (*str == '\n') {
            uart_write_char('\r');
        }
        uart_write_char(*str++);
    }
}*/

