#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"

/*--------------------------------------------------------
| @Description: OS constant define types                 |
--------------------------------------------------------*/


typedef  bit     os_bool_t;

/*--------------------------------------------------------
| @Description: �з�������                               |
--------------------------------------------------------*/

typedef    signed char     os_int8_t;	//  8 bits 

typedef    signed int      os_int16_t;	// 16 bits 

typedef    signed long     os_int32_t;	// 32 bits 



/*--------------------------------------------------------
| @Description: �޷�������                               |
--------------------------------------------------------*/

typedef     unsigned char    os_uint8_t;	//  8 bits 

typedef     unsigned int     os_uint16_t;	// 16 bits 

typedef     unsigned long    os_uint32_t;	// 32 bits 
enum OS_TASK_STATUS_TYPE
{
	OS_READY      = 1,	// ����̬
	OS_RUNING     = 2,	// ����̬
	OS_BLOCKED    = 3,	// ����̬
	OS_SUSPENDED  = 4	// ֹ̬ͣ

};

// ������ƿ�
typedef struct os_tcb_t
{
	os_uint8_t 			sp;					// sp ��ջָ��洢
	os_uint32_t 		delay_tick;			// ��ʱ�δ���
	os_uint8_t 			os_status_type;		// ����״̬
};



