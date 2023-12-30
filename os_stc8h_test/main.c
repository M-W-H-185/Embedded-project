/*
标题：LED红绿灯系统
主频：35MHz
用于：51单片机学习机-学电 
资料：https://rationmcu.com
*/

#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"
#include "os_task.h"

/* 发光二极管定义 */

sbit LED_R = P0^5;    // 红色LED
sbit LED_Y = P0^6;    // 黄色LED
sbit LED_G = P0^7;    // 绿色LED




// 任务堆栈区
os_uint8_t xdata task_stack1[MAX_TASK_DEP];			/*任务1堆栈.*/
os_uint8_t xdata task_stack2[MAX_TASK_DEP];			/*任务2堆栈.*/
// 任务堆栈区



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



void Timer0_Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0xCD;				//设置定时初始值
	TH0 = 0xD4;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
}

unsigned int cut = 0;

// 函数冲重入，由汇编代码调用
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

// 空缓冲区： 在初始化时，头指针和尾指针都指向同一个位置，即缓冲区的起始位置。这时可以通过判断头指针和尾指针是否相等来判断缓冲区是否为空。
// 满缓冲区： 当尾指针移动到头指针的前一个位置时，说明缓冲区已满。如果没有预留一个位置，头尾指针会重叠，无法准确判断是空还是满。
// 消息队列
typedef struct QueueHandle_t
{
	os_uint8_t *r_;			// 下一次要被读取的元素
	os_uint8_t *w_;			// 下一次要被写入的位置
	os_uint8_t 	length;		// 长度
	os_uint8_t 	itemSize;	// 单个数据项的大小
	os_uint8_t 	buff[6];
}QueueHandle;

// 创建一个环形缓冲区
// 没有动态分配，所以只是单纯指向一下
void queue_created(QueueHandle *queue){

	queue->r_ = &queue->buff[0];	// 默认指向buff的0
	queue->w_ = &queue->buff[0];	// 默认指向buff的0
	queue->length = 6;
	return;
}
// 队列写 写入满不重新开始写，需要等待读完
void queue_write(QueueHandle *queue, os_uint8_t _data)
{	os_uint8_t *next_w = queue->w_ + 1;	// 下一次写入的位置
	
	// 下一次写入的位置等于 读取 的位置 表示满了
	if( next_w == queue->r_ )
	{
		return;
	}
	// 溢出咯
	if( next_w >= &queue->buff[queue->length] )
	{
		// 溢出暂时不跳到 数组0吧
		return;
	}
	
	// 写入数据并更新尾指针
	*queue->w_ = _data;
	queue->w_ = next_w;
	
	return;
}
// 队列读 读和写都到头了 重新回到数组0  ； 当前读和下次写位置重合表示空
void queue_read(QueueHandle *queue, os_uint8_t *_data)
{
	os_uint8_t *next_r = queue->r_ + 1;	// 下一次读取的位置

	
	// 读取到头了。代表读位置和写位置都到头了溢出了。两个一起滚回去 数组0
	if( next_r >= &queue->buff[queue->length - 1] && ( queue->w_ >= &queue->buff[queue->length - 1] )    )
	{
		// 到了这里算是 环形缓冲区的最后一位的前一位数据  读取数据后 更新写入和读取指针 
		*_data = *queue->r_;
		
	
		queue->r_ = &queue->buff[0];	
		queue->w_ = &queue->buff[0];	
		// 返回溢出状态
		return;
	}
	
	// 当前读取位置 和 下一次写入位置重合 表示空
	if(queue->r_ == queue->w_)
	{
		return;
	}
	// 读取数据并更新头指针
	*_data = *queue->r_;
	queue->r_ = next_r;
	




	
	return;
}

QueueHandle xdata queue_1 ;


/* 主函数 */
void main()
{
	volatile  os_uint8_t test_data = 0xff;
	
	// 初始化一个队列
	queue_created(&queue_1);
	// 写五次 是成功的
	queue_write(&queue_1,1);
	queue_write(&queue_1,2);
	queue_write(&queue_1,3);
	queue_write(&queue_1,4);
	queue_write(&queue_1,5);
	// 写五次 是成功的
	
	// 写五次后 失败
	queue_write(&queue_1,6);
	queue_write(&queue_1,7);	
	// 写五次后 失败

	// 读五次 成功
	queue_read(&queue_1, &test_data);
	queue_read(&queue_1, &test_data);
	queue_read(&queue_1, &test_data);
	queue_read(&queue_1, &test_data);
	queue_read(&queue_1, &test_data);
	queue_read(&queue_1, &test_data);
	// 读五次 成功

	// 读五次后 第一次读 失败并让 读和写指正回到数组0
	queue_read(&queue_1, &test_data);
	// 读五次后 第二次读 读和写重合  缓冲区数据为空
	queue_read(&queue_1, &test_data);	


	// 这两句后 缓冲区数据为空
	
	// 写一次成功
	queue_write(&queue_1,0x33);
	// 读一次成功
	queue_read(&queue_1, &test_data);	
	
	// 这两句后 缓冲区数据为空

	P0M0 = 0x00;   //设置P0.0~P0.7为双向口模式
	P0M1 = 0x00;
	Timer0_Init();
	EA = 1;
	P_SW2 |= (1<<7);
	
	os_task_create(task1, &task_stack1, 1);//将task1函数装入0号槽
	os_task_create(task2, &task_stack2, 2);//将task2函数装入1号槽
	os_start();


	while(1)
	{

	}
	

}

