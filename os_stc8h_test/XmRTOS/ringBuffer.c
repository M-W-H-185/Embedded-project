#include "ringBuffer.h"

/**
*	环形数组 最后一位保留
*	空缓冲区： 在初始化时，读指针和写指针都指向同一个位置，即缓冲区的起始位置。这时可以通过判断头指针和尾指针是否相等来判断缓冲区是否为空。
*	满缓冲区： 读指针 > 写指针的时候 表示缓冲区已经满了。
*	写指针是不会到达数组最后一位的，在读的时候next_r指向最后一位的时候代表缓冲区已满
*/
// 创建一个环形缓冲区操作句柄
os_uint8_t ringbuffer_created(RingBufferHandle *ring_buffer, void *buff, os_uint8_t buff_size, os_uint8_t itemSize)
{

	
	ring_buffer->r_ 		= buff;           // 默认指向buff头部
	ring_buffer->w_ 		= buff;           // 默认指向buff头部
	ring_buffer->itemSize 	= itemSize;
	ring_buffer->buff_size 	= buff_size;
	ring_buffer->buff 		= buff;

	
	return RINGBUFF_SUCCESS;
}

// 缓冲区写 写入满不重新开始写，需要等待读完
os_uint8_t ringbuffer_write(RingBufferHandle *ring_buffer, void *_data)
{		
	os_uint8_t *next_w = ring_buffer->w_ + ring_buffer->itemSize;  // 下一次写入的位置
	// 下一次写入的位置等于 读取 的位置 表示满了
    if (next_w == ring_buffer->r_) {
		// 溢出
        return RINGBUFF_WRITE_OVERFLOW; 

    }
	// 写一个写入指向 数组最后一位表示溢出
    if (next_w >= (os_uint8_t *)(ring_buffer->buff + ring_buffer->buff_size)) {
		// 溢出
        return RINGBUFF_WRITE_OVERFLOW; 
    }

    memcpy((void *)ring_buffer->w_, _data, ring_buffer->itemSize); // 写入数据
    ring_buffer->w_ = next_w; // 更新写入 尾指针
	return RINGBUFF_SUCCESS;
}

// 缓冲区读 读和写都到头了 重新回到数组0  ； 当前读和下次写位置重合表示空
os_uint8_t ringbuffer_read(RingBufferHandle *ring_buffer, void *_data)
{

	// 下一次读取的位置
    os_uint8_t *next_r  = (os_uint8_t *)ring_buffer->r_ + ring_buffer->itemSize;  
	// 下一次读指针 > 写指针 并且  下一次 等于 数组最尾部的时候 数组溢出
    if (
			next_r > ring_buffer->w_ && next_r == (os_uint8_t *)(ring_buffer->buff + (ring_buffer->buff_size ) )
		) 
	{
        memcpy(_data, ring_buffer->r_, ring_buffer->itemSize);
		// 将读写指针 指向数据起始位 数组0
        ring_buffer->r_ = ring_buffer->buff; 
        ring_buffer->w_ = ring_buffer->buff; 
        return RINGBUFF_READ_NULL; // 算是溢出也算空。空吧算
    }
	
	// 当前读取位置 和 下一次写入位置重合 表示空
    if (ring_buffer->r_ == ring_buffer->w_) {
        return RINGBUFF_READ_NULL; // 空
    }
	
	// 读取数据并更新读指针
    memcpy(_data, ring_buffer->r_, ring_buffer->itemSize); // 读取数据
    ring_buffer->r_ = next_r; // 更新读指针

	
	return RINGBUFF_SUCCESS;
}
