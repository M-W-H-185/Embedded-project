#ifndef __OS_QUEUE_H__
#define __OS_QUEUE_H__
#include "os_task.h"



typedef struct RingBufferHandle_t
{
	os_uint8_t *r_;			// 下一次要被读取的元素
	os_uint8_t *w_;			// 下一次要被写入的位置
	os_uint8_t 	buff_size;	// 缓冲区长度
	os_uint8_t 	itemSize;	// 单个数据项的大小
	void 		*buff;		// 缓冲区
}RingBufferHandle;

enum RingBuffRetuenState{
	RINGBUFF_SUCCESS 			= 1,	// 缓冲区操作成功
	RINGBUFF_WRITE_OVERFLOW 	= 2,	// 写入溢出
	RINGBUFF_READ_NULL 			= 3,	// 读取失败 缓冲区无数据
};

os_uint8_t ringbuffer_created(RingBufferHandle *ring_buffer, void *buff, os_uint8_t buff_size, os_uint8_t itemSize);
os_uint8_t ringbuffer_write(RingBufferHandle *ring_buffer, void *_data);
os_uint8_t ringbuffer_read(RingBufferHandle *ring_buffer, void *_data);

#endif
