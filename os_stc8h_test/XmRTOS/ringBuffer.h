#ifndef __OS_QUEUE_H__
#define __OS_QUEUE_H__
#include "os_task.h"


// 空缓冲区： 在初始化时，头指针和尾指针都指向同一个位置，即缓冲区的起始位置。这时可以通过判断头指针和尾指针是否相等来判断缓冲区是否为空。
// 满缓冲区： 当尾指针移动到头指针的前一个位置时，说明缓冲区已满。如果没有预留一个位置，头尾指针会重叠，无法准确判断是空还是满。
// 消息队列
typedef struct RingBufferHandle_t
{
	os_uint8_t *r_;			// 下一次要被读取的元素
	os_uint8_t *w_;			// 下一次要被写入的位置
	os_uint8_t 	buff_size;	// 缓冲区长度
	os_uint8_t 	itemSize;	// 单个数据项的大小
	void 		*buff;		// 缓冲区
}RingBufferHandle;


void ringbuffer_created(RingBufferHandle *ring_buffer, void *buff, os_uint8_t buff_size, os_uint8_t itemSize);
void ringbuffer_write(RingBufferHandle *ring_buffer, void *_data);
void ringbuffer_read(RingBufferHandle *ring_buffer, void *_data);

#endif
