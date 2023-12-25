#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"

/*--------------------------------------------------------
| @Description: OS constant define types                 |
--------------------------------------------------------*/


typedef  bit     os_bool_t;

/*--------------------------------------------------------
| @Description: 有符号整形                               |
--------------------------------------------------------*/

typedef    signed char     os_int8_t;	//  8 bits 

typedef    signed int      os_int16_t;	// 16 bits 

typedef    signed long     os_int32_t;	// 32 bits 



/*--------------------------------------------------------
| @Description: 无符号整形                               |
--------------------------------------------------------*/

typedef     unsigned char    os_uint8_t;	//  8 bits 

typedef     unsigned int     os_uint16_t;	// 16 bits 

typedef     unsigned long    os_uint32_t;	// 32 bits 
enum OS_TASK_STATUS_TYPE
{
	OS_READY      = 1,	// 就绪态
	OS_RUNING     = 2,	// 运行态
	OS_BLOCKED    = 3,	// 阻塞态
	OS_SUSPENDED  = 4	// 停止态

};

// 任务控制块
typedef struct os_tcb_t
{
	os_uint8_t 			sp;					// sp 堆栈指针存储
	os_uint32_t 		delay_tick;			// 延时滴答数
	os_uint8_t 			os_status_type;		// 任务状态
};



