#include "hal_i2c.h"
#include "QMA7981.h"



QMA7981_XYZ qm7891_read_xyz(void)
{
	QMA7981_XYZ q_xyz;
	
	short databuff[6] = {0};
	short x = 0;
	short y = 0;
	short z = 0;
	databuff[0] = Read_Byte(0x01);  //X轴低位
	databuff[1] = Read_Byte(0x02);  //X轴高位
	databuff[2] = Read_Byte(0x03);  //Y轴低位
	databuff[3] = Read_Byte(0x04);  //Y轴高位
	databuff[4] = Read_Byte(0x05);  //Y轴低位
	databuff[5] = Read_Byte(0x06);  //Y轴高位
	
	x  = (short)((databuff[1]<<8) |( databuff[0]));
	y  = (short)((databuff[3]<<8) |( databuff[2]));
	z  = (short)((databuff[5]<<8) |( databuff[4]));

	q_xyz.x = x>>2;     //X轴原始数据
	q_xyz.y = y>>2;     //Y轴原始数据
	q_xyz.z = z>>2;     //Z轴原始数据
	return q_xyz;
}

/* QMA7981初始化设置 */
void QMA7981Init(void)   //QMA7981初始化设置
{
	i2c_init();
	Write_Byte(0x11,0xc0);       //将设备设置为active模式
}
