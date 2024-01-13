#include "hal_i2c.h"
#include "stc8g.h"

void Wait()
{
	while (!(I2CMSST & 0x40));
	I2CMSST &= ~0x40;
}

void Start()
{
	I2CMSCR = 0x01;                             //发送START命令
	Wait();
}

void SendData(char dat)
{
	I2CTXD = dat;                               //写数据到数据缓冲区
	I2CMSCR = 0x02;                             //发送SEND命令
	Wait();
}

void RecvACK()
{
	I2CMSCR = 0x03;                             //发送读ACK命令
	Wait();
}

char RecvData()
{
	I2CMSCR = 0x04;                             //发送RECV命令
	Wait();
	return I2CRXD;
}

//void SendACK()
//{
//	I2CMSST = 0x00;                             //设置ACK信号
//	I2CMSCR = 0x05;                             //发送ACK命令
//	Wait();
//}

void SendNAK()
{
	I2CMSST = 0x01;                             //设置NAK信号
	I2CMSCR = 0x05;                             //发送ACK命令
	Wait();
}

void Stop()
{
	I2CMSCR = 0x06;                             //发送STOP命令
	Wait();
}

/* IIC写一个字节 
   reg:寄存器地址
   dat:数据 */
void Write_Byte(unsigned char reg, unsigned char dat) 				 
{ 
	Start(); 
	SendData(0x24);//发送器件地址+写命令	
	RecvACK();
	SendData(reg);	//写寄存器地址
	RecvACK();		//等待应答 
	SendData(dat);//发送数据
	RecvACK();	 
	Stop();
}

/* IIC读一个字节  
   reg:寄存器地址 
   返回值:读到的数据 */
unsigned char Read_Byte(unsigned char reg)
{
	unsigned char res;
	
	Start(); 
	SendData(0x24);//发送器件地址+写命令	
	RecvACK();		//等待应答 
	SendData(reg);	//写寄存器地址
	RecvACK();		//等待应答
	Start(); 
	SendData(0x25);//发送器件地址+读命令	
	RecvACK();		//等待应答 
	res=RecvData();//读取数据,发送nACK 
	SendNAK();
	Stop();			//产生一个停止条件 

	return res;		
}

/* QMA7981初始化设置 */
void QMA7981Init(void)   //QMA7981初始化设置
{
	P_SW2 |= 0xB0; // EAXFR=1使能访问XFR  I2C_S=11把I2C引脚映射到P3.2 P3.3引脚

	I2CCFG = 0xFF;   //使能I2C主机模式
	I2CMSST = 0x00;
	Write_Byte(0x11,0xc0);       //将设备设置为active模式
}

