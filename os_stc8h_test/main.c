/*
���⣺LED���̵�ϵͳ
��Ƶ��35MHz
���ڣ�51��Ƭ��ѧϰ��-ѧ�� 
���ϣ�https://rationmcu.com
*/

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"
#include "os_task.h"

/* ��������ܶ��� */

sbit LED_R = P0^5;    // ��ɫLED
sbit LED_Y = P0^6;    // ��ɫLED
sbit LED_G = P0^7;    // ��ɫLED




// �����ջ��
os_uint8_t xdata task_stack1[MAX_TASK_DEP];			/*����1��ջ.*/
os_uint8_t xdata task_stack2[MAX_TASK_DEP];			/*����2��ջ.*/
// �����ջ��



void task1()
{
	while(1)
	{
		LED_R = 1;
		os_delay(100);

		LED_R = 0;
		os_delay(100);
		
		LED_R = 1;
		os_delay(100);

	}
}

void task2()
{

	while(1)
	{
		LED_Y = 1;
		os_delay(1000);
		os_delay(1000);
		
		LED_Y = 0;
		os_delay(1000);
		os_delay(1000);

	}
}



void Timer0_Init(void)		//1����@11.0592MHz
{
	AUXR |= 0x80;			//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TL0 = 0xCD;				//���ö�ʱ��ʼֵ
	TH0 = 0xD4;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ
	ET0 = 1;				//ʹ�ܶ�ʱ��0�ж�
}

unsigned int cut = 0;

// ���������룬�ɻ��������
void time0_handle(void)large reentrant
//void time0_handle(void)interrupt 1
{
	cut++;

	if(cut == 500)
	{
		cut = 0;
		LED_G = !LED_G;
	}
	time_handleHook();

}

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

// ����һ�����λ�����
// û�ж�̬���䣬����ֻ�ǵ���ָ��һ��
void queue_created(QueueHandle *queue){

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

QueueHandle xdata queue_1 ;


/* ������ */
void main()
{
	volatile  os_uint8_t test_data = 0xff;
	
	// ��ʼ��һ������
	queue_created(&queue_1);
	// д��� �ǳɹ���
	queue_write(&queue_1,1);
	queue_write(&queue_1,2);
	queue_write(&queue_1,3);
	queue_write(&queue_1,4);
	queue_write(&queue_1,5);
	// д��� �ǳɹ���
	
	// д��κ� ʧ��
	queue_write(&queue_1,6);
	queue_write(&queue_1,7);	
	// д��κ� ʧ��

	// ����� �ɹ�
	queue_read(&queue_1, &test_data);
	queue_read(&queue_1, &test_data);
	queue_read(&queue_1, &test_data);
	queue_read(&queue_1, &test_data);
	queue_read(&queue_1, &test_data);
	queue_read(&queue_1, &test_data);
	// ����� �ɹ�

	// ����κ� ��һ�ζ� ʧ�ܲ��� ����дָ���ص�����0
	queue_read(&queue_1, &test_data);
	// ����κ� �ڶ��ζ� ����д�غ�  ����������Ϊ��
	queue_read(&queue_1, &test_data);	


	// ������� ����������Ϊ��
	
	// дһ�γɹ�
	queue_write(&queue_1,0x33);
	// ��һ�γɹ�
	queue_read(&queue_1, &test_data);	
	
	// ������� ����������Ϊ��

	P0M0 = 0x00;   //����P0.0~P0.7Ϊ˫���ģʽ
	P0M1 = 0x00;
	Timer0_Init();
	EA = 1;
	P_SW2 |= (1<<7);
	
	os_task_create(task1, &task_stack1, 1);//��task1����װ��0�Ų�
	os_task_create(task2, &task_stack2, 2);//��task2����װ��1�Ų�
	os_start();


	while(1)
	{

	}
	

}

