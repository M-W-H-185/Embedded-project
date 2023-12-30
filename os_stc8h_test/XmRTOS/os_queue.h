#ifndef __OS_QUEUE_H__
#define __OS_QUEUE_H__
#include "os_task.h"


// �ջ������� �ڳ�ʼ��ʱ��ͷָ���βָ�붼ָ��ͬһ��λ�ã�������������ʼλ�á���ʱ����ͨ���ж�ͷָ���βָ���Ƿ�������жϻ������Ƿ�Ϊ�ա�
// ���������� ��βָ���ƶ���ͷָ���ǰһ��λ��ʱ��˵�����������������û��Ԥ��һ��λ�ã�ͷβָ����ص����޷�׼ȷ�ж��ǿջ�������
// ��Ϣ����
typedef struct QueueHandle_t
{
	os_uint8_t *r_;			// ��һ��Ҫ����ȡ��Ԫ��
	os_uint8_t *w_;			// ��һ��Ҫ��д���λ��
	os_uint8_t 	length;		// ����
	os_uint8_t 	itemSize;	// ����������Ĵ�С
	os_uint8_t 	buff[6];
}QueueHandle;


void queue_created(QueueHandle *queue);
void queue_write(QueueHandle *queue, os_uint8_t _data);
void queue_read(QueueHandle *queue, os_uint8_t *_data);

#endif
