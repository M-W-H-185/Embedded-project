#include "os_config.h"
// ������������ִ�к󴴽�������ȼ����� idle
void os_task_start_scheduler(void);

// ����һ������
void os_task_create(void);
// ɾ��һ������
void os_task_delete(void);
// ��һ�������������̬
void os_task_suspend(void);
// ������ʱ����
void os_task_delay(os_uint16_t ticks);
