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

#include <stdint.h>

// Options to control how MicroPython is built
#define MICROPY_GC_STACK_ENTRY_TYPE                 uint32_t
#define MICROPY_HELPER_REPL                         (1)
#define MICROPY_ENABLE_GC                           (1)
#define MICROPY_ENABLE_FINALISER                    (1)
#define MICROPY_REPL_AUTO_INDENT                    (1)


// Memory allocation policy
#define MICROPY_QSTR_BYTES_IN_HASH              (1)

// Compiler configuration
#define MICROPY_COMP_CONST                      (0)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN        (0)

// Python internal features
#define MICROPY_ENABLE_EXTERNAL_IMPORT          (1)
#define MICROPY_ERROR_REPORTING                 (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_LONGINT_IMPL                        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL                          (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_CPYTHON_COMPAT                  (1)
#define MICROPY_MODULE_GETATTR                  (1)
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG   (1)

#define MICROPY_PY_BUILTINS_INPUT                   (1)
#define MICROPY_PY_BUILTINS_HELP                    (1)

// Fine control over Python builtins, classes, modules, etc
#define MICROPY_PY_ASYNC_AWAIT                  (1)
#define MICROPY_PY_ASSIGN_EXPR                  (1)
#define MICROPY_PY_BUILTINS_STR_COUNT           (1)
#define MICROPY_PY_BUILTINS_STR_OP_MODULO       (1)
#define MICROPY_PY_BUILTINS_BYTEARRAY           (1)
#define MICROPY_PY_BUILTINS_DICT_FROMKEYS       (1)
#define MICROPY_PY_BUILTINS_SET                 (1)
#define MICROPY_PY_BUILTINS_SLICE               (1)
#define MICROPY_PY_BUILTINS_PROPERTY            (1)
#define MICROPY_PY_BUILTINS_ENUMERATE           (1)
#define MICROPY_PY_BUILTINS_REVERSED            (1)
#define MICROPY_PY___FILE__                     (0)
#define MICROPY_PY_ARRAY                        (1)
#define MICROPY_PY_COLLECTIONS                  (1)
#define MICROPY_PY_IO                           (0)
#define MICROPY_PY_MATH                         (1)
#define MICROPY_PY_CMATH                         (1)
#define MICROPY_PY_STRUCT                       (1)
#define MICROPY_PY_SYS                          (1)

// Type definitions for the specific machine

typedef int32_t mp_int_t; // must be pointer size
typedef uint32_t mp_uint_t; // must be pointer size
typedef long mp_off_t;
/*
extern const struct _mp_obj_module_t mp_module_uos;

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_ROM_QSTR(MP_QSTR_uos),    MP_ROM_PTR(&mp_module_uos) },      
*/



// Need to provide a declaration/definition of alloca()
#include <alloca.h>

// Define the port's name and hardware.
#define MICROPY_HW_BOARD_NAME "HP39gII"
#define MICROPY_HW_MCU_NAME   "STMP3770"

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8];

