#include "os_config.h"
// 启动调度器。执行后创建最低优先级任务 idle
void os_task_start_scheduler(void);

// 创建一个任务
void os_task_create(void);
// 删除一个任务
void os_task_delete(void);
// 让一个任务进入阻塞态
void os_task_suspend(void);
// 任务延时函数
void os_task_delay(os_uint16_t ticks);
