

#include "tusb.h"

#include "usb_cdc.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#define USBD_STACK_SIZE (3 * configMINIMAL_STACK_SIZE)

StackType_t usb_device_stack[USBD_STACK_SIZE];
StaticTask_t usb_device_taskdef;


void vUsbDeviceTask() {
    tusb_init();
    for (;;) {
        tud_task();
    }
}

void usb_task_init(){
    xTaskCreateStatic(vUsbDeviceTask, "usbd", USBD_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, usb_device_stack, &usb_device_taskdef);
    xTaskCreate(usb_cdc_task, "USB CDC", configMINIMAL_STACK_SIZE, NULL, 4, NULL);

}


