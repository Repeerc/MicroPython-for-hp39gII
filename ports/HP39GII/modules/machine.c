#include "py/runtime.h"

#include "machine.h"

#include "regspower.h"
#include "regsclkctrl.h"


STATIC mp_obj_t machine_info(void) {
    mp_printf(&mp_plat_print, "info about my port\n");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_info_obj, machine_info);

STATIC mp_obj_t machine_poweroff(void) {
    mp_printf(&mp_plat_print, "System shuting down...\n");
    
    HW_POWER_RESET_SET(BF_POWER_RESET_UNLOCK(0x3E77));

    HW_POWER_RESET_SET(BF_POWER_RESET_PWD(1));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_poweroff_obj, machine_poweroff);

STATIC mp_obj_t machine_reboot(void) {
    mp_printf(&mp_plat_print, "Reboot...\n");

    HW_POWER_RESET_SET(BF_POWER_RESET_UNLOCK(0x3E77));

    HW_CLKCTRL_RESET_WR(BF_CLKCTRL_RESET_CHIP(1));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reboot_obj, machine_reboot);



STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_machine) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&machine_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash), MP_ROM_PTR(&machine_flash_type) },
    { MP_ROM_QSTR(MP_QSTR_poweroff), MP_ROM_PTR(&machine_poweroff_obj) },
    { MP_ROM_QSTR(MP_QSTR_reboot), MP_ROM_PTR(&machine_reboot_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);





const mp_obj_module_t machine_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&machine_module_globals,
};
MP_REGISTER_MODULE(MP_QSTR_machine, machine_module, 1);

