#include "hal_i2c.h"
#include "stc8g.h"

void Wait()
{
	while (!(I2CMSST & 0x40));
	I2CMSST &= ~0x40;
}

void Start()
{
	I2CMSCR = 0x01;                             //����START����
	Wait();
}

void SendData(char dat)
{
	I2CTXD = dat;                               //д���ݵ����ݻ�����
	I2CMSCR = 0x02;                             //����SEND����
	Wait();
}

void RecvACK()
{
	I2CMSCR = 0x03;                             //���Ͷ�ACK����
	Wait();
}

char RecvData()
{
	I2CMSCR = 0x04;                             //����RECV����
	Wait();
	return I2CRXD;
}

//void SendACK()
//{
//	I2CMSST = 0x00;                             //����ACK�ź�
//	I2CMSCR = 0x05;                             //����ACK����
//	Wait();
//}

void SendNAK()
{
	I2CMSST = 0x01;                             //����NAK�ź�
	I2CMSCR = 0x05;                             //����ACK����
	Wait();
}

void Stop()
{
	I2CMSCR = 0x06;                             //����STOP����
	Wait();
}

/* IICдһ���ֽ� 
   reg:�Ĵ�����ַ
   dat:���� */
void Write_Byte(unsigned char reg, unsigned char dat) 				 
{ 
	Start(); 
	SendData(0x24);//����������ַ+д����	
	RecvACK();
	SendData(reg);	//д�Ĵ�����ַ
	RecvACK();		//�ȴ�Ӧ�� 
	SendData(dat);//��������
	RecvACK();	 
	Stop();
}

/* IIC��һ���ֽ�  
   reg:�Ĵ�����ַ 
   ����ֵ:���������� */
unsigned char Read_Byte(unsigned char reg)
{
	unsigned char res;
	
	Start(); 
	SendData(0x24);//����������ַ+д����	
	RecvACK();		//�ȴ�Ӧ�� 
	SendData(reg);	//д�Ĵ�����ַ
	RecvACK();		//�ȴ�Ӧ��
	Start(); 
	SendData(0x25);//����������ַ+������	
	RecvACK();		//�ȴ�Ӧ�� 
	res=RecvData();//��ȡ����,����nACK 
	SendNAK();
	Stop();			//����һ��ֹͣ���� 

	return res;		
}

QMA7981_XYZ qm7891_read_xyz(void)
{
	QMA7981_XYZ q_xyz;
	
	short databuff[6] = {0};
	short x = 0;
	short y = 0;
	short z = 0;
	databuff[0] = Read_Byte(0x01);  //X���λ
	databuff[1] = Read_Byte(0x02);  //X���λ
	databuff[2] = Read_Byte(0x03);  //Y���λ
	databuff[3] = Read_Byte(0x04);  //Y���λ
	databuff[4] = Read_Byte(0x05);  //Y���λ
	databuff[5] = Read_Byte(0x06);  //Y���λ
	
	x  = (short)((databuff[1]<<8) |( databuff[0]));
	y  = (short)((databuff[3]<<8) |( databuff[2]));
	z  = (short)((databuff[5]<<8) |( databuff[4]));

	q_xyz.x = x>>2;     //X��ԭʼ����
	q_xyz.y = y>>2;     //Y��ԭʼ����
	q_xyz.z = z>>2;     //Z��ԭʼ����
	return q_xyz;
}

/* QMA7981��ʼ������ */
void QMA7981Init(void)   //QMA7981��ʼ������
{
	P_SW2 |= 0xB0; // EAXFR=1ʹ�ܷ���XFR  I2C_S=11��I2C����ӳ�䵽P3.2 P3.3����

	I2CCFG = 0xFF;   //ʹ��I2C����ģʽ
	I2CMSST = 0x00;
	Write_Byte(0x11,0xc0);       //���豸����Ϊactiveģʽ
}

