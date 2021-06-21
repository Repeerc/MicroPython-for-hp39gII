#include <stdio.h>
#include <string.h>
#include "py/runtime.h"
#include "extmod/vfs.h"

STATIC mp_obj_t machine_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check args.
    printf("NEW FLASH OBJ\n");
    // Return singleton object.
    return MP_OBJ_FROM_PTR(NULL);
}


// readblocks(block_num, buf, [offset])
// read size of buffer number of bytes from block (with offset) into buffer
STATIC mp_obj_t machine_flash_readblocks(size_t n_args, const mp_obj_t *args) {

    printf("FLASH OBJ RD\n");

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_flash_readblocks_obj, 3, 4, machine_flash_readblocks);


STATIC mp_obj_t machine_flash_writeblocks(size_t n_args, const mp_obj_t *args) {

    printf("FLASH OBJ WR\n");

    return MP_OBJ_NEW_SMALL_INT(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_flash_writeblocks_obj, 3, 4, machine_flash_writeblocks);


STATIC mp_obj_t machine_flash_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {

    printf("FLASH OBJ IO\n");

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_flash_ioctl_obj, machine_flash_ioctl);

STATIC const mp_rom_map_elem_t machine_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&machine_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&machine_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&machine_flash_ioctl_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_flash_locals_dict, machine_flash_locals_dict_table);

const mp_obj_type_t machine_flash_type = {
    { &mp_type_type },
    .name = MP_QSTR_Flash,
    .make_new = machine_flash_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_flash_locals_dict,
};
