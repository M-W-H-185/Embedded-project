#include "os_queue.h"
#include "ringBuffer.h"


typedef struct QueueHandle_t
{
	RingBufferHandle  *rb_handle;							// ���λ������������
	os_uint8_t		  osTaskIdsWaitToSendList[MAX_TASKS];	// �ȴ����͵�����id�б�����������id���ȥ
	os_uint8_t		  osTaskIdsWaitToReadList[MAX_TASKS];	// �ȴ���ȡ������id�б���������ȡ�������ݵ�ʱ��id���ȥ

}QueueHandle;


// ����һ������
os_uint8_t os_queueCreate(QueueHandle *queue_handle, void *buff, os_uint8_t buff_size, os_uint8_t itemSize)
{
	RingBufferHandle  *ringbuffer ;
	// ����һ�����λ�����
	ringbuffer_created(ringbuffer, buff, buff_size, itemSize);
	// ָ���λ�����
	queue_handle->rb_handle = ringbuffer;
	
	return 0;
}
