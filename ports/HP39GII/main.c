/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2021 Damien P. George
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
#include <unistd.h>
#include <stdlib.h>

#include "py/compile.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mpconfig.h"
#include "py/stackctrl.h"
#include "py/gc.h"

#include "lib/utils/pyexec.h"
#include "lib/utils/gchelper.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


/*
static const char *demo_single_input =
    "print('hello world!', list(x + 1 for x in range(10)), end='eol\\n')";

static const char *demo_file_input =
    "import micropython\n"
    "\n"
    "print(dir(micropython))\n"
    "\n"
    "for i in range(10):\n"
    "    print('iter {:08}'.format(i))";
*/

/*
static void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // Compile, parse and execute the given string.
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // Uncaught exception: print it out.
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
*/

// Main entry point: initialise the runtime and execute demo strings.

char *heap;

QueueHandle_t REPL_rx_buf_queue;
QueueHandle_t REPL_tx_buf_queue;

void REPL_main(void) {

    REPL_rx_buf_queue = xQueueCreate(1024, sizeof(char));
    REPL_tx_buf_queue = xQueueCreate(1024, sizeof(char));

    heap = malloc(32 * 1024);
    mp_stack_ctrl_init();
    gc_init(heap, heap + 32 * 1024);
    mp_init();
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_path), 0);
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_argv), 0);

    for ( ; ;) {

        if (pyexec_friendly_repl() != 0) {
                break;
            }
    }

    gc_sweep_all();
    mp_deinit();

    //do_str(demo_single_input, MP_PARSE_SINGLE_INPUT);
    //do_str(demo_file_input, MP_PARSE_FILE_INPUT);
    //mp_deinit();
}

// Do a garbage collection cycle.
void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
}

/*
// There is no filesystem so stat'ing returns nothing.
mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}
*/

// There is no filesystem so opening a file raises an exception.
/*
mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}
*/

// Receive single character, blocking until one is available.
int mp_hal_stdin_rx_chr(void) {
    unsigned char c;
    xQueueReceive(REPL_rx_buf_queue, &c, portMAX_DELAY);
    return c;
}

// Send the string of given length.
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    
    while((len > 0) && *str){
        xQueueSend(REPL_tx_buf_queue,str++,0);
        len--;
    }
    
    //xQueueSend(REPL_tx_buf_queue, )
    
    //int r = write(STDOUT_FILENO, str, len);
    //(void)r;
}


// Called if an exception is raised outside all C exception-catching handlers.
void nlr_jump_fail(void *val) {
    for (;;) {
    }
}

#ifndef NDEBUG
// Used when debugging is enabled.
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    for (;;) {
    }
}
#endif
