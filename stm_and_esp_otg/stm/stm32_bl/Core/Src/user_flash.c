#include "user_flash.h"
#include "stm32f1xx_hal_flash.h"
// flash 操作函数 begin
// stm32 flash采用小端存储方式 即 低位在前 高位在后
// 比如flash[2] = {0x58,0x0D} 则读取出来的值就是 uint16 = 0x0D58

/* 最多是2K字节 这里除以2是因为STM_SECTOR_SIZE是字节数，STMFLASH_BUF数组是半字数组 */
uint16_t  STMFLASH_BUF[STM_SECTOR_SIZE/2];  

/**
*@brief		读取指定地址的半字(16位数据)
*@param		faddr:读地址(此地址必须为2的倍数!!)
*@return	读取的数据.
*/
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
	return *(uint16_t*)faddr; 
}
/**
*@brief     从指定地址开始读出指定长度的数据
*@param     ReadAddr  : 起始地址
*           pBuffer   : 数据指针
*           NumToWrite: 半字(16位)数
*@return    无
*/
void STMFLASH_ReadMultipleBytes(uint32_t ReadAddr,uint16_t  *pBuffer,uint16_t  NumToRead)      
{
    uint16_t  i;
    for(i = 0;i < NumToRead;i++)
    {
        pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);         // 读取2个字节.
        ReadAddr += 2;                                        // 偏移2个字节. 
    }
}
/**
*@brief   无校验写入（该函数不校验Flash地址是否可写，是否需要擦除，所以不能直接用于写操作）
*@param   WriteAddr : 起始地址
*         pBuffer   : 数据指针
*         NumToWrite: 半字(16位)数   
*@return  无
*/
void STMFLASH_Write_NoCheck(uint32_t WriteAddr,uint16_t  *pBuffer,uint16_t  NumToWrite)   
{                    
    uint16_t  i;
    for(i = 0;i < NumToWrite;i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,WriteAddr,pBuffer[i]);  // 写入flash FLASH_TYPEPROGRAM_HALFWORD是按照16bit写入的

        WriteAddr += 2;                              // 地址增加2.
    }  
} 
/**
*@brief     从指定地址开始写入指定长度的数据
*@param     WriteAddr : 起始地址(此地址必须为2的倍数!!)
*           pBuffer   : 数据指针
*           NumToWrite: 半字(16位)数(就是要写入的16位数据的个数.)
*@return    无
*/
int8_t STMFLASH_WriteMultipleBytes(uint32_t WriteAddr,uint16_t  *pBuffer,uint16_t  NumToWrite)
{
    uint32_t  PageAddr;                                                                                            // Flash页地址
    uint16_t  WordAddr;                                                                                            // 要写入的地址在Flash页中的位置(16位字计算)
    uint16_t  WordRemainder;                                                                                       // Flash页中的剩余地址(16位字计算)
    uint32_t  ShiftingAddr;                                                                                        // 去掉0X08000000后的地址
    uint16_t  i;    
    __disable_irq();
    // 验证是否能被2整除
    if (WriteAddr % 2 != 0) 
    {
        return -1;
    } 

    /* 当指定起始地址小于STM32_FLASH_BASE (0x0800000) 或者大于芯片本身的Flash容量时，写入地址无效，跳出函数 */
    if(WriteAddr < STM32_FLASH_BASE || (WriteAddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE)))
    {
        return -2;                                                                                                    // 非法地址
    }
    HAL_FLASH_Unlock(); 
    ShiftingAddr  = WriteAddr - STM32_FLASH_BASE;                                                                  // 实际偏移地址.要写入数据起始地址的位置
    PageAddr      =  ShiftingAddr / FLASH_PAGE_SIZE;                                                               // 要写入的地址所在的Flash页（0~256）
    WordAddr      = (ShiftingAddr % FLASH_PAGE_SIZE) / 2;                                                          // 在Flash页内的偏移(2个字节为基本单位.)
    WordRemainder = FLASH_PAGE_SIZE/2 - WordAddr;                                                                  // Flash页剩余空间大小
    if(NumToWrite <= WordRemainder)
    {
        WordRemainder = NumToWrite;                                                                                // 不大于该Flash页范围
    }
 
    while(1) 
    {
        STMFLASH_ReadMultipleBytes(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE,STMFLASH_BUF,FLASH_PAGE_SIZE/2);    // 读出整个Flash页的内容存放到STMFLASH_BUF中
        
        /* 查验数据，看flash页是否需要擦除 */
        for(i = 0;i < WordRemainder;i++)                                                                           // 校验数据
        {
            if(STMFLASH_BUF[WordAddr + i] != 0XFFFF)
            {
                break;                                                                                             // 需要擦除
            }     
        }
        /* 如果要写入数据的Flash页面上，所有的字都等于0XFFFF，那么在上面的循环之后，i = WordRemainder*/
        if(i < WordRemainder)                                                                                      // 需要擦除
        {
            //FLASH_ErasePage(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE);                                      // 擦除这个Flash页
            FLASH_EraseInitTypeDef FLASH_EraseInitType;
            FLASH_EraseInitType.TypeErase 	= FLASH_TYPEERASE_PAGES;
            FLASH_EraseInitType.PageAddress = PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE;
            FLASH_EraseInitType.NbPages 		= 1;
            uint32_t PageError = HAL_FLASH_GetError();
            HAL_FLASHEx_Erase(&FLASH_EraseInitType, &PageError);

            for(i = 0;i < WordRemainder;i++)                                                                       // 复制
            {
                STMFLASH_BUF[i + WordAddr] = pBuffer[i];
            }
            STMFLASH_Write_NoCheck(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE,STMFLASH_BUF,FLASH_PAGE_SIZE/2);    // 写入整个页
        }
        /* i = WordRemainder */
        else
        {
            STMFLASH_Write_NoCheck(WriteAddr,pBuffer,WordRemainder);                                               // 写已经擦除了的,直接写入扇区剩余区间. 
        }
 
        if(NumToWrite == WordRemainder)
        {
            break;                                                                                                 // 写入结束了
        }
        else                                                                                                       // 写入未结束
        {
            PageAddr++;                                                                                            // 页地址增加1
            WordAddr    = 0;                                                                                       // 偏移位置为0     
            pBuffer    += WordRemainder;                                                                           // 指针偏移
            WriteAddr  += WordRemainder*2;                                                                         // 写地址偏移(16位数据址,需要*2)
            NumToWrite -= WordRemainder;                                                                           // 字节(16位)数递减
            if(NumToWrite > (FLASH_PAGE_SIZE/2))
            {
                WordRemainder = FLASH_PAGE_SIZE/2;                                                                 // 下一个Flash页还是写不完
            }
            else WordRemainder = NumToWrite;                                                                       // 下一个Flash页可以写完了
        }    
    };  
    

	HAL_FLASH_Lock();
    __enable_irq();
    return 0;
    
}
// flash 操作函数 end
