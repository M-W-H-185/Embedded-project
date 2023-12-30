#include "os_queue.h"



// ����һ�����λ�����
// û�ж�̬���䣬����ֻ�ǵ���ָ��һ��
void queue_created(QueueHandle *queue)
{

	queue->r_ = &queue->buff[0];	// Ĭ��ָ��buff��0
	queue->w_ = &queue->buff[0];	// Ĭ��ָ��buff��0
	queue->length = 6;
	return;
}

// ����д д���������¿�ʼд����Ҫ�ȴ�����
void queue_write(QueueHandle *queue, os_uint8_t _data)
{	os_uint8_t *next_w = queue->w_ + 1;	// ��һ��д���λ��
	
	// ��һ��д���λ�õ��� ��ȡ ��λ�� ��ʾ����
	if( next_w == queue->r_ )
	{
		return;
	}
	// �����
	if( next_w >= &queue->buff[queue->length] )
	{
		// �����ʱ������ ����0��
		return;
	}
	
	// д�����ݲ�����βָ��
	*queue->w_ = _data;
	queue->w_ = next_w;
	
	return;
}
// ���ж� ����д����ͷ�� ���»ص�����0  �� ��ǰ�����´�дλ���غϱ�ʾ��
void queue_read(QueueHandle *queue, os_uint8_t *_data)
{
	os_uint8_t *next_r = queue->r_ + 1;	// ��һ�ζ�ȡ��λ��

	
	// ��ȡ��ͷ�ˡ������λ�ú�дλ�ö���ͷ������ˡ�����һ�����ȥ ����0
	if( next_r >= &queue->buff[queue->length - 1] && ( queue->w_ >= &queue->buff[queue->length - 1] )    )
	{
		// ������������ ���λ����������һλ��ǰһλ����  ��ȡ���ݺ� ����д��Ͷ�ȡָ�� 
		*_data = *queue->r_;
		
	
		queue->r_ = &queue->buff[0];	
		queue->w_ = &queue->buff[0];	
		// �������״̬
		return;
	}
	
	// ��ǰ��ȡλ�� �� ��һ��д��λ���غ� ��ʾ��
	if(queue->r_ == queue->w_)
	{
		return;
	}
	// ��ȡ���ݲ�����ͷָ��
	*_data = *queue->r_;
	queue->r_ = next_r;
	




	
	return;
}
