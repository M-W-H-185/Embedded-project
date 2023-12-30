#ifndef __OS_QUEUE_H__
#define __OS_QUEUE_H__
#include "os_task.h"



typedef struct RingBufferHandle_t
{
	os_uint8_t *r_;			// ��һ��Ҫ����ȡ��Ԫ��
	os_uint8_t *w_;			// ��һ��Ҫ��д���λ��
	os_uint8_t 	buff_size;	// ����������
	os_uint8_t 	itemSize;	// ����������Ĵ�С
	void 		*buff;		// ������
}RingBufferHandle;

enum RingBuffRetuenState{
	RINGBUFF_SUCCESS 			= 1,	// �����������ɹ�
	RINGBUFF_WRITE_OVERFLOW 	= 2,	// д�����
	RINGBUFF_READ_NULL 			= 3,	// ��ȡʧ�� ������������
};

os_uint8_t ringbuffer_created(RingBufferHandle *ring_buffer, void *buff, os_uint8_t buff_size, os_uint8_t itemSize);
os_uint8_t ringbuffer_write(RingBufferHandle *ring_buffer, void *_data);
os_uint8_t ringbuffer_read(RingBufferHandle *ring_buffer, void *_data);

#endif
