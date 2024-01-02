#include "os_queue.h"
#include "ringBuffer.h"


typedef struct QueueHandle_t
{
	RingBufferHandle  *rb_handle;							// 环形缓冲区操作句柄
	os_uint8_t		  osTaskIdsWaitToSendList[MAX_TASKS];	// 等待发送的任务id列表。缓冲区满后将id存进去
	os_uint8_t		  osTaskIdsWaitToReadList[MAX_TASKS];	// 等待读取的任务id列表。缓冲区读取不到数据的时候将id存进去

}QueueHandle;


// 创建一个队列
os_uint8_t os_queueCreate(QueueHandle *queue_handle, void *buff, os_uint8_t buff_size, os_uint8_t itemSize)
{
	RingBufferHandle  *ringbuffer ;
	// 创建一个环形缓冲区
	ringbuffer_created(ringbuffer, buff, buff_size, itemSize);
	// 指向环形缓冲区
	queue_handle->rb_handle = ringbuffer;
	
	return 0;
}
