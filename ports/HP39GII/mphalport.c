#include <stdio.h>

#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "map.h"

extern struct dhara_map *flash_map_obj;

int mp_vfs_blockdev_read(mp_vfs_blockdev_t *self, size_t block_num, size_t num_blocks, uint8_t *buf){
    printf("VFS RD:%d, bks:%d\n",block_num, num_blocks);
    dhara_error_t err;

    for(int i=0; i<num_blocks; i++){
        dhara_map_read(flash_map_obj, block_num + i, buf, &err);
        if(err != 0){
            return 1;
        }
    }

    return 0;
}


int mp_vfs_blockdev_write(mp_vfs_blockdev_t *self, size_t block_num, size_t num_blocks, const uint8_t *buf){
    
    dhara_error_t err;

    //dhara_map_write(flash_map_obj, block_num, )
    for(int i=0; i<num_blocks; i++){
        dhara_map_write(flash_map_obj, block_num + i, buf + 2048 * i, &err);
        if(err != 0){
            return 1;
        }
    }
    
    
    //printf("VFS WR:%d, bks:%d\n",block_num, num_blocks);
    return 0;
}

mp_obj_t mp_vfs_blockdev_ioctl(mp_vfs_blockdev_t *self, uintptr_t cmd, uintptr_t arg){
    //printf("VFS IOCTL:%d, arg:%d\n",cmd, arg);
    if(flash_map_obj == NULL){
        return NULL;
    }
    dhara_error_t err;

    switch (cmd)
    {
    case MP_BLOCKDEV_IOCTL_SYNC:
        dhara_map_sync(flash_map_obj, &err);
        break;
    case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
        return(mp_obj_new_int(2048));
        break;
    case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
        return(mp_obj_new_int(dhara_map_capacity(flash_map_obj)));
        break;
    case MP_BLOCKDEV_IOCTL_INIT:
        if(flash_map_obj == NULL){
            return MP_OBJ_NEW_SMALL_INT(STA_NOINIT);
        }else{
            self->writeblocks[0] = mp_obj_new_int(dhara_map_capacity(flash_map_obj));
            return MP_OBJ_NEW_SMALL_INT(RES_OK);
        }

        break;
    default:
        return MP_OBJ_NEW_SMALL_INT(RES_PARERR);
        break;
    }

    return NULL;
}

void mp_vfs_blockdev_init(mp_vfs_blockdev_t *self, mp_obj_t bdev){
    
    printf("BDEV INIT\n");

}