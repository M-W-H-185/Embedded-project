#include "os_semaphore.h"





// ����һ���������ź������������ľ����
os_uint8_t semaphoreCreate(SemaphoreHandle *sem_handle, os_uint8_t maxCount, os_uint8_t initCount, os_uint8_t type)
{
	sem_handle->maxCount 		= maxCount;		
	sem_handle->initCount 		= initCount;
	sem_handle->messagesWaiting 	= initCount;	//��ʼ������ֵ
	sem_handle->type 			= type;


	
	memset(sem_handle->osTaskIdsWaitToTakeList, 0, (MAX_TASKS * sizeof(os_uint8_t)));
	return RET_SUCCESS;
}

// �ͷ�һ���ź���
os_uint8_t semaphoreGive(SemaphoreHandle *sem_handle)
{
	os_uint8_t ret 	= RET_ERROR;
	os_uint8_t i 	= 0;
	EA = 0;
	// ��ֵ��
	if(sem_handle->type == SEMAPHORE_TYPE_BINARY)
	{
		sem_handle->messagesWaiting = 1;
		
		ret = RET_SUCCESS;
	}
	// ������
	if(sem_handle->type == SEMAPHORE_TYPE_COUNTING)
	{
		
		if(sem_handle->messagesWaiting < sem_handle->maxCount)
		{
			sem_handle->messagesWaiting  = sem_handle->messagesWaiting  + 1;
			
			ret = RET_SUCCESS;
		}
	}
	// ��Ҫ����������ͺ����ȴ������� 
	
	for(i = 0; i < MAX_TASKS; i++)
	{
		// ����һ���ȴ���������
		if(sem_handle->osTaskIdsWaitToTakeList[i] == 1 && tcb_list[i].os_status_type == OS_BLOCKED)
		{
			// �������������Ϊ����̬ ��ʱ��0
			tcb_list[i].os_status_type 	= OS_READY;
			tcb_list[i].delay_tick		= 0;
			// �ȴ� ���������б�����0
			sem_handle->osTaskIdsWaitToTakeList[i] = 0;
			break;
		}
	}
	EA = 1;
	return ret;
}


// ��ȡһ���ź���
os_uint8_t semaphoreTake(SemaphoreHandle *sem_handle, os_uint32_t waitTicks)
{
	os_uint8_t ret 	= RET_ERROR;
	EA = 0;
	// ������������� 0
	if(sem_handle->messagesWaiting > 0)
	{
		// ��������һ �� �˳��ٽ��� ����
		sem_handle->messagesWaiting = sem_handle->messagesWaiting - 1;
		EA = 1;
		ret =  RET_SUCCESS;
		return ret;
	}
	else
	{
		// ����ǰ��ȡ����ź��� ������Ž�ȥ �ȴ��б��С��ͷ��ź�����ʱ����
		sem_handle->osTaskIdsWaitToTakeList[task_id] = 1;
		// EA = 1;
		os_delay(waitTicks);
		ret =  RET_ERROR;
		return ret;
	}

	EA = 1;
	return ret;
}







