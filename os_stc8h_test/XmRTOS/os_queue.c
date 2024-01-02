#include "os_queue.h"
#include "ringBuffer.h"


// ����һ������
os_uint8_t os_queueCreate(QueueHandle *queue_handle, void *buff, os_uint8_t buff_size, os_uint8_t itemSize)
{
	RingBufferHandle  ringbuffer ;
	
	memset(buff, 0, (buff_size * itemSize)); 		// д��0
	memset(queue_handle, 0, sizeof(QueueHandle)); 	// д��0

	// ����һ�����λ�����
	ringbuffer_created(&ringbuffer, buff, buff_size, itemSize);
	// ָ���λ�����
	queue_handle->rb_handle = ringbuffer;
	
	return 0;
}
// д�����
os_uint8_t os_queueSend(QueueHandle *queue_handle, void *_data, os_uint32_t waitTicks)
{
	
	os_uint8_t ret = ringbuffer_write(&(queue_handle->rb_handle), _data);
	
	
	
	// ���д��ʧ�� ����������ʱ��    ����ǰ�������ȴ������б���
	if(ret != RINGBUFF_SUCCESS && waitTicks > 0 )
	{
		// ������ǰ����id ���� osTaskIdsWaitToSendList[tackid] = 1 ��ʾ��ǰ�����Ǳ�������ж�����
		// ������������д����
		// д���������Ѷ�����
		queue_handle->osTaskIdsWaitToSendList[task_id] = 1;
		os_delay(waitTicks);
	}
	else	// д��ɹ���ʱ��
	{
		// д����������ѵȴ���ȡ������
		
	}
	
	return ret;
}










