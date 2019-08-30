/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* Declarations of disk functions */
#include <stm32f4xx.h>
#include "stm324xg_eval_sdio_sd.h"


#define BLOCK_SIZE            512 /* Block Size in Bytes */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
){
	
	return RES_OK; //懒的管了 有空写写 可以加个
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	SD_Error  Status;
	
	/* Supports only single driveFATFS支持多个设备 所以有个设备号drive nmuber当然了我就一个SD卡所以只有零号 */
	if (pdrv){
		return STA_NOINIT;
	}
	
	/*-------------------------- SD Init ----------------------------- */
	Status = SD_Init();
	
	if (Status!=SD_OK ){
		return STA_NOINIT;
	}else{
		return RES_OK;
	}
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
){
	
	SD_Error Status = SD_OK;
	
	if(Status == SD_OK){
		if (count == 1){
			SD_ReadBlock(buff, sector*BLOCK_SIZE, BLOCK_SIZE);
		}else{
			SD_ReadMultiBlocks(buff, sector*BLOCK_SIZE, BLOCK_SIZE, count);
		}
	}
	
	Status = SD_WaitReadOperation();
	while(SD_GetStatus() != SD_TRANSFER_OK);
	
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
){
	
	SD_Error Status = SD_OK;
	
	if(Status == SD_OK){
		if (count == 1){
			SD_WriteBlock((uint8_t *)buff,sector*BLOCK_SIZE, BLOCK_SIZE);
		}else{
			SD_WriteMultiBlocks((uint8_t *)buff, sector*BLOCK_SIZE, BLOCK_SIZE, count);
		}
	}
	
	Status = SD_WaitWriteOperation();
	while(SD_GetStatus() != SD_TRANSFER_OK);
	
	return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
){
	return RES_OK;
}

DWORD get_fattime(void){
	
	return ((2011UL-1980) << 25)	// Year = 2011
	| (3UL << 21)       // Month = Mar
	| (26UL << 16)       // Day = 26
	| (13U << 11)       // Hour = 13
	| (19U << 5)       // Min =19
	| (0U >> 1)       // Sec = 0
	;
} 
