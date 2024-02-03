#include "user_flash.h"
#include "stm32f1xx_hal_flash.h"
// flash �������� begin
// stm32 flash����С�˴洢��ʽ �� ��λ��ǰ ��λ�ں�
// ����flash[2] = {0x58,0x0D} ���ȡ������ֵ���� uint16 = 0x0D58

/* �����2K�ֽ� �������2����ΪSTM_SECTOR_SIZE���ֽ�����STMFLASH_BUF�����ǰ������� */
uint16_t  STMFLASH_BUF[STM_SECTOR_SIZE/2];  

/**
*@brief		��ȡָ����ַ�İ���(16λ����)
*@param		faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
*@return	��ȡ������.
*/
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
	return *(uint16_t*)faddr; 
}
/**
*@brief     ��ָ����ַ��ʼ����ָ�����ȵ�����
*@param     ReadAddr  : ��ʼ��ַ
*           pBuffer   : ����ָ��
*           NumToWrite: ����(16λ)��
*@return    ��
*/
void STMFLASH_ReadMultipleBytes(uint32_t ReadAddr,uint16_t  *pBuffer,uint16_t  NumToRead)      
{
    uint16_t  i;
    for(i = 0;i < NumToRead;i++)
    {
        pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);         // ��ȡ2���ֽ�.
        ReadAddr += 2;                                        // ƫ��2���ֽ�. 
    }
}
/**
*@brief   ��У��д�루�ú�����У��Flash��ַ�Ƿ��д���Ƿ���Ҫ���������Բ���ֱ������д������
*@param   WriteAddr : ��ʼ��ַ
*         pBuffer   : ����ָ��
*         NumToWrite: ����(16λ)��   
*@return  ��
*/
void STMFLASH_Write_NoCheck(uint32_t WriteAddr,uint16_t  *pBuffer,uint16_t  NumToWrite)   
{                    
    uint16_t  i;
    for(i = 0;i < NumToWrite;i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,WriteAddr,pBuffer[i]);  // д��flash FLASH_TYPEPROGRAM_HALFWORD�ǰ���16bitд���

        WriteAddr += 2;                              // ��ַ����2.
    }  
} 
/**
*@brief     ��ָ����ַ��ʼд��ָ�����ȵ�����
*@param     WriteAddr : ��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
*           pBuffer   : ����ָ��
*           NumToWrite: ����(16λ)��(����Ҫд���16λ���ݵĸ���.)
*@return    ��
*/
int8_t STMFLASH_WriteMultipleBytes(uint32_t WriteAddr,uint16_t  *pBuffer,uint16_t  NumToWrite)
{
    uint32_t  PageAddr;                                                                                            // Flashҳ��ַ
    uint16_t  WordAddr;                                                                                            // Ҫд��ĵ�ַ��Flashҳ�е�λ��(16λ�ּ���)
    uint16_t  WordRemainder;                                                                                       // Flashҳ�е�ʣ���ַ(16λ�ּ���)
    uint32_t  ShiftingAddr;                                                                                        // ȥ��0X08000000��ĵ�ַ
    uint16_t  i;    
    __disable_irq();
    // ��֤�Ƿ��ܱ�2����
    if (WriteAddr % 2 != 0) 
    {
        return -1;
    } 

    /* ��ָ����ʼ��ַС��STM32_FLASH_BASE (0x0800000) ���ߴ���оƬ�����Flash����ʱ��д���ַ��Ч���������� */
    if(WriteAddr < STM32_FLASH_BASE || (WriteAddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE)))
    {
        return -2;                                                                                                    // �Ƿ���ַ
    }
    HAL_FLASH_Unlock(); 
    ShiftingAddr  = WriteAddr - STM32_FLASH_BASE;                                                                  // ʵ��ƫ�Ƶ�ַ.Ҫд��������ʼ��ַ��λ��
    PageAddr      =  ShiftingAddr / FLASH_PAGE_SIZE;                                                               // Ҫд��ĵ�ַ���ڵ�Flashҳ��0~256��
    WordAddr      = (ShiftingAddr % FLASH_PAGE_SIZE) / 2;                                                          // ��Flashҳ�ڵ�ƫ��(2���ֽ�Ϊ������λ.)
    WordRemainder = FLASH_PAGE_SIZE/2 - WordAddr;                                                                  // Flashҳʣ��ռ��С
    if(NumToWrite <= WordRemainder)
    {
        WordRemainder = NumToWrite;                                                                                // �����ڸ�Flashҳ��Χ
    }
 
    while(1) 
    {
        STMFLASH_ReadMultipleBytes(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE,STMFLASH_BUF,FLASH_PAGE_SIZE/2);    // ��������Flashҳ�����ݴ�ŵ�STMFLASH_BUF��
        
        /* �������ݣ���flashҳ�Ƿ���Ҫ���� */
        for(i = 0;i < WordRemainder;i++)                                                                           // У������
        {
            if(STMFLASH_BUF[WordAddr + i] != 0XFFFF)
            {
                break;                                                                                             // ��Ҫ����
            }     
        }
        /* ���Ҫд�����ݵ�Flashҳ���ϣ����е��ֶ�����0XFFFF����ô�������ѭ��֮��i = WordRemainder*/
        if(i < WordRemainder)                                                                                      // ��Ҫ����
        {
            //FLASH_ErasePage(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE);                                      // �������Flashҳ
            FLASH_EraseInitTypeDef FLASH_EraseInitType;
            FLASH_EraseInitType.TypeErase 	= FLASH_TYPEERASE_PAGES;
            FLASH_EraseInitType.PageAddress = PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE;
            FLASH_EraseInitType.NbPages 		= 1;
            uint32_t PageError = HAL_FLASH_GetError();
            HAL_FLASHEx_Erase(&FLASH_EraseInitType, &PageError);

            for(i = 0;i < WordRemainder;i++)                                                                       // ����
            {
                STMFLASH_BUF[i + WordAddr] = pBuffer[i];
            }
            STMFLASH_Write_NoCheck(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE,STMFLASH_BUF,FLASH_PAGE_SIZE/2);    // д������ҳ
        }
        /* i = WordRemainder */
        else
        {
            STMFLASH_Write_NoCheck(WriteAddr,pBuffer,WordRemainder);                                               // д�Ѿ������˵�,ֱ��д������ʣ������. 
        }
 
        if(NumToWrite == WordRemainder)
        {
            break;                                                                                                 // д�������
        }
        else                                                                                                       // д��δ����
        {
            PageAddr++;                                                                                            // ҳ��ַ����1
            WordAddr    = 0;                                                                                       // ƫ��λ��Ϊ0     
            pBuffer    += WordRemainder;                                                                           // ָ��ƫ��
            WriteAddr  += WordRemainder*2;                                                                         // д��ַƫ��(16λ����ַ,��Ҫ*2)
            NumToWrite -= WordRemainder;                                                                           // �ֽ�(16λ)���ݼ�
            if(NumToWrite > (FLASH_PAGE_SIZE/2))
            {
                WordRemainder = FLASH_PAGE_SIZE/2;                                                                 // ��һ��Flashҳ����д����
            }
            else WordRemainder = NumToWrite;                                                                       // ��һ��Flashҳ����д����
        }    
    };  
    

	HAL_FLASH_Lock();
    __enable_irq();
    return 0;
    
}
// flash �������� end
