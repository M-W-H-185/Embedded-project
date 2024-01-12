#include "ringBuffer.h"

/**
*	环形数组 最后一位保留
*	空缓冲区： 在初始化时，读指针和写指针都指向同一个位置，即缓冲区的起始位置。这时可以通过判断头指针和尾指针是否相等来判断缓冲区是否为空。
*	满缓冲区： 读指针 > 写指针的时候 表示缓冲区已经满了。
*	写指针是不会到达数组最后一位的，在读的时候next_r指向最后一位的时候代表缓冲区已满
*/


/***********************************************************************
*	函数描述：创建一个环形缓冲区操作句柄
*	传入参数：
*				ring_buffer	: 缓冲区操作句柄指针
*				buff		: 缓冲数组
*				buff_size	: 缓冲数组
*				itemSize	: 缓冲数组每一个元素的大小
* 返回参数：
*				os_uint8_t	: RINGBUFF_SUCCESS 只返回成功
***********************************************************************/
os_uint8_t ringbuffer_created(RingBufferHandle *ring_buffer, void *buff, os_uint8_t buff_size, os_uint8_t itemSize)
{

	
	ring_buffer->r_ 		= buff;           // 默认指向buff头部
	ring_buffer->w_ 		= buff;           // 默认指向buff头部
	ring_buffer->itemSize 	= itemSize;
	ring_buffer->buff_size 	= buff_size;
	ring_buffer->buff 		= buff;

	
	return RINGBUFF_SUCCESS;
}
/***********************************************************************
*	函数描述：  缓冲区写 写入满不重新开始写，需要等待读完
*	传入参数：
*				ring_buffer	: 缓冲区操作句柄指针
*				_data		: 写入的数据项
* 返回参数：
*				os_uint8_t	: 
*								RINGBUFF_WRITE_OVERFLOW = 缓冲区溢出
*								RINGBUFF_SUCCESS		= 写入成功 
***********************************************************************/
os_uint8_t ringbuffer_write(RingBufferHandle *ring_buffer, void *_data)
{		
	os_uint8_t *next_w = ring_buffer->w_ + ring_buffer->itemSize;  // 下一次写入的位置

	
	// 下一次写入的位置等于 读取 的位置 表示满了
    if (next_w == ring_buffer->r_) {
		// 溢出
        return RINGBUFF_WRITE_OVERFLOW; 

    }

	// 写一个写入指向 数组最后一位表示溢出
    if (next_w >= (os_uint8_t *)(ring_buffer->buff + ( (ring_buffer->buff_size - 1) * ring_buffer->itemSize ) )) {
		// 溢出
        return RINGBUFF_WRITE_OVERFLOW; 
    }

    memcpy((void *)ring_buffer->w_, _data, ring_buffer->itemSize); // 写入数据
    ring_buffer->w_ = next_w; // 更新写入 尾指针
	return RINGBUFF_SUCCESS;
}

/***********************************************************************
*	函数描述：  缓冲区读 读和写都到头了 重新回到数组0
*	传入参数：
*				ring_buffer	: 缓冲区操作句柄指针
*				_data		: 读取到数据的指针
* 返回参数：
*				os_uint8_t	: 
*								RINGBUFF_READ_NULL		= 缓冲区无数据
*								RINGBUFF_SUCCESS		= 写入成功 
***********************************************************************/
os_uint8_t ringbuffer_read(RingBufferHandle *ring_buffer, void *_data)
{

	// 下一次读取的位置
    os_uint8_t *next_r  = (os_uint8_t *)ring_buffer->r_ + ring_buffer->itemSize;  
	// 下一次读指针 > 写指针 并且  下一次 等于 数组最尾部的时候 数组溢出
    if (
			next_r > ring_buffer->w_ && next_r == (os_uint8_t *)( ring_buffer->buff + ( (ring_buffer->buff_size - 1) * ring_buffer->itemSize ) )
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

#if 1==0
void ringBufferTest(void)
{

	RingBufferHandle xdata rb_handle ;
	os_uint8_t xdata queue_buff[6];
	volatile  os_uint8_t test_data = 0xff;
	volatile  os_uint8_t w_data = 0;
	
	// 初始化一个队列
	ringbuffer_created(&rb_handle,&queue_buff,6,sizeof(os_uint8_t));
	// 写五次 是成功的
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
	// 写五次 是成功的
	
	// 写五次后 失败
	ringbuffer_write(&rb_handle,&(os_uint8_t *)6);
	ringbuffer_write(&rb_handle,&(os_uint8_t *)7);	
	// 写五次后 失败

	// 读五次 成功
	ringbuffer_read(&rb_handle, &test_data);
	ringbuffer_read(&rb_handle, &test_data);
	ringbuffer_read(&rb_handle, &test_data);
	ringbuffer_read(&rb_handle, &test_data);
	ringbuffer_read(&rb_handle, &test_data);
	ringbuffer_read(&rb_handle, &test_data);
	// 读五次 成功

	// 读五次后 第一次读 失败并让 读和写指正回到数组0
	ringbuffer_read(&rb_handle, &test_data);
	// 读五次后 第二次读 读和写重合  缓冲区数据为空
	ringbuffer_read(&rb_handle, &test_data);	


	// 这两句后 缓冲区数据为空
	
	// 写一次成功
	w_data = 0x33;
	ringbuffer_write(&rb_handle,&w_data);
	// 读一次成功
	ringbuffer_read(&rb_handle, &test_data);	
	
	// 这两句后 缓冲区数据为空
}
#endif


