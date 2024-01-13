#ifndef __QMA7981_H__
#define __QMA7981_H__
#include "hal_i2c.h"
#include "intrins.h"
#include "stdio.h"
#include "string.h"
typedef struct QMA7981_XYZ_t
{
	short x;
	short y; 
	short z; 

}QMA7981_XYZ;

unsigned char Read_Byte(unsigned char reg); 
QMA7981_XYZ qm7891_read_xyz(void);


void QMA7981Init(void);
#endif

