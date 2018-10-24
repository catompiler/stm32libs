#include "sdcard_diskio.h"


DSTATUS sdcard_disk_initialize(sdcard_t* sdcard)
{
    sdcard_init_card(sdcard);

    if(sdcard_initialized(sdcard)) return 0;
    if(sdcard_identified(sdcard)) return STA_NOINIT;

    return STA_NODISK;
}

DSTATUS sdcard_disk_status(sdcard_t* sdcard)
{
    if(sdcard_initialized(sdcard)) return 0;
    if(sdcard_identified(sdcard)) return STA_NOINIT;

    return STA_NODISK;
}

DRESULT sdcard_disk_read(sdcard_t* sdcard, BYTE* buff, DWORD sector, UINT count)
{
    if(!sdcard_initialized(sdcard)) return RES_NOTRDY;

    err_t err = E_NO_ERROR;

    err = sdcard_select(sdcard);
    if(err != E_NO_ERROR) return RES_ERROR;

    err = sdcard_read_sector(sdcard, sector, count, buff, NULL);

    sdcard_deselect(sdcard);
    if(err != E_NO_ERROR) return RES_ERROR;

    return RES_OK;
}

#if _USE_WRITE
DRESULT sdcard_disk_write(sdcard_t* sdcard, const BYTE* buff, DWORD sector, UINT count)
{
    if(!sdcard_initialized(sdcard)) return RES_NOTRDY;

    err_t err = E_NO_ERROR;

    err = sdcard_select(sdcard);
    if(err != E_NO_ERROR) return RES_ERROR;

    err = sdcard_write_sector(sdcard, sector, count, buff, NULL);
    sdcard_deselect(sdcard);
    if(err != E_NO_ERROR) return RES_ERROR;

    return RES_OK;
}

#endif // _USE_WRITE

#if _USE_IOCTL
DRESULT sdcard_disk_ioctl(sdcard_t* sdcard, BYTE cmd, void* buff)
{
    if(!sdcard_initialized(sdcard)) return RES_NOTRDY;

    DRESULT res = RES_ERROR;
    err_t err = E_NO_ERROR;

    uint32_t start_erase_block = 0;
    uint32_t end_erase_block = 0;

    switch(cmd){
    case CTRL_SYNC:
        err = sdcard_select(sdcard);
        if(err != E_NO_ERROR) break;

        err = sdcard_wait_busy(sdcard);
        if(err == E_NO_ERROR) res = RES_OK;

        sdcard_deselect(sdcard);

        break;
    case GET_SECTOR_COUNT:
        *((DWORD*)buff) = sdcard_sectors_count(sdcard);
        res = RES_OK;
        break;
    case GET_SECTOR_SIZE:
        *((DWORD*)buff) = sdcard_sector_size(sdcard);
        res = RES_OK;
        break;
    case GET_BLOCK_SIZE:
        *((DWORD*)buff) = sdcard_erase_block_len(sdcard);
        res = RES_OK;
        break;
    case CTRL_TRIM:
        err = sdcard_select(sdcard);
        if(err != E_NO_ERROR) break;

        start_erase_block = ((DWORD*)buff)[0];
        end_erase_block = ((DWORD*)buff)[1];

        err = sdcard_erase_sector(sdcard, start_erase_block,
                                  end_erase_block - start_erase_block + 1);
        if(err == E_NO_ERROR) res = RES_OK;

        sdcard_deselect(sdcard);

        break;
    default:
        res = RES_PARERR;
    }

    return res;
}
#endif // _USE_IOCTL
