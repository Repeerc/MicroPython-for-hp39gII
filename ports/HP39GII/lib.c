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

#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "FreeRTOS.h"
#include "task.h"

#include "uart_debug.h"

#include "regsrtc.h"

extern uint32_t _heap_start, _heap_stop;
uint8_t *currentHeapEnd = (uint8_t *)&_heap_start;

void * _sbrk_r(struct _reent *pReent, int incr) {

    vTaskSuspendAll(); 
    if ( (uint32_t)currentHeapEnd + incr > (uint32_t)&_heap_stop ){
        pReent->_errno = ENOMEM;
        xTaskResumeAll(); 
        return NULL;
    }
    uint8_t *previousHeapEnd = currentHeapEnd;
    currentHeapEnd += incr;

    xTaskResumeAll(); 
    return previousHeapEnd;
}

_off_t _lseek(int file, _off_t offset, int whence) {
	return -1;
}

int _close(int fd) {
	return -1;
}

_ssize_t _read(int fd, void *ptr, size_t len) {
    return -1;
}

int _kill (int pid, int sig){
	return -1;
}

int _getpid(void){
	return -1;
}

int _fstat(int file, struct stat *st) {
	return -1;
}

int _isatty(int file) {
    return -1;
}

void _exit(int return_code){

	while(1);
}

int _open(const char *file, int flags, int mode)
{
	return -1;
}

int gettimeofday(struct timeval*tv, struct timezone *tz){
    tv->tv_sec = HW_RTC_SECONDS_RD();
    tv->tv_usec = HW_RTC_MILLISECONDS_RD();
    
    return 0;

}

