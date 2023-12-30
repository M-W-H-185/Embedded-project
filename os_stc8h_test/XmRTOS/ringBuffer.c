#include "ringBuffer.h"

/**
*	�������� ���һλ����
*	�ջ������� �ڳ�ʼ��ʱ����ָ���дָ�붼ָ��ͬһ��λ�ã�������������ʼλ�á���ʱ����ͨ���ж�ͷָ���βָ���Ƿ�������жϻ������Ƿ�Ϊ�ա�
*	���������� ����дָ��ָ�� ����max-1��ʱ���������
*/

// ����һ�����λ������������
void ringbuffer_created(RingBufferHandle *ring_buffer, void *buff, os_uint8_t buff_size, os_uint8_t itemSize)
{

	
	ring_buffer->r_ 		= buff;           // Ĭ��ָ��buffͷ��
	ring_buffer->w_ 		= buff;           // Ĭ��ָ��buffͷ��
	ring_buffer->itemSize 	= itemSize;
	ring_buffer->buff_size 	= buff_size;
	ring_buffer->buff 		= buff;

	
	return;
}

// ������д д���������¿�ʼд����Ҫ�ȴ�����
void ringbuffer_write(RingBufferHandle *ring_buffer, void *_data)
{		
	os_uint8_t *next_w = ring_buffer->w_ + ring_buffer->itemSize;  // ��һ��д���λ��
	// ��һ��д���λ�õ��� ��ȡ ��λ�� ��ʾ����
    if (next_w == ring_buffer->r_) {
		// ����
        return; 

    }

    if (next_w >= (os_uint8_t *)(ring_buffer->buff + ring_buffer->buff_size)) {
		// ���
        return; 
    }

    memcpy((void *)ring_buffer->w_, _data, ring_buffer->itemSize); // д������
    ring_buffer->w_ = next_w; // ����д�� βָ��
	return;
}
// �������� ����д����ͷ�� ���»ص�����0  �� ��ǰ�����´�дλ���غϱ�ʾ��
void ringbuffer_read(RingBufferHandle *ring_buffer, void *_data)
{

	// ��һ�ζ�ȡ��λ��
    os_uint8_t *next_r = (os_uint8_t *)ring_buffer->r_ + ring_buffer->itemSize;  
	// ��ȡ��ͷ�ˡ������λ�ú�дλ�ö��� ������������һλ-1  ��������ˡ�����һ�����ȥ ����0
    if (
			next_r >= (os_uint8_t *)(ring_buffer->buff + (ring_buffer->buff_size - 1) )
				&& 
			ring_buffer->w_ >= (os_uint8_t *)(ring_buffer->buff + (ring_buffer->buff_size - 1) )
		) 
	{
        memcpy(_data, ring_buffer->r_, ring_buffer->itemSize);
		// ����дָ�� ָ��������ʼλ ����0
        ring_buffer->r_ = ring_buffer->buff; 
        ring_buffer->w_ = ring_buffer->buff; 
        return; // ���״̬
    }
	
	// ��ǰ��ȡλ�� �� ��һ��д��λ���غ� ��ʾ��
    if (ring_buffer->r_ == ring_buffer->w_) {
        return; // ��
    }
	
	// ��ȡ���ݲ����¶�ָ��
    memcpy(_data, ring_buffer->r_, ring_buffer->itemSize); // ��ȡ����
    ring_buffer->r_ = next_r; // ���¶�ָ��

	
	return;
}
