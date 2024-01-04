#ifndef __OS_QUEUE_H__
#define __OS_QUEUE_H__

#include "intrins.h"
#include "stdio.h"
#include "string.h"
#include "os_task.h"
#include "ringBuffer.h"


typedef struct QueueHandle_t
{
	RingBufferHandle  rb_handle;							// 环形缓冲区操作句柄
	os_uint8_t		  osTaskIdsWaitToSendList[MAX_TASKS];	// 等待发送的任务id列表。缓冲区满后将id存进去
	os_uint8_t		  osTaskIdsWaitToReadList[MAX_TASKS];	// 等待读取的任务id列表。缓冲区读取不到数据的时候将id存进去
}QueueHandle;
os_uint8_t os_queueCreate(QueueHandle *queue_handle, void *buff, os_uint8_t buff_size, os_uint8_t itemSize);
os_uint8_t os_queueSend(QueueHandle *queue_handle, void *_data, os_uint32_t waitTicks);
os_uint8_t os_queueRead(QueueHandle *queue_handle, void *_data, os_uint32_t waitTicks);

#endif

