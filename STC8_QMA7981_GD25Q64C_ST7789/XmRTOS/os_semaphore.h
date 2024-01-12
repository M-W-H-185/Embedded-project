


#ifndef __OS_SEMAPHORE__
#define __OS_SEMAPHORE__

#include "intrins.h"
#include "stdio.h"
#include "string.h"
#include "os_task.h"
#include "ringBuffer.h"

#define SEMAPHORE_TYPE_MUTEX               	( ( os_uint8_t ) 1 ) /* 互斥信号量 */
#define SEMAPHORE_TYPE_COUNTING  			( ( os_uint8_t ) 2 ) /* 计数信号量 */
#define SEMAPHORE_TYPE_BINARY    			( ( os_uint8_t ) 3 ) /* 二值信号量 */

typedef struct SemaphoreHandle_t
{
	os_uint8_t maxCount;							// 最大的计数值
	os_uint8_t initCount;							// 初始化的值
	os_uint8_t messagesWaiting;						// 信号计数器
	os_uint8_t osTaskIdsWaitToTakeList[MAX_TASKS];	// 等待获取型号量的任务id列表
	os_uint8_t type;								// 信号量类型
}SemaphoreHandle;


// 创建一个通用信号量函数
os_uint8_t semaphoreCreate(SemaphoreHandle *sem_handle, os_uint8_t maxCount, os_uint8_t initCount, os_uint8_t type);
// 释放一个信号量
os_uint8_t semaphoreGive(SemaphoreHandle *sem_handle);
// 获取一个信号量
os_uint8_t semaphoreTake(SemaphoreHandle *sem_handle, os_uint32_t waitTicks);



// 创建一个计数型信号量
#define semaphoreCreateCount(sem_handel,maxCount,initCount) semaphoreCreate(sem_handel,maxCount,initCount,SEMAPHORE_TYPE_COUNTING)
// 创建一个二值型信号量 默认值是0 需要先释放在获取
#define semaphoreCreateBinary(sem_handel) semaphoreCreate(sem_handel, 1, 0,SEMAPHORE_TYPE_BINARY)

#endif

