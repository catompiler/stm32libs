#ifndef SDCARD_DISKIO_H
#define SDCARD_DISKIO_H

#include "sdcard.h"
#include "defs/defs.h"

#ifndef _DISKIO_DEFINED
#include "fatfs/diskio.h"
#endif


/**
 * Инициализирует диск.
 * @param sdcard SD-карта.
 * @return Статус.
 */
EXTERN DSTATUS sdcard_disk_initialize(sdcard_t* sdcard);

/**
 * Получает статус диска.
 * @param sdcard SD-карта.
 * @return Статус.
 */
EXTERN DSTATUS sdcard_disk_status(sdcard_t* sdcard);

/**
 * Читает сектор диска.
 * @param sdcard SD-карта.
 * @param buff Буфер.
 * @param sector Сектор.
 * @param count Число секторов.
 * @return Результат.
 */
EXTERN DRESULT sdcard_disk_read(sdcard_t* sdcard, BYTE* buff, DWORD sector, UINT count);

#if _USE_WRITE
/**
 * Записывает сектор диска.
 * @param sdcard SD-карта.
 * @param buff Буфер.
 * @param sector Сектор.
 * @param count Число секторов.
 * @return Результат.
 */
EXTERN DRESULT sdcard_disk_write(sdcard_t* sdcard, const BYTE* buff, DWORD sector, UINT count);

#endif // _USE_WRITE

#if _USE_IOCTL
/**
 * Управляет диском.
 * @param sdcard SD-карта.
 * @param cmd Команда.
 * @param buff Буфер.
 * @return Результат.
 */
EXTERN DRESULT sdcard_disk_ioctl(sdcard_t* sdcard, BYTE cmd, void* buff);
#endif // _USE_IOCTL

#endif // SDCARD_DISKIO_H
