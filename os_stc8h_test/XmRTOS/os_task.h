
#define MAX_TASKS 5       /*����۸���.�����ʵ��������һ��*/
#define MAX_TASK_DEP 50   /*���ջ��.��Ͳ�������2��,����ֵΪ12*/
typedef     unsigned char    os_uint8_t;	//  8 bits 

typedef     unsigned int     os_uint16_t;	// 16 bits 

typedef     unsigned long    os_uint32_t;	// 32 bits 
void os_start();
void os_delay(os_uint32_t tasks);
void os_task_create(void(*task)(void) ,os_uint8_t *tstack,int tid);
void os_switch();
void time_handleHook(void);
