
#include "uart_debug.h"

#include "tusb.h"

#include "usb_cdc.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

extern QueueHandle_t REPL_rx_buf_queue;

extern QueueHandle_t REPL_tx_buf_queue;

void usb_cdc_repl_tx_task(){
    char ch;

    for(;;){
        if(tud_cdc_connected()){
            if(REPL_tx_buf_queue != NULL){
                xQueueReceive(REPL_tx_buf_queue, &ch, portMAX_DELAY);
                while (!tud_cdc_write_available()) {
                    vTaskDelay(1);
                }
                    tud_cdc_write_char(ch);
                    tud_cdc_write_flush();
            }
        }else{
            vTaskDelay(1000);
        }
    }
}

void usb_cdc_task(){

    char ch;
    xTaskCreate(usb_cdc_repl_tx_task, "REPL CDC TX", configMINIMAL_STACK_SIZE, NULL, 4, NULL);

    for (;;) {

        if (tud_cdc_available()) {
            ch = tud_cdc_read_char();
            if(REPL_rx_buf_queue != NULL){
                xQueueSend(REPL_rx_buf_queue,&ch,0);
            }
        }
        vTaskDelay(1);
        //taskYIELD();
    }

}