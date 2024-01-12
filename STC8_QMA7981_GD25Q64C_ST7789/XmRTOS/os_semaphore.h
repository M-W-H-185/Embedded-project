


#ifndef __OS_SEMAPHORE__
#define __OS_SEMAPHORE__

#include "intrins.h"
#include "stdio.h"
#include "string.h"
#include "os_task.h"
#include "ringBuffer.h"

#define SEMAPHORE_TYPE_MUTEX               	( ( os_uint8_t ) 1 ) /* �����ź��� */
#define SEMAPHORE_TYPE_COUNTING  			( ( os_uint8_t ) 2 ) /* �����ź��� */
#define SEMAPHORE_TYPE_BINARY    			( ( os_uint8_t ) 3 ) /* ��ֵ�ź��� */

typedef struct SemaphoreHandle_t
{
	os_uint8_t maxCount;							// ���ļ���ֵ
	os_uint8_t initCount;							// ��ʼ����ֵ
	os_uint8_t messagesWaiting;						// �źż�����
	os_uint8_t osTaskIdsWaitToTakeList[MAX_TASKS];	// �ȴ���ȡ�ͺ���������id�б�
	os_uint8_t type;								// �ź�������
}SemaphoreHandle;


// ����һ��ͨ���ź�������
os_uint8_t semaphoreCreate(SemaphoreHandle *sem_handle, os_uint8_t maxCount, os_uint8_t initCount, os_uint8_t type);
// �ͷ�һ���ź���
os_uint8_t semaphoreGive(SemaphoreHandle *sem_handle);
// ��ȡһ���ź���
os_uint8_t semaphoreTake(SemaphoreHandle *sem_handle, os_uint32_t waitTicks);



// ����һ���������ź���
#define semaphoreCreateCount(sem_handel,maxCount,initCount) semaphoreCreate(sem_handel,maxCount,initCount,SEMAPHORE_TYPE_COUNTING)
// ����һ����ֵ���ź��� Ĭ��ֵ��0 ��Ҫ���ͷ��ڻ�ȡ
#define semaphoreCreateBinary(sem_handel) semaphoreCreate(sem_handel, 1, 0,SEMAPHORE_TYPE_BINARY)

#endif

