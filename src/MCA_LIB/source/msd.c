/*******************************************************************************
 * File Name: msd.c
 *******************************************************************************/
#include "string.h"
#include "stm32f10x_type.h"
#include "msd.h"
#include "usb_scsi.h"
#include "usb_bot.h"
#include "usb_lib.h"
#define _FLASH_PROG
#include "stm32f10x_flash.h"
#include "HW_V1_Config.h"

void SD_Set_Changed(void);
u8 SD_Is_Changed(void);
u8 SD_Card_Check(void);
void Read_Memory(void);
void Write_Memory(void);
bool Address_Management_Test(u8 Cmd);
void *memcpy(void *dest, const void *src, size_t n);

vu32 Memory_Offset;                   //Memory Offset
u32 Transfer_Length;                  //Transfer Length
vu32 Block_Read_count = 0;
vu32 Block_offset;
vu32 Counter = 0;
u8 Data_Buffer[BULK_MAX_PACKET_SIZE * 8]; //512 bytes

extern u8 Bulk_Data_Buff[BULK_MAX_PACKET_SIZE];  //data buffer
extern u16 Data_Len;
extern u8 Bot_State;
extern Bulk_Only_CBW CBW;
extern Bulk_Only_CSW CSW;
extern u16 boot_msg_y;
extern sMSD_CSD MSD_csd;
extern u32 Mass_Block_Size;
extern u32 Mass_Block_Count;

volatile u8 must_init = 1;
volatile u8 must_refresh = 1;
volatile u8 mutex = 0;


/*******************************************************************************
 * Set SD refresh flag
 *******************************************************************************/
void SD_Set_Changed(void)
{
	must_refresh = 1;
}
/*******************************************************************************
 * Set SD refresh flag
 *******************************************************************************/
// used by USB only
u8 SD_Is_Changed(void)
{
	if (must_refresh) {
		must_refresh = 0;
		return 1;
	}
	return 0;
}
/*******************************************************************************
 * Check if card is resent (return 1)
 *******************************************************************************/
// used by USB and main()
u8 SD_Card_Check(void)
{
	if (!SD_Card_ON()) {
		must_init = 1;
		Mass_Block_Count = 0;
		return 0;
	}
	if (must_init) {
		if (MSD_Init()) return 0;
		Get_Medium_Characteristics();
		SD_Set_Changed();
		must_init = 0;
	}
	return 1;
}

/*******************************************************************************
 * Get_Medium_Characteristics :Read MicroSD Size
 *******************************************************************************/
void Get_Medium_Characteristics(void)
{
	unsigned int temp_block_mul;
	unsigned int DeviceSizeMul;

	MSD_GetCSDRegister(&MSD_csd);

	if(MSD_csd.CSDStruct == 1) { // SD version 2.0
		Mass_Block_Count = (MSD_csd.DeviceSize + 1) << 10;	// * (524288 / 512)
	} else {  // SD version 1.0
		DeviceSizeMul = MSD_csd.DeviceSizeMul + 2;
		temp_block_mul = (1 << MSD_csd.RdBlockLen) / 512;
		Mass_Block_Count = ((MSD_csd.DeviceSize + 1) * (1 << (DeviceSizeMul))) * temp_block_mul;
	}
	Mass_Block_Size = 512;
}
/*******************************************************************************
 * Function Name  : Read_MSD
 * Description    : Handle the Read operation from the microSD card.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 Used by USB only.
 *******************************************************************************/
void Read_MSD(void)
{
	if (!Block_Read_count)
	{
		MSD_ReadBlock(Data_Buffer, Memory_Offset, 512);
		UserToPMABufferCopy(Data_Buffer, ENDP1_TXADDR, BULK_MAX_PACKET_SIZE);
		Block_Read_count = 512 - BULK_MAX_PACKET_SIZE;
		Block_offset = BULK_MAX_PACKET_SIZE;
	}
	else
	{
		UserToPMABufferCopy(Data_Buffer + Block_offset, ENDP1_TXADDR, BULK_MAX_PACKET_SIZE);
		Block_Read_count -= BULK_MAX_PACKET_SIZE;
		Block_offset += BULK_MAX_PACKET_SIZE;
	}
	SetEPTxCount(ENDP1, BULK_MAX_PACKET_SIZE);
	SetEPTxStatus(ENDP1, EP_TX_VALID);
	Memory_Offset += BULK_MAX_PACKET_SIZE;
	Transfer_Length -= BULK_MAX_PACKET_SIZE;
	CSW.dDataResidue -= BULK_MAX_PACKET_SIZE;
	if (Transfer_Length == 0)
	{
		Block_Read_count = 0;
		Block_offset = 0;
		Memory_Offset = 0;
		Bot_State = BOT_DATA_IN_LAST;
	}
}

/*******************************************************************************
 * Function Name  : Write_MSD
 * Description    : Handle the Write operation to the microSD card.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 Used by USB only.
 *******************************************************************************/
void Write_MSD(void)
{
	u32 temp =  Counter + 64;
	u32 i = 0;

	for (; Counter < temp; Counter++)
	{
		Data_Buffer[Counter] = Bulk_Data_Buff[i];
		i++;
	}
	Memory_Offset += Data_Len;
	Transfer_Length -= Data_Len;
	if (!(Transfer_Length % 512))
	{
		Counter = 0;
		MSD_WriteBlock(Data_Buffer, Memory_Offset - 512, 512);
	}
	CSW.dDataResidue -= Data_Len;
	SetEPRxStatus(ENDP2, EP_RX_VALID); //enable the next transaction
	if ((Transfer_Length == 0) || (Bot_State == BOT_CSW_Send))
	{
		Counter = 0;
		Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
	}
}

/*******************************************************************************
 * Function Name  : Address_Management_Test
 * Description    : Test the received address.
 * Input          : u8 Cmd : the command can be SCSI_READ10 or SCSI_WRITE10.
 * Output         : None.
 * Return         : Read\Write status (bool).
	 *******************************************************************************/
bool Address_Management_Test(u8 Cmd)
{
	vu32 temp1;
	vu32 temp2;

	//Logical Block Address of First Block
	temp1 = (CBW.CB[2] << 24) |
		(CBW.CB[3] << 16) |
		(CBW.CB[4] <<  8) |
		(CBW.CB[5] <<  0);
	//Number of Blocks to transfer
	temp2 = (CBW.CB[7] <<  8) |
		(CBW.CB[8] <<  0);
	Memory_Offset = temp1 * Mass_Block_Size;
	Transfer_Length = temp2 * Mass_Block_Size;
	if (((Memory_Offset + Transfer_Length) >> 9) > Mass_Block_Count)	// 512 byte blocks
	{
		if (Cmd == SCSI_WRITE10)
			Bot_Abort(BOTH_DIR);
		else
			Bot_Abort(DIR_IN);
		Set_Scsi_Sense_Data(ILLEGAL_REQUEST, ADDRESS_OUT_OF_RANGE);
		Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
		return (FALSE);
	}
	if (CBW.dDataLength != Transfer_Length)
	{
		if (Cmd == SCSI_WRITE10)
			Bot_Abort(BOTH_DIR);
		else
			Bot_Abort(DIR_IN);
		Set_Scsi_Sense_Data(ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
		Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
		return (FALSE);
	}
	return (TRUE);
}
/*******************************************************************************
 Delay few time
 *******************************************************************************/
void Delay_us(void)
{
	volatile u32 dlyCount = 0x100;
	while(--dlyCount);
}
/*******************************************************************************
 MicroSD initialization                                    return: 0x00=success
 *******************************************************************************/
u8 MSD_Init(void)
{
	u32 i=0;

	SPI_Config();
	MSD_CS_HIGH();
	for(i=0; i <10; i++)
		MSD_WriteByte(DUMMY);  //Send dummy byte: 8 Clock pulses of delay
	return MSD_GoIdleState();
}
/*******************************************************************************
 MicroSD Write a Block                                   return: 0x00=success
 pBuffer = memory address of a 512 byte sized data
 WriteAddr = block address, in blocks
 NumByteToWrite = 512
 *******************************************************************************/
u8 MSD_WriteBlock(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{ u32 i=0; u8 rvalue = MSD_RESPONSE_FAILURE;
	mutex++;
	while (mutex > 1);
	MSD_CS_LOW();
	Delay_us();
	MSD_SendCmd(MSD_WRITE_BLOCK, WriteAddr, 0);//Send CMD24
	if(!MSD_GetResponse(MSD_RESPONSE_NO_ERROR)){
		MSD_WriteByte(DUMMY);                       //Send dummy byte: 8 Clock pulses of delay
		MSD_WriteByte(0xFE);
		for(i=0; i<NumByteToWrite; i++) {
			MSD_WriteByte(*pBuffer); pBuffer++;
		}
		MSD_ReadByte();
		MSD_ReadByte();             //DUMMY CRC bytes
		if(MSD_GetDataResponse()==MSD_DATA_OK)
			rvalue=MSD_RESPONSE_NO_ERROR;
	}
	MSD_CS_HIGH(); MSD_WriteByte(DUMMY);          //Send dummy byte: 8 Clock pulses of delay
	mutex--;
	return rvalue;
}
/*******************************************************************************
 MicroSD Read a Block                                   return: 0x00=success
 pBuffer = memory address of a 512 byte of data
 WriteAddr = block address, in blocks
 NumByteToWrite = 512
 *******************************************************************************/
u8 MSD_ReadBlock(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
	u32 i = 0;
	u8 rvalue = MSD_RESPONSE_FAILURE;
	mutex++;
	while (mutex > 1);
	MSD_CS_LOW(); Delay_us();
	MSD_SendCmd(MSD_READ_SINGLE_BLOCK, ReadAddr, 0);		//send CMD17
	if (!MSD_GetResponse(MSD_RESPONSE_NO_ERROR)) {
		if (!MSD_GetResponse(MSD_START_DATA_SINGLE_BLOCK_READ)) {
			for (i = 0; i < NumByteToRead; i++) {
				*pBuffer = MSD_ReadByte(); pBuffer++;
			}
			MSD_ReadByte();
			MSD_ReadByte();                //DUMMY CRC bytes
			rvalue = MSD_RESPONSE_NO_ERROR;
		} else {
			//Display_Info("ReadBlock: no token", 0);
		}
	} else {
		//Display_Info("ReadBlock: CMD17 failed", 0);
	}
	MSD_CS_HIGH();
	MSD_WriteByte(DUMMY);               //Send dummy byte: 8 Clock pulses of delay
	mutex--;
	return rvalue;
}

/*******************************************************************************
 * Function Name  : MSD_GetCSDRegister
 * Description    : Read the CSD card register.
 *                  Reading the contents of the CSD register in SPI mode
 *                  is a simple read-block transaction.
 * Input          : - MSD_csd: pointer on an SCD register structure
 * Output         : None
 * Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
 *                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed
 *******************************************************************************/
u8 MSD_GetCSDRegister(sMSD_CSD* MSD_csd)
{
	u32 i = 0;
	u8 rvalue = MSD_RESPONSE_FAILURE;
	u8 CSD_Tab[16];

	/* MSD chip select low */
	MSD_CS_LOW();
	Delay_us();
	/* Send CMD9 (CSD register) or CMD10(CSD register) */
	MSD_SendCmd(MSD_SEND_CSD, 0, 0);
	/* Wait for response in the R1 format (0x00 is no errors) */
	if (!MSD_GetResponse(MSD_RESPONSE_NO_ERROR))
	{
		if (!MSD_GetResponse(MSD_START_DATA_SINGLE_BLOCK_READ))
		{
			for (i = 0; i < 16; i++)
			{
				/* Store CSD register value on CSD_Tab */
				CSD_Tab[i] = MSD_ReadByte();
			}
		}
		/* Get CRC bytes (not really needed by us, but required by MSD) */
		MSD_ReadByte();
		MSD_ReadByte();
		/* Set response value to success */
		rvalue = MSD_RESPONSE_NO_ERROR;
	}
	/* MSD chip select high */
	MSD_CS_HIGH();
	/* Send dummy byte: 8 Clock pulses of delay */
	MSD_WriteByte(DUMMY);
	/* Byte 0 */
	MSD_csd->CSDStruct = (CSD_Tab[0] & 0xC0) >> 6;
	MSD_csd->SysSpecVersion = (CSD_Tab[0] & 0x3C) >> 2;
	MSD_csd->Reserved1 = 0;
	/* Byte 1 */
	MSD_csd->TAAC = CSD_Tab[1] ;
	/* Byte 2 */
	MSD_csd->NSAC = CSD_Tab[2];
	/* Byte 3 */
	MSD_csd->MaxBusClkFrec = CSD_Tab[3];
	/* Byte 4 */
	MSD_csd->CardComdClasses = CSD_Tab[4] << 4;
	/* Byte 5 */
	MSD_csd->CardComdClasses |= (CSD_Tab[5] & 0xF0) >> 4;
	MSD_csd->RdBlockLen = CSD_Tab[5] & 0x0F;
	/* Byte 6 */
	MSD_csd->PartBlockRead = (CSD_Tab[6] & 0x80) >> 7;
	MSD_csd->WrBlockMisalign = (CSD_Tab[6] & 0x40) >> 6;
	MSD_csd->RdBlockMisalign = (CSD_Tab[6] & 0x20) >> 5;
	MSD_csd->DSRImpl = (CSD_Tab[6] & 0x10) >> 4;
	MSD_csd->Reserved2 = 0; /* Reserved */
/*
	if ((csd[0] >> 6) == 1) {	// SDC version 2.00
		csize = csd[9] + ((WORD)csd[8] << 8) + 1;
		*(DWORD*)buff = (DWORD)csize << 10;
	} else {					// SDC version 1.XX or MMC
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
		*(DWORD*)buff = (DWORD)csize << (n - 9);
	}
*/
	if(MSD_csd->CSDStruct == 1){
		/* Byte 7 */
		/* Byte 8 */
		MSD_csd->DeviceSize = CSD_Tab[8] << 8;
		/* Byte 9 */
		MSD_csd->DeviceSize |= CSD_Tab[9];
	} else {
		MSD_csd->DeviceSize = (CSD_Tab[6] & 0x03) << 10;
		/* Byte 7 */
		MSD_csd->DeviceSize |= (CSD_Tab[7]) << 2;
		/* Byte 8 */
		MSD_csd->DeviceSize |= (CSD_Tab[8] & 0xC0) >> 6;
		MSD_csd->MaxRdCurrentVDDMin = (CSD_Tab[8] & 0x38) >> 3;
		MSD_csd->MaxRdCurrentVDDMax = (CSD_Tab[8] & 0x07);
		/* Byte 9 */
		MSD_csd->MaxWrCurrentVDDMin = (CSD_Tab[9] & 0xE0) >> 5;
		MSD_csd->MaxWrCurrentVDDMax = (CSD_Tab[9] & 0x1C) >> 2;
		MSD_csd->DeviceSizeMul = (CSD_Tab[9] & 0x03) << 1;
	}
	/* Byte 10 */
	MSD_csd->DeviceSizeMul |= (CSD_Tab[10] & 0x80) >> 7;
	MSD_csd->EraseGrSize = (CSD_Tab[10] & 0x7C) >> 2;
	MSD_csd->EraseGrMul = (CSD_Tab[10] & 0x03) << 3;
	/* Byte 11 */
	MSD_csd->EraseGrMul |= (CSD_Tab[11] & 0xE0) >> 5;
	MSD_csd->WrProtectGrSize = (CSD_Tab[11] & 0x1F);
	/* Byte 12 */
	MSD_csd->WrProtectGrEnable = (CSD_Tab[12] & 0x80) >> 7;
	MSD_csd->ManDeflECC = (CSD_Tab[12] & 0x60) >> 5;
	MSD_csd->WrSpeedFact = (CSD_Tab[12] & 0x1C) >> 2;
	MSD_csd->MaxWrBlockLen = (CSD_Tab[12] & 0x03) << 2;
	/* Byte 13 */
	MSD_csd->MaxWrBlockLen |= (CSD_Tab[13] & 0xc0) >> 6;
	MSD_csd->WriteBlockPaPartial = (CSD_Tab[13] & 0x20) >> 5;
	MSD_csd->Reserved3 = 0;
	MSD_csd->ContentProtectAppli = (CSD_Tab[13] & 0x01);
	/* Byte 14 */
	MSD_csd->FileFormatGrouop = (CSD_Tab[14] & 0x80) >> 7;
	MSD_csd->CopyFlag = (CSD_Tab[14] & 0x40) >> 6;
	MSD_csd->PermWrProtect = (CSD_Tab[14] & 0x20) >> 5;
	MSD_csd->TempWrProtect = (CSD_Tab[14] & 0x10) >> 4;
	MSD_csd->FileFormat = (CSD_Tab[14] & 0x0C) >> 2;
	MSD_csd->ECC = (CSD_Tab[14] & 0x03);
	/* Byte 15 */
	MSD_csd->CRC = (CSD_Tab[15] & 0xFE) >> 1;
	MSD_csd->Reserved4 = 1;
	/* Return the reponse */
	return rvalue;
}

/*******************************************************************************
 * Function Name  : MSD_GetCIDRegister
 * Description    : Read the CID card register.
 *                  Reading the contents of the CID register in SPI mode
 *                  is a simple read-block transaction.
 * Input          : - MSD_cid: pointer on an CID register structure
 * Output         : None
 * Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
 *                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed
 *******************************************************************************/
u8 MSD_GetCIDRegister(sMSD_CID* MSD_cid)
{
	u32 i = 0;
	u8 rvalue = MSD_RESPONSE_FAILURE;
	u8 CID_Tab[16];

	/* MSD chip select low */
	MSD_CS_LOW();
	Delay_us();
	/* Send CMD10 (CID register) */
	MSD_SendCmd(MSD_SEND_CID, 0, 0);
	/* Wait for response in the R1 format (0x00 is no errors) */
	if (!MSD_GetResponse(MSD_RESPONSE_NO_ERROR))
	{
		if (!MSD_GetResponse(MSD_START_DATA_SINGLE_BLOCK_READ))
		{
			/* Store CID register value on CID_Tab */
			for (i = 0; i < 16; i++)
			{
				CID_Tab[i] = MSD_ReadByte();
			}
		}
		/* Get CRC bytes (not really needed by us, but required by MSD) */
		MSD_WriteByte(DUMMY);
		MSD_WriteByte(DUMMY);
		/* Set response value to success */
		rvalue = MSD_RESPONSE_NO_ERROR;
	}
	/* MSD chip select high */
	MSD_CS_HIGH();
	/* Send dummy byte: 8 Clock pulses of delay */
	MSD_WriteByte(DUMMY);
	/* Byte 0 */
	MSD_cid->ManufacturerID = CID_Tab[0];
	/* Byte 1 */
	MSD_cid->OEM_AppliID = CID_Tab[1] << 8;
	/* Byte 2 */
	MSD_cid->OEM_AppliID |= CID_Tab[2];
	/* Byte 3 */
	MSD_cid->ProdName1 = CID_Tab[3] << 24;
	/* Byte 4 */
	MSD_cid->ProdName1 |= CID_Tab[4] << 16;
	/* Byte 5 */
	MSD_cid->ProdName1 |= CID_Tab[5] << 8;
	/* Byte 6 */
	MSD_cid->ProdName1 |= CID_Tab[6];
	/* Byte 7 */
	MSD_cid->ProdName2 = CID_Tab[7];
	/* Byte 8 */
	MSD_cid->ProdRev = CID_Tab[8];
	/* Byte 9 */
	MSD_cid->ProdSN = CID_Tab[9] << 24;
	/* Byte 10 */
	MSD_cid->ProdSN |= CID_Tab[10] << 16;
	/* Byte 11 */
	MSD_cid->ProdSN |= CID_Tab[11] << 8;
	/* Byte 12 */
	MSD_cid->ProdSN |= CID_Tab[12];
	/* Byte 13 */
	MSD_cid->Reserved1 |= (CID_Tab[13] & 0xF0) >> 4;
	/* Byte 14 */
	MSD_cid->ManufactDate = (CID_Tab[13] & 0x0F) << 8;
	/* Byte 15 */
	MSD_cid->ManufactDate |= CID_Tab[14];
	/* Byte 16 */
	MSD_cid->CRC = (CID_Tab[15] & 0xFE) >> 1;
	MSD_cid->Reserved2 = 1;
	/* Return the reponse */
	return rvalue;
}

/*******************************************************************************
 * Function Name  : MSD_SendCmd
 * Description    : Send 5 bytes command to the MSD card.
 * Input          : - Cmd: the user expected command to send to MSD card
 *                  - Arg: the command argument
 *                  - Crc: the CRC
 * Output         : None
 * Return         : None
 *******************************************************************************/
void MSD_SendCmd(u8 Cmd, u32 Arg, u8 Crc)
{
	u32 i=0;
	u8 Frame[6];
	Frame[0] = (Cmd | 0x40);
	Frame[1] = (u8)(Arg >> 24);
	Frame[2] = (u8)(Arg >> 16);
	Frame[3] = (u8)(Arg >> 8);
	Frame[4] = (u8)(Arg);
	Frame[5] = (Crc);
	if(Crc != 0){	// special case when first putting card into SPI mode
		for (i=0; i < 6; i++){
			MSD_WriteByte(Frame[i]);
		}
	} else {
		for (i=0; i < 5; i++){
			MSD_WriteByte(Frame[i]);
		}
		// Use the stm32 crc hardware
		SPI_TransmitCRC(SPI2);
	}
}
/*******************************************************************************
 * Function Name  : MSD_GetDataResponse
 * Description    : Get MSD card data response.
 * Input          : None
 * Output         : None
 * Return         : The MSD status: Read data response xxx0<status>1
 *                   - status 010: Data accecpted
 *                   - status 101: Data rejected due to a crc error
 *                   - status 110: Data rejected due to a Write error.
 *                   - status 111: Data rejected due to other error.
 *******************************************************************************/
u8 MSD_GetDataResponse(void)
{ u32 i=0; u8 response, rvalue;
	while (i<=64){
		response = MSD_ReadByte();//Read resonse
		response &= 0x1F;//Mask unused bits
		switch (response){
			case MSD_DATA_OK:
			{
				rvalue = MSD_DATA_OK;
				break;
			}
			case MSD_DATA_CRC_ERROR:
				return MSD_DATA_CRC_ERROR;
			case MSD_DATA_WRITE_ERROR:
				return MSD_DATA_WRITE_ERROR;
			default:
			{
				rvalue = MSD_DATA_OTHER_ERROR;
				break;
			}
		}
		if (rvalue == MSD_DATA_OK)//Exit loop in case of data ok
			break;
		i++;//Increment loop counter
	}
	while (MSD_ReadByte()==0);//Wait null data
	return response;//Return response
}
/*******************************************************************************
 * Function Name  : MSD_GetResponse
 * Description    : Returns the MSD response.
 * Input          : None
 * Output         : None
 * Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
 *                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed
 *******************************************************************************/
u8 MSD_GetResponse(u8 Response)
{ u32 Count=0xFFF;
	while ((MSD_ReadByte()!=Response)&& Count) Count--;//Check if response is got or a timeout is happen
	if (Count==0) return MSD_RESPONSE_FAILURE;//After time out
	else return MSD_RESPONSE_NO_ERROR;//Right response got
}
/*******************************************************************************
 * Function Name  : MSD_GetStatus
 * Description    : Returns the MSD status.
 * Input          : None
 * Output         : None
 * Return         : The MSD status.
 *******************************************************************************/
u16 MSD_GetStatus(void)
{
	u16 Status = 0;

	/* MSD chip select low */
	MSD_CS_LOW();
	Delay_us();
	/* Send CMD13 (MSD_SEND_STATUS) to get MSD status */
	MSD_SendCmd(MSD_SEND_STATUS, 0, 0);
	Status = MSD_ReadByte();
	Status |= (u16)(MSD_ReadByte() << 8);
	/* MSD chip select high */
	MSD_CS_HIGH();
	/* Send dummy byte 0xFF */
	MSD_WriteByte(DUMMY);
	return Status;
}

/*******************************************************************************
 * Function Name  : MSD_GoIdleState
 * Description    : Put MSD in Idle state.
 * Input          : None
 * Output         : None
 * Return         : The MSD Response: - MSD_RESPONSE_FAILURE: Sequence failed
 *                                    - MSD_RESPONSE_NO_ERROR: Sequence succeed
 *******************************************************************************/
u8 MSD_GoIdleState(void)
{
	u8 cnt;

	//Display_Info(8, (boot_msg_y -= 16), "MSD_GoIdleState ", 0);
	//Delay_MS(1000);
	/* MSD chip select low */
	MSD_CS_LOW();
	Delay_us();
	/* Send CMD0 (GO_IDLE_STATE) to put MSD in SPI mode */
	MSD_SendCmd(MSD_GO_IDLE_STATE, 0, 0x95);
	/* Wait for In Idle State Response (R1 Format) equal to 0x01 */
	if (MSD_GetResponse(MSD_IN_IDLE_STATE))
	{
		/* No Idle State Response: return response failue */
		//Display_Info("SD Flash: No Idle State Response", 0);
		return MSD_RESPONSE_FAILURE;
	}
	/*----------Activates the card initialization process-----------*/
	//Display_Info(8, (boot_msg_y -= 16), "MSD_GoIdleState ", 1);
	cnt = 0xff;
	do{
		/* MSD chip select high */
		MSD_CS_HIGH();
		/* Send Dummy byte 0xFF */
		MSD_WriteByte(DUMMY);
		/* MSD chip select low */
		MSD_CS_LOW();
		/* Send CMD1 (Activates the card process) until response equal to 0x0 */
		MSD_SendCmd(MSD_SEND_OP_COND, 0, 0);
		/* Wait for no error Response (R1 Format) equal to 0x00 */
	} while (MSD_GetResponse(MSD_RESPONSE_NO_ERROR) && --cnt);
	if (cnt == 0){
		//Display_Info("SD Flash: Stuck In Idle", 0);
		return MSD_RESPONSE_FAILURE;
	}
	//Display_Info(8, (boot_msg_y -= 16), "MSD_GoIdleState ", 2);
	/* MSD chip select high */
	MSD_CS_HIGH();
	/* Send dummy byte 0xFF */
	MSD_WriteByte(DUMMY);
	//Display_Info(8, (boot_msg_y -= 16), "MSD_GoIdleState Success", 0);
	//Delay_MS(1000);
	return MSD_RESPONSE_NO_ERROR;
}
/**********************************************************************
 Function Name : Flash_Write
 Description : stores configuration in flash memory
 *******************************************************************************/
void Flash_Write(u16 *hwordp, u32 hword_adr, u16 sz)
{
	u16 x;

	FLASH_Unlock();
	FLASH_ErasePage(hword_adr);
	for(; sz > 0; sz -= 2){
		FLASH_ProgramHalfWord(hword_adr, *hwordp);
		hword_adr += 2;
		hwordp++;
	}
	FLASH_Lock();
}
/*******************************************************************************
 Function Name : Flash_Read
 Description : restores configuration from flash memory
 *******************************************************************************/
void Flash_Read(u16 *hwordp, u32 hword_adr, u16 sz)
{
	memcpy(hwordp, (const void *)hword_adr, sz);
}
/********************************* END OF FILE ********************************/
