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
	
	os_uint8_t ret = ringbuffer_write(&(queue_handle->rb_handle), _data);
	
	
	
	// 如果写入失败 并且有阻塞时间    将当前任务放入等待发送列表内
	if(ret != RINGBUFF_SUCCESS && waitTicks > 0 )
	{
		// 并将当前任务id 放入 osTaskIdsWaitToSendList[tackid] = 1 表示当前任务是被这个队列堵塞了
		// 读任务来唤醒写任务
		// 写任务来唤醒读任务
		queue_handle->osTaskIdsWaitToSendList[task_id] = 1;
		os_delay(waitTicks);
	}
	else	// 写入成功的时候
	{
		// 写入队列来唤醒等待读取的任务
		
	}
	
	return ret;
}










