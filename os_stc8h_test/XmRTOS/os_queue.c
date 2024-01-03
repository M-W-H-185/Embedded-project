#include "os_queue.h"
#include "ringBuffer.h"


// 创建一个队列
os_uint8_t os_queueCreate(QueueHandle *queue_handle, void *buff, os_uint8_t buff_size, os_uint8_t itemSize)
{
	RingBufferHandle  ringbuffer ;
	
	memset(buff, 0, (buff_size * itemSize)); 		// 写入0
	memset(queue_handle, 0, sizeof(QueueHandle)); 	// 写入0

	// 创建一个环形缓冲区
	ringbuffer_created(&ringbuffer, buff, buff_size, itemSize);
	// 指向环形缓冲区
	queue_handle->rb_handle = ringbuffer;
	
	return 0;
}
// 写入队列
os_uint8_t os_queueSend(QueueHandle *queue_handle, void *_data, os_uint32_t waitTicks)
{
	
	os_uint8_t i = 0;
	os_uint8_t ret = ringbuffer_write(&(queue_handle->rb_handle), _data);
	EA = 0;
	
	

	if(ret == RINGBUFF_SUCCESS )// 写入成功的时候
	{
		// 写入队列来唤醒等待读取的任务
		for(i = 0; i < MAX_TASKS; i++)
		{
			// 唤醒一个等待读取任务
			if(queue_handle->osTaskIdsWaitToReadList[i] == 1 && tcb_list[i].os_status_type == OS_BLOCKED)
			{
				// 将这个任务设置为就绪态 延时清0
				tcb_list[i].os_status_type 	= OS_READY;
				tcb_list[i].delay_tick		= 0;
				// 等待 读取任务列表将他设0
				queue_handle->osTaskIdsWaitToReadList[i] = 0;
				break;
			}
		}
	}
	
	// 如果写入失败 并且有阻塞时间    将当前任务放入等待发送列表内
	else if(ret != RINGBUFF_SUCCESS && waitTicks > 0 )
	{
		// 并将当前任务id 放入 osTaskIdsWaitToSendList[tackid] = 1 表示当前任务是被这个队列堵塞了
		// 读任务来唤醒写任务
		// 写任务来唤醒读任务
		queue_handle->osTaskIdsWaitToSendList[task_id] = 1;
		EA = 1;
		os_delay(waitTicks);
	}
	EA = 1;
	return ret;
}

// 读取队列
os_uint8_t os_queueRead(QueueHandle *queue_handle, void *_data, os_uint32_t waitTicks)
{
	os_uint8_t i = 0;
	os_uint8_t ret = ringbuffer_read(&(queue_handle->rb_handle), _data);
	EA = 0;
	if(ret == RINGBUFF_SUCCESS)	// 读取成功的时候
	{
		// 读取队列 来唤醒等待 发送的任务
		for(i = 0; i < MAX_TASKS; i++)
		{
			// 唤醒一个等待发送任务
			if(queue_handle->osTaskIdsWaitToSendList[i] == 1 && tcb_list[i].os_status_type == OS_BLOCKED)
			{
				// 将这个任务设置为就绪态 延时清0
				tcb_list[i].os_status_type 	= OS_READY;
				tcb_list[i].delay_tick		= 0;
				// 等待 发送任务列表将他设0
				queue_handle->osTaskIdsWaitToSendList[i] = 0;
				break;
			}
		}
	}
	
	
	// 如果读取失败 并且有阻塞时间    将当前任务放入等待读取列表内
	else if(ret != RINGBUFF_SUCCESS && waitTicks > 0 )
	{
		// 并将当前任务id 放入 osTaskIdsWaitToReadList[tackid] = 1 表示当前任务是被这个队列堵塞了
		// 读任务来唤醒写任务
		// 写任务来唤醒读任务
		queue_handle->osTaskIdsWaitToReadList[task_id] = 1;
		EA = 1;
		os_delay(waitTicks);
	}
	EA = 1;
	return ret;
}








