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

#include "mphalport.h"

// Options to control how MicroPython is built
#define MICROPY_GC_STACK_ENTRY_TYPE                 uint32_t
#define MICROPY_HELPER_REPL                         (1)
#define MICROPY_ENABLE_GC                           (1)
#define MICROPY_ENABLE_FINALISER                    (1)
#define MICROPY_REPL_AUTO_INDENT                    (1)

#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_A)

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



#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)

#define MICROPY_PY_ALL_SPECIAL_METHODS (1)
#define MICROPY_PY_REVERSE_SPECIAL_METHODS (1)
#define MICROPY_PY_BUILTINS_COMPILE (MICROPY_ENABLE_COMPILER)
#define MICROPY_PY_BUILTINS_EXECFILE (MICROPY_ENABLE_COMPILER)

#define MICROPY_PY_BUILTINS_INPUT                   (1)
#define MICROPY_PY_BUILTINS_HELP                    (1)
#define MICROPY_PY_BUILTINS_POW3    (1)

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
#define MICROPY_PY___FILE__                     (1)
#define MICROPY_PY_ARRAY                        (1)
#define MICROPY_PY_COLLECTIONS                  (1)
#define MICROPY_PY_IO                           (1)
#define MICROPY_PY_IO_IOBASE                (1)
#define MICROPY_PY_IO_BYTESIO               (1)
#define MICROPY_PY_IO_FILEIO        (MICROPY_VFS_FAT)
#define MICROPY_PY_MATH                         (1)
//#define MICROPY_PY_CMATH                         (1)
#define MICROPY_PY_STRUCT                       (1)
#define MICROPY_PY_SYS                          (1)
//#define MICROPY_PY_SYS_STDFILES     (1)

#define MICROPY_PY_BUILTINS_HELP_MODULES (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN (1)
#define MICROPY_PY_COLLECTIONS_DEQUE (1)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT (1)
#define MICROPY_PY_MATH_SPECIAL_FUNCTIONS (0)
#define MICROPY_PY_MATH_ISCLOSE     (1)
#define MICROPY_PY_MATH_FACTORIAL   (1)


#ifndef MICROPY_PY_UASYNCIO
#define MICROPY_PY_UASYNCIO         (1)
#endif
#ifndef MICROPY_PY_UCTYPES
#define MICROPY_PY_UCTYPES          (1)
#endif
#ifndef MICROPY_PY_UHEAPQ
#define MICROPY_PY_UHEAPQ           (1)
#endif
/*
#ifndef MICROPY_PY_UHASHLIB
#define MICROPY_PY_UHASHLIB         (1)
#endif*/
#ifndef MICROPY_PY_URANDOM
#define MICROPY_PY_URANDOM          (1)
#define MICROPY_PY_URANDOM_SEED_INIT_FUNC (get_random_seed())
#endif
#ifndef MICROPY_PY_URANDOM_EXTRA_FUNCS
#define MICROPY_PY_URANDOM_EXTRA_FUNCS (1)
#endif


#define MICROPY_PY_UOS              (1)

#define MICROPY_PY_MACHINE                       (1)

/*

#define MICROPY_VFS_FAT                         (1)
#define MICROPY_VFS                         (1)
*/


#define MICROPY_VFS                 (1)
#define MICROPY_READER_VFS          (MICROPY_VFS)
#define MICROPY_VFS_FAT             (1)

// fatfs configuration used in ffconf.h
#define MICROPY_FATFS_ENABLE_LFN       (1)
#define MICROPY_FATFS_LFN_CODE_PAGE    437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_USE_LABEL        (1)
#define MICROPY_FATFS_RPATH            (2)
#define MICROPY_FATFS_MULTI_PARTITION  (1)


// Type definitions for the specific machine

typedef int32_t mp_int_t; // must be pointer size
typedef uint32_t mp_uint_t; // must be pointer size
typedef long mp_off_t;
/*
extern const struct _mp_obj_module_t mp_module_uos;

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_ROM_QSTR(MP_QSTR_uos),    MP_ROM_PTR(&mp_module_uos) },      
*/



#define mp_type_fileio                      mp_type_vfs_fat_fileio
#define mp_type_textio                      mp_type_vfs_fat_textio

 #define mp_import_stat mp_vfs_import_stat
 #define mp_builtin_open mp_vfs_fat_open
 #define mp_builtin_open_obj mp_vfs_open_obj

 #define MICROPY_PORT_BUILTINS \
     { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) }, \



// Need to provide a declaration/definition of alloca()
#include <alloca.h>

// Define the port's name and hardware.
#define MICROPY_HW_BOARD_NAME "HP39gII"
#define MICROPY_HW_MCU_NAME   "STMP3770"

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8];

