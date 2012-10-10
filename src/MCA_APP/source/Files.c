/*******************************************************************************
File Name: files.c
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
 *   This file contains portions of example code from STMicroelectronics.  *
 *                                                                         *
 *   This software is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with OpenGamma. If not, see <http://www.gnu.org/licenses/>.     *
 ***************************************************************************/


#include "stm32f10x_type.h"
#include "main.h"
#include "Function.h"
#include "Lcd.h"
#include "Files.h"
#include "Menu.h"
#include "stm32f10x_flash.h"
#include "string.h"
#include "HW_V1_Config.h"

#define Page_Address    0x0801F000 //start address of the stm32 flash

// External calls
// WriteAddr and ReadAddr are in blocks, not bytes
u8 __MSD_WriteBlock(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
u8 __MSD_ReadBlock(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
void Write_Config(void);

#if LANGUAGE == EN
const char *SD_Msgs[] = {"Write Err", "No File", "SD Err", "No Card", "Save Ok", "Failed", "Read Err"};
#endif

extern s32 pulse_height[MAX_CHANNELS];	// pha data in counts
extern u16 msgy;
extern configurations *confp;
extern char *hex_conv;

static u32 Root_Addr;		// address of root in bytes (FAT16)
static u32 FDT_Start;		// address of first FAT32 directory block in bytes
static u32 FAT_Addr;		// Address of first fat block relative to start of disk in bytes
static u32 FDT_Cluster;   // cluster number of first directory (root)

static u32 CurFileAddr;		// index into current cluster of file in bytes
static u32 CurFileSz;		// size of file in bytes
static u32 CurDir;		// disk address of current directory entry in bytes
static u32 CurCluster;	// current cluster
static u32 CurBlk;		// block number in bytes reflected in buffer
static u16 SectorSize;	// sector size in bytes
static u16 SecPerClus;	// sectors per cluster
static u16 DirBlkNum;		// used to find byte index into file for 1st cluster
static u8 FAT16;			// set if FAT16, cleared if FAT32
static u16 Cluster_Cnt = 0;	// counts blocks to detect end of cluster state
static u8 F_Buff[512];	// read/write buffer

bool SD_Card = FALSE;		// true if FAT_Info succeeds

/*******************************************************************************
Function Name : FAT_Info
Description : read the basic infomation of the SD card
*******************************************************************************/
u8 FAT_Info(void)
{
	u32 DiskStart;		// in blocks
	u16 RsvdSecCnt;
	u16 FAT_Size;		// FAT size in sectors
	u16 DIR_Size;		// max number directory entries
	u16 i;

	Display_Info("FAT_Info()", 0);
	// read partition table
	if (__MSD_ReadBlock(F_Buff, 0, 512)){
		Display_Info("FAT_Info() read partition failed", 0);
		return 0xFF; // Disk error!
	}
	// find first block in first partition
	i = MBR_PART_START + PART_LBA;
	DiskStart = (F_Buff[i + 3] << 24) + (F_Buff[i + 2] << 16)
		+ (F_Buff[i + 1] << 8) + F_Buff[i];
	DiskStart = DiskStart << 9;	// convert to bytes
	// read the boot sector
	if (__MSD_ReadBlock(F_Buff, DiskStart, 512)){
		Display_Info("FAT_Info() rd boot sect failed", DiskStart);
		return 0xFF; // Disk error!
	}
	SectorSize = (F_Buff[BPB_SEC_SZ + 1] << 8) + F_Buff[BPB_SEC_SZ];
	SecPerClus = F_Buff[BPB_SEC_CLUST];
	RsvdSecCnt = (F_Buff[BPB_RES_SEC + 1] << 8) + F_Buff[BPB_RES_SEC];
	DIR_Size = (F_Buff[BPB_DIR_ENTS + 1] << 8) + F_Buff[BPB_DIR_ENTS];
	if (DIR_Size) {
		FAT16 = 1;  // FAT16
		FAT_Size = (F_Buff[BPB_SEC_FAT_16+ 1] << 8) + F_Buff[BPB_SEC_FAT_16];
	} else {
		FAT16 = 0;  // FAT32
		FAT_Size = (F_Buff[BPB32_SEC_FAT + 3] << 24) +
			(F_Buff[BPB32_SEC_FAT + 2] << 16) +
			(F_Buff[BPB32_SEC_FAT + 1] << 8) +
			F_Buff[BPB32_SEC_FAT];
	}
	FAT_Addr = DiskStart + RsvdSecCnt * SectorSize;	// start of FAT table
	Root_Addr = FAT_Addr + F_Buff[BPB_FATS] * FAT_Size * SectorSize;	// start of data
	if (FAT16) {
		DirBlkNum = DIR_Size / (512 / 32); // 32 bytes per dir entry
		CurDir = Root_Addr;
	} else {
		FDT_Cluster = (F_Buff[BPB_ROOT + 3] << 24) + (F_Buff[BPB_ROOT + 2] << 16)
			+ (F_Buff[BPB_ROOT + 1] << 8) + F_Buff[BPB_ROOT];
		FDT_Start = Root_Addr + (FDT_Cluster - 2) * SecPerClus * SectorSize;
		CurDir = FDT_Start;
	}
	// initialized in this method
	Display_Info("FAT_Info() FAT16", FAT16);			// set if FAT16, cleared if FAT32
	Display_Info("FAT_Info() SectorSize", SectorSize);	// sector size in bytes
	Display_Info("FAT_Info() SecPerClus", SecPerClus);	// sectors per cluster
	Display_Info("FAT_Info() FAT_Addr", FAT_Addr);		// Address of first fat block relative to start of disk in bytes
	Display_Info("FAT_Info() Root_Addr", Root_Addr);		// address of root in bytes (FAT16)
	Display_Info("FAT_Info() DirBlkNum", DirBlkNum);		// used to find byte index into file for 1st cluster
	Display_Info("FAT_Info() FDT_Start", FDT_Start);		// address of first FAT32 directory block in bytes
	Display_Info("FAT_Info() FDT_Cluster", FDT_Cluster);  // cluster number of first directory (root)
	// not set right now
	//Display_Info("FAT_Info() CurDir", CurDir);		// disk address of current directory entry in bytes
	//Display_Info("FAT_Info() CurFileAddr", CurFileAddr);		// index into current cluster of file in bytes
	//Display_Info("FAT_Info() CurFileSz", CurFileSz);		// size of file in bytes
	//Display_Info("FAT_Info() CurCluster", CurCluster);	// current cluster
	if(confp->debug_level > 3)
		while(1){}
	SD_Card = TRUE;
	return 0; // Disk ok!
}

/*******************************************************************************
Function Name : NextClust
Description : Point CurFileAddr to next FAT cluster in current file
*******************************************************************************/
u8 NextClust(void)
{
	u16 i;	// index in block for current fat entry
	u32 addr;	// address of fat entry in bytes
	u32 tmp;

	Display_Info("NextClust()", 0);
	addr = FAT_Addr;
	if (FAT16) {
		addr += CurCluster * 2;
	} else {
		addr += CurCluster * 4;
	}
	i = (addr & 0x1FF);
	addr = (addr & ~(0x1FF));
	if (__MSD_ReadBlock(F_Buff, addr, 512))
		return 0xff;
	if (FAT16) {
		tmp = (F_Buff[i + 1] << 8) + F_Buff[i];
		if (tmp >= 0xfff0)
			return 0xff;
		CurCluster = tmp;	// do not trash existing values on reaching EOC
		CurFileAddr = Root_Addr + DirBlkNum * 512 + (CurCluster - 2) * SecPerClus * SectorSize;
	} else {
		tmp = ((F_Buff[i + 3] & 0x0f) << 24) +
			(F_Buff[i + 2] << 16) +
			(F_Buff[i + 1] << 8) +
			F_Buff[i];
		if (tmp >= 0x0ffffff0)
			return 0xff;
		CurCluster = tmp;	// do not trash existing values on reaching EOC
		CurFileAddr = Root_Addr + (CurCluster - 2) * SecPerClus * SectorSize;
	}
 	Display_Info("NextClust() CurFileAddr", CurFileAddr);
	Display_Info("NextClust() Root_Addr", Root_Addr);
	Display_Info("NextClust() CurCluster", CurCluster);
	return 0;
}

/*******************************************************************************
Function Name : GetClust
Description : Find unused cluster in FAT
*******************************************************************************/
u8 GetClust(void)
{
	u16 i;	// index in block for current fat entry
	u32 addr;	// address of fat entry in bytes
	u32 clust;	// temporary cluster number
	u32 tmp;

	Display_Info("GetClust()", 0);
	clust = 2;		// skip first 2 reserved fat entries
	if (FAT16) {
		addr = FAT_Addr + clust * 2;
	} else {
		addr = FAT_Addr + clust * 4;
	}
	Display_Info("GetClust() FAT_Addr", FAT_Addr);
	Display_Info("GetClust() CurCluster", clust);
	i = (addr & 0x1FF);
	addr = addr & 0xFFFFe00;
	Display_Info("GetClust() addr", addr);
	Display_Info("GetClust() i", i);
	if (__MSD_ReadBlock(F_Buff, addr, 512))
		return 0xff;
	// search in block of fat entries
	while(i < 512)
	{
		if (FAT16) {
			tmp = (F_Buff[i + 1] << 8) + F_Buff[i];
			if (tmp == FAT_FREE) {
				// mark as End Of Chain
				F_Buff[i + 1] = 0xFF;
				F_Buff[i] = 0xF8;
				if (__MSD_WriteBlock(F_Buff, addr, 512))
					return 0xff;
				CurCluster = clust;
				CurFileAddr = Root_Addr + DirBlkNum * 512 + (clust - 2) * SecPerClus * SectorSize;
				break;
			}
			i  += 2;
			clust++;
		} else {   // FAT32
			tmp = ((F_Buff[i + 3] & 0x0f) << 24) +
				(F_Buff[i + 2] << 16) +
				(F_Buff[i + 1] << 8) +
				F_Buff[i];
			if (tmp == FAT_FREE) {
				// mark as End Of Chain
				F_Buff[i + 3] = 0xFF;
				F_Buff[i + 2]  = 0xFF;
				F_Buff[i + 1]  = 0xFF;
				F_Buff[i] = 0xF8;
				if (__MSD_WriteBlock(F_Buff, addr, 512))
					return 0xff;
				CurCluster = clust;
				CurFileAddr = Root_Addr + (clust - 2) * SecPerClus * SectorSize;
				break;
			}
			i  += 4;
			clust++;
		}
	}
	if(i >= 512)
		return 0xff;
	Display_Info("GetClust() CurFileAddr", CurFileAddr);
	Display_Info("GetClust() Root_Addr", Root_Addr);
	Display_Info("GetClust() CurCluster", CurCluster);
	return 0;
}
/*******************************************************************************
Function Name : AddClust
Description : Add another cluster to current file
*******************************************************************************/
unsigned char AddClust()
{
	u32 i;	// index in block for fat entry
	u32 addr;	// byte address of sector from FAT table
	u32 tmp;

	// get address of current FAT entry
	Display_Info("AddClust() FAT_Addr", FAT_Addr);
	if (FAT16) {
		addr = FAT_Addr + CurCluster * 2;
	} else {
		addr = FAT_Addr + CurCluster * 4;
	}
	i = (addr & 0x1FF);
	addr = addr & 0xFFFFe00;
	Display_Info("AddClust() addr", addr);
	Display_Info("AddClust() i", i);
	if(GetClust()) {	// this sets CurCluster and CurFileAddr reflect new cluster
		Display_Info("AddClust() GetClust failed", 0);
		return 0xFF;
	}
	// update old FAT entry to point to new cluster
	if (__MSD_ReadBlock(F_Buff, addr, 512))
			return 0xff;
	if(FAT16){
		F_Buff[i + 1] = (CurCluster >> 8) & 0xff;
		F_Buff[i] = CurCluster & 0xff;
	}
	else
	{
		F_Buff[i+3] = (CurCluster >> 24) & 0xff;
		F_Buff[i+2] = (CurCluster >> 16) & 0xff;
		F_Buff[i+1] = (CurCluster >> 8) & 0xff;
		F_Buff[i] = CurCluster & 0xff;
	}
	if(__MSD_WriteBlock(F_Buff, addr, 512))
		return 0xff;
	// Note that GetClust() marked EOC
	return 0;
}


/*******************************************************************************
Function Name : DirTouch
Description : Update directory timestamp
*******************************************************************************/
u8 DirTouch(void)
{
	Display_Info("DirTouch", 0);
	if (__MSD_ReadBlock(F_Buff, CurDir & ~(0x1FF), 512))
		return 0xff;
	F_Buff[(CurDir & 0x1FF) + DIR_MOD_TIME] += 1;
	return __MSD_WriteBlock(F_Buff, CurDir >> 9, 512);
}

/*******************************************************************************
Function Name : Open
Description : find and open file
Para :  name is filename, ext is extension part of name
*******************************************************************************/
u8 Open(uc8 *name, uc8 *ext)
{
	u16  i, j;
	u32 dirindex;	// index on device for a directory structure
	u16  blkcnt;	// count of blocks read to fine end of a cluster

	Display_Info("Open()", 0);
	if (SD_Card == FALSE){
		__Display_Str(0, 0, RED, BKGND_COLOR, SD_Msgs[NoCard]);
		return (0xFF);
	}
	// FAT info is read when card inserted
	if (FAT16){
		dirindex = Root_Addr;
	} else { // FAT32
		dirindex = FDT_Start;
	}
	for(blkcnt = 0; blkcnt < SecPerClus; blkcnt++){
		Display_Info("Open() dirindex", dirindex);
		if (__MSD_ReadBlock(F_Buff, dirindex & ~(0x1FF), 512)){
			Display_Info("Open() MSD_ReadBlock failed", dirindex);
			return 0xff; // Read Sector fail
		}
		for (i = 0; i < 512; i += 32, dirindex += 32){ // within current block
			if(F_Buff[i + DIR_ATTR] & 0x4E)
				continue; // skip directory, volume label, system,  hidden, read only
			if(strncmp(F_Buff + i, name, 8) == 0 && strncmp(F_Buff + i + 8, ext, 3) == 0)
			{
				CurDir = dirindex;
				CurFileSz = 0;	// reset file size to write from beginning
				//CurFileSz = (F_Buff[i + DIR_FILE_SZ + 3] << 24) +
				//	(F_Buff[i + DIR_FILE_SZ + 2] << 16) +
				//	(F_Buff[i + DIR_FILE_SZ + 1] << 8) +
				//	(F_Buff[i + DIR_FILE_SZ]);
				if (FAT16)
				{
					CurCluster = (F_Buff[i + DIR16_STRT + 1] << 8) + F_Buff[i + DIR16_STRT];
					CurFileAddr = Root_Addr + DirBlkNum * 512 + (CurCluster - 2) * SecPerClus * SectorSize;
				}
				else
				{
					CurCluster = (F_Buff[i + DIR32_START_HI + 1] << 24) + (F_Buff[i + DIR32_START_HI] << 16)
						+ (F_Buff[i + DIR32_STRT_LO + 1] << 8) + F_Buff[i + DIR32_STRT_LO];
					CurFileAddr = Root_Addr + (CurCluster - 2) * SecPerClus * SectorSize;
				}
				Display_Info("Open() CurDir", CurDir);
				Display_Info("Open() CurFileAddr", CurFileAddr);
				Display_Info("Open() CurCluster", CurCluster);
				return 0;
			}
		} // for (i = 0; i < 512; i += 32)
	}
	Display_Info("Open() no file found", 0);
	return 0xff;
}
/*******************************************************************************
Function Name : New
 Description : create a file next in sequence
 Para :  name is file name
 ext is extension part of name
 *******************************************************************************/
u8 New(uc8 *name, uc8 *ext)
{

	u16  i, j;
	u32 dirindex;	// index on device for a directory structure
	u16  blkcnt;	// count of blocks read to fine end of a cluster

	Display_Info("New()", 0);
	if (SD_Card == FALSE){
		__Display_Str(0, 0, RED, BKGND_COLOR, SD_Msgs[NoCard]);
		return (0xFF);
	}
	// get free cluster
	if(GetClust()){
		__Display_Str(0, 0, RED, BKGND_COLOR, SD_Msgs[SDErr]);
		return (0xFF);
	}
	// FAT info is read when card inserted
	if (FAT16){
		dirindex = Root_Addr;
	} else { // FAT32
		dirindex = FDT_Start;
	}
	for(blkcnt = 0; blkcnt < SecPerClus; blkcnt++){
		Display_Info("New() dirindex", dirindex);
		if (__MSD_ReadBlock(F_Buff, dirindex & ~(0x1FF), 512)){
			Display_Info("New() MSD_ReadBlock failed", dirindex);
			return 0xff; // Read Sector fail
		}
		for (i = 0; i < 512; i += 32, dirindex += 32){ // within current block
			if(F_Buff[i + DIR_FILE_NM] != DIR_FILE_NM_AVAIL && F_Buff[i + DIR_FILE_NM] != DIR_FILE_NM_ERASED)
				continue;	// try next directory entry
			// found an unused directory entry
			CurFileSz = 0;
			CurDir = dirindex;
			Display_Info("New() found empty entry", CurDir);
			bzero(&F_Buff[i], DIR_LEN);
			for(j = 0; j < 8; j++)
			{
				F_Buff[i + DIR_FILE_NM + j] = name[j];
			}
			for(j = 0; j < 3; j++)
			{
				F_Buff[i + DIR_EXT + j] = ext[j];
			}
			F_Buff[i + DIR_ATTR] = DIR_ATTR_MSK_ARCH;
			F_Buff[i + DIR_OWNER] = 0x2a;	// this works
			F_Buff[i + DIR_OWNER + 1] = 0x41;
			if(FAT16){
				F_Buff[i + DIR16_PERM] = 0;
				F_Buff[i + DIR16_PERM + 1] = 0;
			} else {
				F_Buff[i + DIR32_START_HI + 1] = (CurCluster >> 24) & 0xff;
				F_Buff[i + DIR32_START_HI] = (CurCluster >> 16) & 0xff;
			}
			if(FAT16){
				F_Buff[i + DIR16_STRT + 1] = (CurCluster >> 8) & 0xff;
				F_Buff[i + DIR16_STRT] = CurCluster & 0xff;
			} else {
				F_Buff[i + DIR32_STRT_LO + 1] = (CurCluster >> 8) & 0xff;
				F_Buff[i + DIR32_STRT_LO] = CurCluster & 0xff;
			}
			if (__MSD_WriteBlock(F_Buff, CurDir & ~(0x1FF), 512)){
				Display_Info("New() MSD_WriteBlock failed", CurDir);
				return 0xff;
			}
			Display_Info("New() CurDir", CurDir);
			Display_Info("New() CurFileAddr", CurFileAddr);
			Display_Info("New() CurCluster", CurCluster);
			Cluster_Cnt = 0;
			return 0;
		}	// for (i = 0; i < 512; i += 32)
	} 	// for(blkcnt = 0; blkcnt < SecPerClus; blkcnt++)
	Display_Info("Open() no file found", 0);
	return 0xff;
}

/*******************************************************************************
Function Name : Close
Description : close the active file file
Para :
*******************************************************************************/
char Close()
{
	u16  i, j;
	u32 Disk_Index;
	Display_Info( "Close() CurDir", CurDir);
	// flush out the last block of data
	Disk_Index = CurFileAddr & ~(0x1FF);	// must use block aligned address
	if (__MSD_WriteBlock(F_Buff, Disk_Index, 512)){
		Display_Info( "Close() Error on writing file)", CurFileAddr);
		return 0xFF; // File Write Error
	}
	// update file size in directory entry
	// read block with directory entry
	Disk_Index = CurDir & ~(0x1FF);	// must use block aligned address
	if (__MSD_ReadBlock(F_Buff, Disk_Index, 512)){
		Display_Info( "Close() Err on directory read)", CurDir);
		return 0xFF; // File Write Error
	}
	i = CurDir & 0x1FF;	// index into block for directory entry
	F_Buff[i + DIR_FILE_SZ + 3] = (CurFileSz >> 24) & 0xFF;
	F_Buff[i + DIR_FILE_SZ + 2] = (CurFileSz >> 16) & 0xFF;
	F_Buff[i + DIR_FILE_SZ + 1] = (CurFileSz >> 8) & 0xFF;
	F_Buff[i + DIR_FILE_SZ] = CurFileSz & 0xFF;
	if (__MSD_WriteBlock(F_Buff, Disk_Index, 512)){
		Display_Info( "Close() Err on directory write", CurDir);
		return 0xFF;
	}
	Display_Info( "Close() file size", CurFileSz);
	CurFileSz = 0;
	return 0;
}

/*******************************************************************************
 Function Name : W_Strng
 Description : appends a string to file
 *******************************************************************************/
u8 W_Data(char *datap, u16 len)
{
	u8 rval;
	u16 i;
	u32 blk;

	if(confp->debug_level > 1)
		while(1){}
	i = CurFileAddr & 0x1FF;	// get index within block
	blk = CurFileAddr & ~(0x1FF);
	if(blk != CurBlk){
		if (__MSD_ReadBlock(F_Buff, blk, 512)){
			Display_Info( "W_Data() Error on reading file)", CurFileAddr);
			return 0xFF; // File Write Error
		}
		CurBlk = blk;
	}
	Display_Info( "W_Data() len ", len);
	while(len-- > 0){
		F_Buff[i++] = *datap++;
		CurFileSz++;
		CurFileAddr++;
		if (i >= 512){ // Buffer full
			Display_Info( "W_Data() block filled", CurFileSz);
			if (__MSD_WriteBlock(F_Buff, blk, 512)){
				Display_Info( "W_Data() Error on writing file)", CurFileAddr);
				return 0xFF; // File Write Error
			}
			bzero(F_Buff, 512);
			i = 0;
			Cluster_Cnt++;
			if (Cluster_Cnt >= SecPerClus) {
				if (NextClust()){		// if not enough room
					if(AddClust()) 		// extend the file
						return 0xff;
				}
				Cluster_Cnt = 0;
				blk = CurBlk = CurFileAddr & ~(0x1FF);	// CurFileAddr updated by cluster code
			} else {
				blk += 512;
				CurBlk = blk;
			}
		}
	}
	return 0;
}
/*******************************************************************************
 Function Name : W_Strng
 Description : appends a string to file
 *******************************************************************************/
u8 W_Strng(char *str)
{
	return (W_Data(str, strlen(str)));
}
/*******************************************************************************
 Function Name : Save_Spectrum_CSV
 Description : stores spectrum on flash card as comma separated values
 *******************************************************************************/

// note that msd code returns a non-zero value on error
void Save_Spectrum_CSV(void)
{
	u16 i,n;
	u32 *php, num;
	char  buf[17], *charp;
	bool neg = FALSE;

	strcpy(buf, "SPECT000");

	n = confp->pha_file_seq;
	buf[8] = NULL;
	buf[7] = hex_conv[n % 10];
	n /= 10;
	buf[6] = hex_conv[n % 10];
	n /= 10;
	buf[5] = hex_conv[n % 10];
	Display_Info( buf, confp->pha_file_seq);
	if (Open(buf,"CSV")){
		if (New(buf,"CSV")){
			__Display_Str(0, 0, RED, BKGND_COLOR, SD_Msgs[NoFile]);
			return;
		}
	}
	W_Strng("Gamma Grapher Version 2,\n");
	W_Strng("Comma Separated Values,\n");
	for(php = &pulse_height[0], i = 0; i < MAX_CHANNELS; i++){
		memset(buf, ' ', 16);
		charp = buf;
		num = *php++;
		//Unsigned_To_Dec_Str(charp, U32_DIGITS, (u32) i);
		//charp += U32_DIGITS;
		//charp++ = ',';
		Unsigned_To_Dec_Str(charp, U32_DIGITS, num);
		charp += U32_DIGITS;
		*charp++ = ',';
		*charp++ = '\n';
		*charp++ = NULL;
		W_Strng(buf);
	}
	Close();
	confp->pha_file_seq++;
	Write_Config();
	if(confp->debug_level < 1)
		Display_Menu(confp->menu_index);
}

/*******************************************************************************
 Function Name : Save_Spectrum_N42
 Description : stores spectrum on flash card in NIST N42 format
 *******************************************************************************/
void Save_Spectrum_N42(void)
{
	int i;
	u32 *php, num;
	char  buf[12], n;
	bool neg = FALSE;

	if (Open("FILE0000","DAT") == 0){
		__Display_Str(22 * CHAR_WIDTH, 0, TXT_COLOR, BKGND_COLOR, SD_Msgs[NoFile]);
	}

	W_Strng( "<Measurement>\n");
	W_Strng( "    <Remarks>\n");
	W_Strng( "    <Remarks>\n");
	W_Strng( "    \n");
	W_Strng( "    <InstrumentInformation>\n");
	W_Strng( "        <InstrumentType>\n");
	W_Strng( "        </InstrumentType>\n");
	W_Strng( "        <Manufacturer>\n");
	W_Strng( "        </Manufacturer>\n");
	W_Strng( "        <InstrumentModel>\n");
	W_Strng( "        </InstrumentModel>\n");
	W_Strng(  "        <InstrumentID>\n");
	W_Strng( "        </InstrumentID>\n");
	W_Strng( "    </InstrumentInformation>\n");
	W_Strng( "    <MeasuredItemInformation>\n");
	W_Strng( "        <MeasurementLocation>\n");
	W_Strng( "            <Coordinates>\n");        // The spec calls for coordinates inside the braces
	W_Strng( "            </Coordinates>\n");
	W_Strng( "        </MeasurementLocation>\n");
	W_Strng( "    </MeasuredItemInformation>\n");
	W_Strng( "    <Spectrum>\n");
	W_Strng( "        <StartTime>\n");
	//W_Strng( "        " << samp.sampleDateTime.toString(Qt::ISODate) << endl;
	W_Strng( "        </StartTime>\n");
	W_Strng( "        <RealTime>\n");
	//W_Strng( "            " << str.setNum(samp.realTime, 'f', 1) << endl;
	W_Strng( "        </RealTime>\n");
	W_Strng( "        <LiveTime>\n");
	//W_Strng( "            " << str.setNum(samp.liveTime, 'f', 1) << endl;
	W_Strng( "        </LiveTime>\n");
	W_Strng( "        <Calibration>\n");
	W_Strng( "            <Equation>\n");
	W_Strng( "            </Equation>\n");
	W_Strng( "        </Calibration>\n");
	W_Strng( "        <ChannelData>\n");
	for(php = &pulse_height[0], i = MAX_CHANNELS; i > 0; i--){
		num = *php++;
		Unsigned_To_Dec_Str(buf, U32_DIGITS, num);
		buf[U32_DIGITS+1] = 0;
		W_Strng( "            ");
		W_Strng(buf);
	}
	W_Strng( "        </ChannelData>\n");
	W_Strng( "    </Spectrum>\n");
	W_Strng( "</Measurement>\n");
}

/******************************** END OF FILE *********************************/
