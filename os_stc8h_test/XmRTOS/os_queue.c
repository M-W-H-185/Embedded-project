#include "os_queue.h"



// 创建一个环形缓冲区
// 没有动态分配，所以只是单纯指向一下
void queue_created(QueueHandle *queue)
{

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
