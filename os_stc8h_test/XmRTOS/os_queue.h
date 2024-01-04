#ifndef __OS_QUEUE_H__
#define __OS_QUEUE_H__

#include "intrins.h"
#include "stdio.h"
#include "string.h"
#include "os_task.h"
#include "ringBuffer.h"


typedef struct QueueHandle_t
{
	RingBufferHandle  rb_handle;							// ���λ������������
	os_uint8_t		  osTaskIdsWaitToSendList[MAX_TASKS];	// �ȴ����͵�����id�б�����������id���ȥ
	os_uint8_t		  osTaskIdsWaitToReadList[MAX_TASKS];	// �ȴ���ȡ������id�б���������ȡ�������ݵ�ʱ��id���ȥ
}QueueHandle;
os_uint8_t os_queueCreate(QueueHandle *queue_handle, void *buff, os_uint8_t buff_size, os_uint8_t itemSize);
os_uint8_t os_queueSend(QueueHandle *queue_handle, void *_data, os_uint32_t waitTicks);
os_uint8_t os_queueRead(QueueHandle *queue_handle, void *_data, os_uint32_t waitTicks);

#endif

