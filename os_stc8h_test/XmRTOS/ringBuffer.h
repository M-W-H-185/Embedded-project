#ifndef __OS_QUEUE_H__
#define __OS_QUEUE_H__
#include "os_task.h"


// �ջ������� �ڳ�ʼ��ʱ��ͷָ���βָ�붼ָ��ͬһ��λ�ã�������������ʼλ�á���ʱ����ͨ���ж�ͷָ���βָ���Ƿ�������жϻ������Ƿ�Ϊ�ա�
// ���������� ��βָ���ƶ���ͷָ���ǰһ��λ��ʱ��˵�����������������û��Ԥ��һ��λ�ã�ͷβָ����ص����޷�׼ȷ�ж��ǿջ�������
// ��Ϣ����
typedef struct RingBufferHandle_t
{
	os_uint8_t *r_;			// ��һ��Ҫ����ȡ��Ԫ��
	os_uint8_t *w_;			// ��һ��Ҫ��д���λ��
	os_uint8_t 	buff_size;	// ����������
	os_uint8_t 	itemSize;	// ����������Ĵ�С
	void 		*buff;		// ������
}RingBufferHandle;


void ringbuffer_created(RingBufferHandle *ring_buffer, void *buff, os_uint8_t buff_size, os_uint8_t itemSize);
void ringbuffer_write(RingBufferHandle *ring_buffer, void *_data);
void ringbuffer_read(RingBufferHandle *ring_buffer, void *_data);

#endif
