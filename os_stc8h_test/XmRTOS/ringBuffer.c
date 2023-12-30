#include "ringBuffer.h"

/**
*	环形数组 最后一位保留
*	空缓冲区： 在初始化时，读指针和写指针都指向同一个位置，即缓冲区的起始位置。这时可以通过判断头指针和尾指针是否相等来判断缓冲区是否为空。
*	满缓冲区： 读和写指针指向 数组max-1的时候代表满了
*/

// 创建一个环形缓冲区操作句柄
void ringbuffer_created(RingBufferHandle *ring_buffer, void *buff, os_uint8_t buff_size, os_uint8_t itemSize)
{

	
	ring_buffer->r_ 		= buff;           // 默认指向buff头部
	ring_buffer->w_ 		= buff;           // 默认指向buff头部
	ring_buffer->itemSize 	= itemSize;
	ring_buffer->buff_size 	= buff_size;
	ring_buffer->buff 		= buff;

	
	return;
}

// 缓冲区写 写入满不重新开始写，需要等待读完
void ringbuffer_write(RingBufferHandle *ring_buffer, void *_data)
{		
	os_uint8_t *next_w = ring_buffer->w_ + ring_buffer->itemSize;  // 下一次写入的位置
	// 下一次写入的位置等于 读取 的位置 表示满了
    if (next_w == ring_buffer->r_) {
		// 满了
        return; 

    }

    if (next_w >= (os_uint8_t *)(ring_buffer->buff + ring_buffer->buff_size)) {
		// 溢出
        return; 
    }

    memcpy((void *)ring_buffer->w_, _data, ring_buffer->itemSize); // 写入数据
    ring_buffer->w_ = next_w; // 更新写入 尾指针
	return;
}
// 缓冲区读 读和写都到头了 重新回到数组0  ； 当前读和下次写位置重合表示空
void ringbuffer_read(RingBufferHandle *ring_buffer, void *_data)
{

	// 下一次读取的位置
    os_uint8_t *next_r = (os_uint8_t *)ring_buffer->r_ + ring_buffer->itemSize;  
	// 读取到头了。代表读位置和写位置都到 环形数组的最后一位-1  代表溢出了。两个一起滚回去 数组0
    if (
			next_r >= (os_uint8_t *)(ring_buffer->buff + (ring_buffer->buff_size - 1) )
				&& 
			ring_buffer->w_ >= (os_uint8_t *)(ring_buffer->buff + (ring_buffer->buff_size - 1) )
		) 
	{
        memcpy(_data, ring_buffer->r_, ring_buffer->itemSize);
		// 将读写指针 指向数据起始位 数组0
        ring_buffer->r_ = ring_buffer->buff; 
        ring_buffer->w_ = ring_buffer->buff; 
        return; // 溢出状态
    }
	
	// 当前读取位置 和 下一次写入位置重合 表示空
    if (ring_buffer->r_ == ring_buffer->w_) {
        return; // 空
    }
	
	// 读取数据并更新读指针
    memcpy(_data, ring_buffer->r_, ring_buffer->itemSize); // 读取数据
    ring_buffer->r_ = next_r; // 更新读指针

	
	return;
}
