#include "ramdisk.h"

#include "ff.h"
#include "diskio.h"

#include <string.h>

static volatile
DSTATUS Stat[FF_RAM_DISKS] = {STA_NOINIT, STA_NOINIT};

uint8_t ramdisk[FF_RAM_DISKS][FF_RAM_SECTOR_SIZE * FF_RAM_SECTORS];

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive number (0) */
)
{
	Stat[drv] &= ~STA_NOINIT;	/* Clear STA_NOINIT flag */
	return Stat[drv];
}



/*-----------------------------------------------------------------------*/
/* Get disk status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive number (0) */
)
{
	if (drv >= FF_RAM_DISKS) return STA_NOINIT;		/* Supports only drive 0 */
	return Stat[drv];	/* Return disk status */
}



/*-----------------------------------------------------------------------*/
/* Read sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,		/* Physical drive number (0) */
	BYTE *buff,		/* Pointer to the data buffer to store read data */
	LBA_t sector,	/* Start sector number (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	if (drv >= FF_RAM_DISKS || !count) return RES_PARERR;		/* Check parameter */
	if (Stat[drv] & STA_NOINIT) return RES_NOTRDY;	/* Check if drive is ready */

	memcpy(buff, &ramdisk[drv][sector*512], count * 512);

	return RES_OK;
}



#if !FF_FS_READONLY && !FF_FS_NORTC
/* get the current time */
DWORD get_fattime (void)
{
	return 0;
}
#endif

DRESULT disk_write (
	BYTE drv,			/* Physical drive number (0) */
	const BYTE *buff,	/* Ponter to the data to write */
	LBA_t sector,		/* Start sector number (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	if (drv >= FF_RAM_DISKS || !count) return RES_PARERR;		/* Check parameter */
	if (Stat[drv] & STA_NOINIT) return RES_NOTRDY;	/* Check drive status */
	if (Stat[drv] & STA_PROTECT) return RES_WRPRT;	/* Check write protect */

	memcpy(&ramdisk[drv][sector*512], buff, count * 512);

	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous drive controls other than data read/write               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive number (0) */
	BYTE cmd,		/* Control command code */
	void *buff		/* Pointer to the conrtol data */
)
{
	DRESULT res;
	BYTE n, csd[16];
	DWORD *dp, st, ed, csize;


	if (drv >= FF_RAM_DISKS) return RES_PARERR;					/* Check parameter */
	if (Stat[drv] & STA_NOINIT) return RES_NOTRDY;	/* Check if drive is ready */

	res = RES_ERROR;

	switch (cmd) {
	case CTRL_SYNC :		/* Wait for end of internal write process of the drive */
		res = RES_OK;
		break;

	case GET_SECTOR_COUNT :	/* Get drive capacity in unit of sector (DWORD) */
		*(DWORD*)buff = FF_RAM_SECTORS;
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
		*(DWORD*)buff = 1;
		res = RES_OK;
		break;

	case CTRL_TRIM :	/* Erase a block of sectors (used when _USE_ERASE == 1) */
		res = RES_OK;	/* FatFs does not check result of this command */
		break;

	default:
		res = RES_PARERR;
	}

	return res;
}
