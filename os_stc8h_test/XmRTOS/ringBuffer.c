#include "ringBuffer.h"

/**
*	�������� ���һλ����
*	�ջ������� �ڳ�ʼ��ʱ����ָ���дָ�붼ָ��ͬһ��λ�ã�������������ʼλ�á���ʱ����ͨ���ж�ͷָ���βָ���Ƿ�������жϻ������Ƿ�Ϊ�ա�
*	���������� ��ָ�� > дָ���ʱ�� ��ʾ�������Ѿ����ˡ�
*	дָ���ǲ��ᵽ���������һλ�ģ��ڶ���ʱ��next_rָ�����һλ��ʱ�������������
*/
// ����һ�����λ������������
os_uint8_t ringbuffer_created(RingBufferHandle *ring_buffer, void *buff, os_uint8_t buff_size, os_uint8_t itemSize)
{

	
	ring_buffer->r_ 		= buff;           // Ĭ��ָ��buffͷ��
	ring_buffer->w_ 		= buff;           // Ĭ��ָ��buffͷ��
	ring_buffer->itemSize 	= itemSize;
	ring_buffer->buff_size 	= buff_size;
	ring_buffer->buff 		= buff;

	
	return RINGBUFF_SUCCESS;
}

// ������д д���������¿�ʼд����Ҫ�ȴ�����
os_uint8_t ringbuffer_write(RingBufferHandle *ring_buffer, void *_data)
{		
	os_uint8_t *next_w = ring_buffer->w_ + ring_buffer->itemSize;  // ��һ��д���λ��
	// ��һ��д���λ�õ��� ��ȡ ��λ�� ��ʾ����
    if (next_w == ring_buffer->r_) {
		// ���
        return RINGBUFF_WRITE_OVERFLOW; 

    }
	// дһ��д��ָ�� �������һλ��ʾ���
    if (next_w >= (os_uint8_t *)(ring_buffer->buff + ring_buffer->buff_size)) {
		// ���
        return RINGBUFF_WRITE_OVERFLOW; 
    }

    memcpy((void *)ring_buffer->w_, _data, ring_buffer->itemSize); // д������
    ring_buffer->w_ = next_w; // ����д�� βָ��
	return RINGBUFF_SUCCESS;
}

// �������� ����д����ͷ�� ���»ص�����0  �� ��ǰ�����´�дλ���غϱ�ʾ��
os_uint8_t ringbuffer_read(RingBufferHandle *ring_buffer, void *_data)
{

	// ��һ�ζ�ȡ��λ��
    os_uint8_t *next_r  = (os_uint8_t *)ring_buffer->r_ + ring_buffer->itemSize;  
	// ��һ�ζ�ָ�� > дָ�� ����  ��һ�� ���� ������β����ʱ�� �������
    if (
			next_r > ring_buffer->w_ && next_r == (os_uint8_t *)(ring_buffer->buff + (ring_buffer->buff_size ) )
		) 
	{
        memcpy(_data, ring_buffer->r_, ring_buffer->itemSize);
		// ����дָ�� ָ��������ʼλ ����0
        ring_buffer->r_ = ring_buffer->buff; 
        ring_buffer->w_ = ring_buffer->buff; 
        return RINGBUFF_READ_NULL; // �������Ҳ��ա��հ���
    }
	
	// ��ǰ��ȡλ�� �� ��һ��д��λ���غ� ��ʾ��
    if (ring_buffer->r_ == ring_buffer->w_) {
        return RINGBUFF_READ_NULL; // ��
    }
	
	// ��ȡ���ݲ����¶�ָ��
    memcpy(_data, ring_buffer->r_, ring_buffer->itemSize); // ��ȡ����
    ring_buffer->r_ = next_r; // ���¶�ָ��

	
	return RINGBUFF_SUCCESS;
}
