#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "tusb.h"

#include "raw_flash.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "flash.h"
#include "map.h"
//#include "lfs1.h"

NandConfigBlockInfo_t *p_NandConfigBlock;
NandConfigBlockInfo_t SysNandConfigBlock;
NandConfigBlockRegionInfo_t Regions[7];
unsigned int nRegion = 0;
unsigned int LDLBBlockPos = 0;
unsigned int data_start_block = 0;
bool _isRawFlash = true;



SemaphoreHandle_t flashLock;


extern volatile unsigned int dmaOperationCompleted;
extern volatile unsigned int eccOperationCompleted;
extern unsigned char address_page_data[5];
unsigned char RAWreadBackMetaData[200] __attribute__((aligned(0x200)));

#define FLASH_TIMEOUT_MS    1000

int readFlashPage(uint32_t page, void *buffer)
{

    if (xSemaphoreTake(flashLock, FLASH_TIMEOUT_MS) == pdFALSE) {
        return -1;
    }
    
    while (dmaOperationCompleted == 0);
    set_page_address_data(page);
    GPMI_read_block_with_ecc8(NAND_CMD_READ0, NAND_CMD_READSTART, address_page_data, buffer, (unsigned int *)RAWreadBackMetaData, 4);
    while (!dmaOperationCompleted || !eccOperationCompleted);

    xSemaphoreGive(flashLock);
    
    return 0;
}

int eraseFlashBlock(uint32_t block)
{

    if (xSemaphoreTake(flashLock, FLASH_TIMEOUT_MS) == pdFALSE) {
        return -1;
    }

    GPMI_erase_block_cmd(NAND_CMD_ERASE1, NAND_CMD_ERASE2, NAND_CMD_STATUS, block);
    while (!dmaOperationCompleted);

    xSemaphoreGive(flashLock);
    return 0;
}

unsigned char NMETA[19] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

int programFlashPage(uint32_t page, void *data, void *meta)
{
    if (xSemaphoreTake(flashLock, FLASH_TIMEOUT_MS) == pdFALSE) {
        return -1;
    }

    GPMI_write_block_with_ecc8(NAND_CMD_SEQIN, NAND_CMD_PAGEPROG, NAND_CMD_STATUS,
                                   page, data, meta);
    while (!dmaOperationCompleted);
    eccOperationCompleted = 1;

    xSemaphoreGive(flashLock);
    return 0;
}


void vScanAndBuildRegionInfo() {
    unsigned int block;
    unsigned int *buffer;
    int status = 0;
    nRegion = 0;
    buffer = pvPortMalloc(2048);
    //printf("buffer %08x\n",(unsigned int)buffer);
    for (block = 0; block < 64; block++) {

       // status = xReadFlashPages(block * 64 + 1, 1, buffer, 5000);
        status = readFlashPage(block * 64 + 1, buffer);
        if(status != 0){
            printf("Disk ERROR\r\n");
            return;
        }

        p_NandConfigBlock = (NandConfigBlockInfo_t *)buffer;

        //flush_cache();
        if ((p_NandConfigBlock->iMagicCookie == NAND_CONFIG_BLOCK_MAGIC_COOKIE) &&

            (p_NandConfigBlock->iVersionNum == NAND_CONFIG_BLOCK_VERSION)) {

            memcpy(&SysNandConfigBlock, p_NandConfigBlock, sizeof(SysNandConfigBlock));
            _isRawFlash = false;
            printf("Found %d STMP Region(s).\n", SysNandConfigBlock.iNumRegions);

            for (int i = 0; i < SysNandConfigBlock.iNumRegions; i++) {
                Regions[i].eDriveType = SysNandConfigBlock.Regions[i].eDriveType;
                Regions[i].wTag = SysNandConfigBlock.Regions[i].wTag;
                Regions[i].iNumBlks = SysNandConfigBlock.Regions[i].iNumBlks;
                Regions[i].iChip = SysNandConfigBlock.Regions[i].iChip;
                Regions[i].iStartBlock = SysNandConfigBlock.Regions[i].iStartBlock;
                nRegion++;
                printf("%d: DriveType: %d, Block Size: %d, Start Block: %d, Tag: %08lX\n", i,
                       SysNandConfigBlock.Regions[i].eDriveType,
                       SysNandConfigBlock.Regions[i].iNumBlks,
                       SysNandConfigBlock.Regions[i].iStartBlock,
                       SysNandConfigBlock.Regions[i].wTag

                );
            }
            LDLBBlockPos = block;
            printf("LDLBBlockPos=%d\n", LDLBBlockPos);
            break;
        }
    }
    
    vPortFree(buffer);
}

int getDataRegonTotalBlocks() {

    if (_isRawFlash) {
        return -1;
    }
    for (int i = 0; i < nRegion; i++) {
        if (Regions[i].eDriveType == kDriveTypeData) {
            return Regions[i].iNumBlks;
        }
    }
    return -1;
}

int getDataRegonStartBlock() {
    if (_isRawFlash) {
        return -1;
    }
    for (int i = 0; i < nRegion; i++) {
        if (Regions[i].eDriveType == kDriveTypeData) {
            return Regions[i].iStartBlock;
        }
    }
    return -1;
}

 

//#define MAPDEBUG

int dhara_nand_is_bad(const struct dhara_nand *n, dhara_block_t b) {
    #ifdef MAPDEBUG
        printf("CHK BAD BLOCK:%ld\n",b);
    #endif
    return 0;
}

void dhara_nand_mark_bad(const struct dhara_nand *n, dhara_block_t b) {
    #ifdef MAPDEBUG
        printf("MARK BAD BLOCK:%ld\n",b);
    #endif

}


int dhara_nand_erase(const struct dhara_nand *n, dhara_block_t b,
                     dhara_error_t *err)
{

    #ifdef MAPDEBUG
        printf("ERASE BLOCK:%ld\n",b);
    #endif

    eraseFlashBlock(data_start_block + b);
    return 0;
}

int dhara_nand_prog(const struct dhara_nand *n, dhara_page_t p,
                    const uint8_t *data,
                    dhara_error_t *err) 
{

    #ifdef MAPDEBUG
        printf("PROG PAGE:%ld\n",p);
    #endif

    programFlashPage(data_start_block * 64 + p, (void *)data, NMETA);

    return 0;
}

char testPageBuff[2048]  __attribute__((aligned(0x4)));

extern unsigned char ecc_res[4];
int dhara_nand_is_free(const struct dhara_nand *n, dhara_page_t p) {

    #ifdef MAPDEBUG
        printf("TST PAGE EMPTY:%ld\n",p);
    #endif

    readFlashPage(data_start_block * 64 + p, (void *)testPageBuff);
    if (
        (ecc_res[0] == 0xF) &&
        (ecc_res[1] == 0xF) &&
        (ecc_res[2] == 0xF) &&
        (ecc_res[3] == 0xF)) 
    {
            return 1;
    }
    return 0;
}

int dhara_nand_read(const struct dhara_nand *n, dhara_page_t p,
                    size_t offset, size_t length,
                    uint8_t *data,
                    dhara_error_t *err)
{

    #ifdef MAPDEBUG
        printf("PAGE READ:%ld,offset:%d, length:%d\n",p,offset,length);
    #endif
    readFlashPage(data_start_block * 64 + p, testPageBuff);
    memcpy(data, testPageBuff + offset, length);

    #ifdef MAPDEBUG
        //printf("READFIN\n");
    #endif

    return 0;
}

int dhara_nand_copy(const struct dhara_nand *n,
                    dhara_page_t src, dhara_page_t dst,
                    dhara_error_t *err)
{

    #ifdef MAPDEBUG
        printf("PAGE CPY:%ld to %ld\n",src,dst);
    #endif

    readFlashPage(data_start_block * 64 + src, testPageBuff);

    programFlashPage(data_start_block * 64 + dst, testPageBuff, NMETA);

    return 0;
}

struct dhara_map flash_map;
struct dhara_nand nandinf;

struct dhara_map *flash_map_obj = NULL;

char dharaPageBuff[2048]  __attribute__((aligned(0x4)));

#define GC_RATIO    8

void erase_all_data_block(){

    for(int i = data_start_block; i < (data_start_block + getDataRegonTotalBlocks() - 1); i++){
        eraseFlashBlock(i);
        if(i % 100 == 0){
            printf("Erase Block:%d\n",i);
        }
    }

}

void flash_init()
{
    flashLock = xSemaphoreCreateMutex();
    vScanAndBuildRegionInfo();
    if(_isRawFlash) {
        printf("DISK ERROR\n");
        return;
    }

    //lfs_cfg.block_count = getDataRegonTotalBlocks();
    data_start_block = getDataRegonStartBlock();

    dhara_error_t err = 0;
    nandinf.log2_page_size = 11; //2^11 = 2048
    nandinf.log2_ppb = 6;        //2^6 = 64
    nandinf.num_blocks = getDataRegonTotalBlocks();


    dhara_map_init(&flash_map, &nandinf, (uint8_t *)dharaPageBuff, GC_RATIO);
    int status = dhara_map_resume(&flash_map, &err);
    printf("resume: %d, err:%d \n", status, err);
    printf("  flash capacity: %ld KiB\n", dhara_map_capacity(&flash_map)*2048 / 1024);
    status = dhara_map_sync(&flash_map, &err);
    flash_map_obj = &flash_map;

}


