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
	
	os_uint8_t i = 0;
	os_uint8_t ret = ringbuffer_write(&(queue_handle->rb_handle), _data);
	EA = 0;
	
	

	if(ret == RINGBUFF_SUCCESS )// д��ɹ���ʱ��
	{
		// д����������ѵȴ���ȡ������
		for(i = 0; i < MAX_TASKS; i++)
		{
			// ����һ���ȴ���ȡ����
			if(queue_handle->osTaskIdsWaitToReadList[i] == 1 && tcb_list[i].os_status_type == OS_BLOCKED)
			{
				// �������������Ϊ����̬ ��ʱ��0
				tcb_list[i].os_status_type 	= OS_READY;
				tcb_list[i].delay_tick		= 0;
				// �ȴ� ��ȡ�����б�����0
				queue_handle->osTaskIdsWaitToReadList[i] = 0;
				break;
			}
		}
	}
	
	// ���д��ʧ�� ����������ʱ��    ����ǰ�������ȴ������б���
	else if(ret != RINGBUFF_SUCCESS && waitTicks > 0 )
	{
		// ������ǰ����id ���� osTaskIdsWaitToSendList[tackid] = 1 ��ʾ��ǰ�����Ǳ�������ж�����
		// ������������д����
		// д���������Ѷ�����
		queue_handle->osTaskIdsWaitToSendList[task_id] = 1;
		EA = 1;
		os_delay(waitTicks);
	}
	EA = 1;
	return ret;
}

// ��ȡ����
os_uint8_t os_queueRead(QueueHandle *queue_handle, void *_data, os_uint32_t waitTicks)
{
	os_uint8_t i = 0;
	os_uint8_t ret = ringbuffer_read(&(queue_handle->rb_handle), _data);
	EA = 0;
	if(ret == RINGBUFF_SUCCESS)	// ��ȡ�ɹ���ʱ��
	{
		// ��ȡ���� �����ѵȴ� ���͵�����
		for(i = 0; i < MAX_TASKS; i++)
		{
			// ����һ���ȴ���������
			if(queue_handle->osTaskIdsWaitToSendList[i] == 1 && tcb_list[i].os_status_type == OS_BLOCKED)
			{
				// �������������Ϊ����̬ ��ʱ��0
				tcb_list[i].os_status_type 	= OS_READY;
				tcb_list[i].delay_tick		= 0;
				// �ȴ� ���������б�����0
				queue_handle->osTaskIdsWaitToSendList[i] = 0;
				break;
			}
		}
	}
	
	
	// �����ȡʧ�� ����������ʱ��    ����ǰ�������ȴ���ȡ�б���
	else if(ret != RINGBUFF_SUCCESS && waitTicks > 0 )
	{
		// ������ǰ����id ���� osTaskIdsWaitToReadList[tackid] = 1 ��ʾ��ǰ�����Ǳ�������ж�����
		// ������������д����
		// д���������Ѷ�����
		queue_handle->osTaskIdsWaitToReadList[task_id] = 1;
		EA = 1;
		os_delay(waitTicks);
	}
	EA = 1;
	return ret;
}








