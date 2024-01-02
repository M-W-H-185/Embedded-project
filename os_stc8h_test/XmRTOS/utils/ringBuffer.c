#include "ringBuffer.h"

/**
*	�������� ���һλ����
*	�ջ������� �ڳ�ʼ��ʱ����ָ���дָ�붼ָ��ͬһ��λ�ã�������������ʼλ�á���ʱ����ͨ���ж�ͷָ���βָ���Ƿ�������жϻ������Ƿ�Ϊ�ա�
*	���������� ��ָ�� > дָ���ʱ�� ��ʾ�������Ѿ����ˡ�
*	дָ���ǲ��ᵽ���������һλ�ģ��ڶ���ʱ��next_rָ�����һλ��ʱ�������������
*/


/***********************************************************************
*	��������������һ�����λ������������
*	���������
*				ring_buffer	: �������������ָ��
*				buff		: ��������
*				buff_size	: ��������
*				itemSize	: ��������ÿһ��Ԫ�صĴ�С
* ���ز�����
*				os_uint8_t	: RINGBUFF_SUCCESS ֻ���سɹ�
***********************************************************************/
os_uint8_t ringbuffer_created(RingBufferHandle *ring_buffer, void *buff, os_uint8_t buff_size, os_uint8_t itemSize)
{

	
	ring_buffer->r_ 		= buff;           // Ĭ��ָ��buffͷ��
	ring_buffer->w_ 		= buff;           // Ĭ��ָ��buffͷ��
	ring_buffer->itemSize 	= itemSize;
	ring_buffer->buff_size 	= buff_size;
	ring_buffer->buff 		= buff;

	
	return RINGBUFF_SUCCESS;
}
/***********************************************************************
*	����������  ������д д���������¿�ʼд����Ҫ�ȴ�����
*	���������
*				ring_buffer	: �������������ָ��
*				_data		: д���������
* ���ز�����
*				os_uint8_t	: 
*								RINGBUFF_WRITE_OVERFLOW = ���������
*								RINGBUFF_SUCCESS		= д��ɹ� 
***********************************************************************/
os_uint8_t ringbuffer_write(RingBufferHandle *ring_buffer, void *_data)
{		
	os_uint8_t *next_w = ring_buffer->w_ + ring_buffer->itemSize;  // ��һ��д���λ��

	
	// ��һ��д���λ�õ��� ��ȡ ��λ�� ��ʾ����
    if (next_w == ring_buffer->r_) {
		// ���
        return RINGBUFF_WRITE_OVERFLOW; 

    }

	// дһ��д��ָ�� �������һλ��ʾ���
    if (next_w >= (os_uint8_t *)(ring_buffer->buff + ( (ring_buffer->buff_size - 1) * ring_buffer->itemSize ) )) {
		// ���
        return RINGBUFF_WRITE_OVERFLOW; 
    }

    memcpy((void *)ring_buffer->w_, _data, ring_buffer->itemSize); // д������
    ring_buffer->w_ = next_w; // ����д�� βָ��
	return RINGBUFF_SUCCESS;
}

/***********************************************************************
*	����������  �������� ����д����ͷ�� ���»ص�����0
*	���������
*				ring_buffer	: �������������ָ��
*				_data		: ��ȡ�����ݵ�ָ��
* ���ز�����
*				os_uint8_t	: 
*								RINGBUFF_READ_NULL		= ������������
*								RINGBUFF_SUCCESS		= д��ɹ� 
***********************************************************************/
os_uint8_t ringbuffer_read(RingBufferHandle *ring_buffer, void *_data)
{

	// ��һ�ζ�ȡ��λ��
    os_uint8_t *next_r  = (os_uint8_t *)ring_buffer->r_ + ring_buffer->itemSize;  
	// ��һ�ζ�ָ�� > дָ�� ����  ��һ�� ���� ������β����ʱ�� �������
    if (
			next_r > ring_buffer->w_ && next_r == (os_uint8_t *)( ring_buffer->buff + ( (ring_buffer->buff_size - 1) * ring_buffer->itemSize ) )
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

#if 1==0
void ringBufferTest(void)
{

	RingBufferHandle xdata rb_handle ;
	os_uint8_t xdata queue_buff[6];
	volatile  os_uint8_t test_data = 0xff;
	volatile  os_uint8_t w_data = 0;
	
	// ��ʼ��һ������
	ringbuffer_created(&rb_handle,&queue_buff,6,sizeof(os_uint8_t));
	// д��� �ǳɹ���
	w_data++;
	ringbuffer_write(&rb_handle,&w_data);
	w_data++;
	ringbuffer_write(&rb_handle,&w_data);
	w_data++;
	ringbuffer_write(&rb_handle,&w_data);
	w_data++;
	ringbuffer_write(&rb_handle,&w_data);
	w_data++;
	ringbuffer_write(&rb_handle,&w_data);
	// д��� �ǳɹ���
	
	// д��κ� ʧ��
	ringbuffer_write(&rb_handle,&(os_uint8_t *)6);
	ringbuffer_write(&rb_handle,&(os_uint8_t *)7);	
	// д��κ� ʧ��

	// ����� �ɹ�
	ringbuffer_read(&rb_handle, &test_data);
	ringbuffer_read(&rb_handle, &test_data);
	ringbuffer_read(&rb_handle, &test_data);
	ringbuffer_read(&rb_handle, &test_data);
	ringbuffer_read(&rb_handle, &test_data);
	ringbuffer_read(&rb_handle, &test_data);
	// ����� �ɹ�

	// ����κ� ��һ�ζ� ʧ�ܲ��� ����дָ���ص�����0
	ringbuffer_read(&rb_handle, &test_data);
	// ����κ� �ڶ��ζ� ����д�غ�  ����������Ϊ��
	ringbuffer_read(&rb_handle, &test_data);	


	// ������� ����������Ϊ��
	
	// дһ�γɹ�
	w_data = 0x33;
	ringbuffer_write(&rb_handle,&w_data);
	// ��һ�γɹ�
	ringbuffer_read(&rb_handle, &test_data);	
	
	// ������� ����������Ϊ��
}
#endif


