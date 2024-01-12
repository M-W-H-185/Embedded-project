#include "os_semaphore.h"





// 创建一个计数型信号量，返回它的句柄。
os_uint8_t semaphoreCreate(SemaphoreHandle *sem_handle, os_uint8_t maxCount, os_uint8_t initCount, os_uint8_t type)
{
	sem_handle->maxCount 		= maxCount;		
	sem_handle->initCount 		= initCount;
	sem_handle->messagesWaiting 	= initCount;	//初始化计数值
	sem_handle->type 			= type;


	
	memset(sem_handle->osTaskIdsWaitToTakeList, 0, (MAX_TASKS * sizeof(os_uint8_t)));
	return RET_SUCCESS;
}

// 释放一个信号量
os_uint8_t semaphoreGive(SemaphoreHandle *sem_handle)
{
	os_uint8_t ret 	= RET_ERROR;
	os_uint8_t i 	= 0;
	EA = 0;
	// 二值型
	if(sem_handle->type == SEMAPHORE_TYPE_BINARY)
	{
		sem_handle->messagesWaiting = 1;
		
		ret = RET_SUCCESS;
	}
	// 计数型
	if(sem_handle->type == SEMAPHORE_TYPE_COUNTING)
	{
		
		if(sem_handle->messagesWaiting < sem_handle->maxCount)
		{
			sem_handle->messagesWaiting  = sem_handle->messagesWaiting  + 1;
			
			ret = RET_SUCCESS;
		}
	}
	// 需要唤醒在这个型号量等待的任务 
	
	for(i = 0; i < MAX_TASKS; i++)
	{
		// 唤醒一个等待发送任务
		if(sem_handle->osTaskIdsWaitToTakeList[i] == 1 && tcb_list[i].os_status_type == OS_BLOCKED)
		{
			// 将这个任务设置为就绪态 延时清0
			tcb_list[i].os_status_type 	= OS_READY;
			tcb_list[i].delay_tick		= 0;
			// 等待 发送任务列表将他设0
			sem_handle->osTaskIdsWaitToTakeList[i] = 0;
			break;
		}
	}
	EA = 1;
	return ret;
}


// 获取一个信号量
os_uint8_t semaphoreTake(SemaphoreHandle *sem_handle, os_uint32_t waitTicks)
{
	os_uint8_t ret 	= RET_ERROR;
	EA = 0;
	// 如果计数器大于 0
	if(sem_handle->messagesWaiting > 0)
	{
		// 计数器减一 并 退出临界区 返回
		sem_handle->messagesWaiting = sem_handle->messagesWaiting - 1;
		EA = 1;
		ret =  RET_SUCCESS;
		return ret;
	}
	else
	{
		// 将当前获取最高信号量 的任务放进去 等待列表中。释放信号量的时候环形
		sem_handle->osTaskIdsWaitToTakeList[task_id] = 1;
		// EA = 1;
		os_delay(waitTicks);
		ret =  RET_ERROR;
		return ret;
	}

	EA = 1;
	return ret;
}







