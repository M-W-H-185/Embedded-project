
#define MAX_TASKS 5       /*任务槽个数.必须和实际任务数一至*/
#define MAX_TASK_DEP 50   /*最大栈深.最低不得少于2个,保守值为12*/
typedef     unsigned char    os_uint8_t;	//  8 bits 

typedef     unsigned int     os_uint16_t;	// 16 bits 

typedef     unsigned long    os_uint32_t;	// 32 bits 
void os_start();
void os_delay(os_uint32_t tasks);
void os_task_create(void(*task)(void) ,os_uint8_t *tstack,int tid);
void os_switch();
void time_handleHook(void);
