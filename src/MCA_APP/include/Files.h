/*******************************************************************************
 File name  : Files.h
 *******************************************************************************/
/***************************************************************************
 *   Copyright (C) 2012 by LeDoyle R. Pingel, safelyiniowa@yahoo.com       *
 *                                                                         *
 *   This file is part of Gamma Grapher Version 2                          *
 *                                                                         *
 *   Gamma Grapher Version 2 is free software; you can redistribute it     *
 *   and/or modify it under the terms of the GNU General Public License as *
 *	 published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This software is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with OpenGamma. If not, see <http://www.gnu.org/licenses/>.     *
 ***************************************************************************/

#ifndef __FILES_H
#define __FILES_H
//==================== Function declarations ====================
u8 FAT_Info(void);
u8 Open_File(unsigned const char *name, unsigned const char *ext);
u8 SetClusterNext(void);
u8 DirTouch(void);
u8 DirMatch(unsigned const char *p1, unsigned const char *p2, unsigned short n);
u8 Read_File(u8* pBuffer, u32 File_Addr, u16 Sz);
u8 Write_File(u8* pBuffer, u32 File_Addr, u16 Sz);
void Read_Parameter(void);
u8 Write_Parameter(void);

//==================== type declarations ====================
typedef enum {WriteErr, NoFile, SDErr, NoCard, SaveOk, Failed, ReadErr} SD_Status;

// MBR
#define MBR_PART_START		0x1BE		// partition table start
#define MBR_PART_LEN 		16
#define MBR_BOOT_SIG		0x1FE		// Boot signature, 0xFF, 0xAA

// Partition table entries
#define PART_STATUS			0x0			// bit 7
#define PART_CHS			0x1			// first sector in CHS
#define PART_TYPE			0x4			// partition type
#define PART_CHS_LST		0x5			// last sector in CHS
#define PART_LBA			0x8			// LBA address of first sector
#define PART_SZ				0xC			// size in sectors

// BIOS Parameter Block
#define BPB_SEC_SZ			0x0B		// bytes per sector, in 2 bytes
#define BPB_SEC_CLUST		0x0D		// sectors per cluster, in 1 byte
#define BPB_RES_SEC			0x0E		// reserved sectors in 2 bytes
#define BPB_FATS			0x10		// number of fat tables in 1 byte
#define BPB_DIR_ENTS		0x11		// maximum number of directory entries (FAT16)
#define BPB_TOT_SEC16		0x13		// total logical secotrs, may be 0
#define BPB_MEDIA_DESC		0x15		// media descriptor
#define BPB_SEC_FAT_16		0x16		// sectors per FAT (FAT16) in 2 bytes
#define BPB_SEC_TRK			0x18		// sectors per track in 2 bytes
#define BPB_HEADS			0x1A		// number of heads in 2 bytes
#define BPB_HID_SEC			0x1C		// hidden sectors in 4 bytes
#define BPB32_TOT_SEC		0x20		// total sectors (FAT32)
#define BPB32_SEC_FAT		0x24		// sectors per FAT in 4  bytes
#define BPB_DRV_DESC		0x28		// drive description in 2 bytes
#define BPB_VER				0x2A		// version in 2 bytes
#define BPB_ROOT			0x2C		// cluster number of root directory
#define BPB_FS_INFO			0x30		// sector number of FS info

// FAT
#define FAT_FREE 			0x0
#define FAT16_BAD_SECT		0xFFF7
#define FAT32_BAD_SECT		0xFFFFFF7
#define FAT16_EOC			0xFFF8		// and all values above this
#define FAT32_EOC			0x0FFFFFF0	// and all values above this

// Directory Table
#define DIR_LEN				32
#define DIR_FILE_NM			0x0		// 8 space padded characters
	#define DIR_FILE_NM_AVAIL	0x00	// 1st byte, directory entry is available
	#define DIR_FILE_NM_DOT		0x2E	// 1st byte, a dot or dot dot
	#define DIR_FILE_NM_ERASED	0xE5	// entry has been erased and is avilable
#define DIR_EXT				0x08		// 3 char extension, space padded
#define DIR_ATTR			0x0B	// file attributes, true if bit set, in 1 byte
	#define DIR_ATTR_MSK_RO		0x01	// read only
	#define DIR_ATTR_MSK_HID	0x02	// hidden
	#define DIR_ATTR_MSK_SYS	0x04	// system file
	#define DIR_ATTR_MSK_LABEL	0x08	// volume label
	#define DIR_ATTR_MSK_SUB	0x10	// sub directory
	#define DIR_ATTR_MSK_ARCH	0x20	// needs to be archived
	#define DIR_ATTR_MSK_DEV	0x40	// device
	#define DIR_ATTR_MSK_RES	0x80	// reserved
#define DIR_ATTR_USER		0x0C
#define DIR_FRST_CHAR		0x0D
#define DIR_TIME			0x0E	// creation time 2 bytes
	#define DIR_TIME_HRS		0xf800	// hour
	#define DIR_TIME_MIN		0x07e0	// minute
	#define DIR_TIME_SEC		0x001f	// seconds
#define DIR_DATE			0x10		// creation date 2 bytes
	#define DIR_DATE_YEAR		0xFE00	// year
	#define DIR_DATE_MONTH		0x01E0	// month
	#define DIR_DATE_DAY		0x001f	// day
#define DIR_OWNER			0x12	// owner info 2 bytes
#define DIR16_PERM			0x14	// permissions in 2 bytes
#define DIR32_START_HI		0x14	// high 2 bytes starting cluster
#define DIR_MOD_TIME		0x16	// bits same as DIR_TIME
#define DIR_MOD_DATE		0x18	// bits same as DIR_DATE
#define DIR16_STRT			0x1A	// starting cluster in 2 bytes
#define DIR32_STRT_LO		0x1A	// lower 2 bytes of starting cluster
#define DIR_FILE_SZ			0x1C	// file size in bytes, 4 bytes
#endif
/********************************* END OF FILE ********************************/
