#include "hal_i2c.h"
#include "stc8g.h"
void I2C_Isr() interrupt 24
{

    if (I2CMSST & 0x40)
    {
        I2CMSST &= ~0x40;                       //���жϱ�־
    }

}
// ��ȡ����״̬ 0 ���� 1æµ
uint8_t hal_I2cGetIdleState(void)
{
	// I2CMSST & 0x40 bit6Ϊ0��ʱ�����ûִ���꣬bit6Ϊ1��ʱ��Ϊ1��ʱ�����ִ������
	if((I2CMSST & 0x40) == 1)
	{
		I2CMSST &= ~0x40;	// ÿ��I2CMSCRִ�������Ҫ���set0
		return 0;
	}
	else
	{
		return 1;
	}
	return 1;
	
}
// ������ʼ�ź�
uint8_t hal_I2cStart(void)
{
	I2CMSCR = 0X01;
	while(hal_I2cGetIdleState()==1);	// �ȴ����ͳɹ�
	return 0;
}

// ����ֹͣ�ź�
uint8_t hal_I2cStop(void)
{
	I2CMSCR = 0X06;
	while(hal_I2cGetIdleState()==1);	// �ȴ����ͳɹ�
	return 0;
}
// ����byte
uint8_t hal_I2cSendByte(uint8_t _data)
{
	I2CTXD = _data; // �������
	I2CMSCR = 0x02;	// ������������
	while(hal_I2cGetIdleState()==1);	// �ȴ����ͳɹ�
	return 0 ;
}
// ��ȡһ���ֽ�
uint8_t hal_I2cReadByteData(void)
{
		uint8_t temp = 0;
		I2CMSCR = 0x04;	// ��ȡ��������
		while(hal_I2cGetIdleState()==1);		// �ȴ����ͳɹ�
		temp = I2CRXD;
		return temp;
}

// ����һ��nack
uint8_t hal_I2cSendNACK(void)
{
	I2CMSST = 0X01;
	I2CMSCR = 0X05;
	while(hal_I2cGetIdleState()==1);	// �ȴ����ͳɹ�
	return 0 ;
}
void hal_I2cRendACK()
{
	I2CMSCR = 0x03;                             //���Ͷ�ACK����
	while(hal_I2cGetIdleState()==1);	// �ȴ����ͳɹ�
}

void hal_I2cInit(void)
{
	P_SW2 |= 0x30;	// bit45д��11 I2C���ܽ�ѡ��λ 
	I2CCFG |= 0xC0;	// 1100 0000 ����i2c����Ϊ����ģʽ
	I2CCFG |= 0xD;	// 1100 1101 ���������ٶ�
	I2CMSCR|= 0x80;	// 1000 0000 ���������ж� 
}
