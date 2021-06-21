#ifndef INCLUDED_MPHALPORT_H
#define INCLUDED_MPHALPORT_H

#include "mpconfigport.h"
#include "regsdigctl.h"

static inline void mp_hal_set_interrupt_char(char c){
    
}

static inline mp_uint_t mp_hal_ticks_ms(void) {
    return HW_DIGCTL_MICROSECONDS_RD()/1000;
}



int32_t inline get_random_seed(){
    return HW_DIGCTL_ENTROPY_RD();
}

#endif
