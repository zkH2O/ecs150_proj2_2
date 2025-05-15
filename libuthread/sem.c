#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "private.h"
#include "sem.h"

struct semaphore {
        /* TODO Phase 3 */
	size_t count;           
	queue_t waitingQueue;
	queue_t wakeupQueue;
};

sem_t sem_create(size_t count)
{
        /* TODO Phase 3 */
	sem_t sem = malloc(sizeof(struct semaphore));
	if (sem == NULL)
		return NULL;
	
	sem->count = count;
	
	sem->waitingQueue = queue_create();
	if (sem->waitingQueue == NULL) {
		free(sem);
		return NULL;
	}
	
	sem->wakeupQueue = queue_create();
	if (sem->wakeupQueue == NULL) {
		queue_destroy(sem->waitingQueue);
		free(sem);
		return NULL;
	}
	
	return sem;
}

int sem_destroy(sem_t sem)
{
        /* TODO Phase 3 */
	if (sem == NULL)
		return -1;
	
	/* check if threads still stuck here */
	if (queue_length(sem->waitingQueue) > 0 || queue_length(sem->wakeupQueue) > 0)
		return -1;
	
	/*cleaning */
	queue_destroy(sem->waitingQueue);
	queue_destroy(sem->wakeupQueue);
	free(sem);
	
	return 0;
}

int sem_down(sem_t sem)
{
        /* TODO Phase 3 */
	if (sem == NULL)
		return -1;
	
	preempt_disable();
	
	if (sem->count > 0) {
		sem->count--;
		preempt_enable();
		return 0;
	}
	
	/* check if thread wake before but no run yet */
	struct uthread_tcb *thread = uthread_current();
	struct uthread_tcb *waitingThread;
	int found = 0;
	
	/* look through wake list for thread now */
	size_t wakeupLen = queue_length(sem->wakeupQueue);
	for (size_t i = 0; i < wakeupLen; i++) {
		queue_dequeue(sem->wakeupQueue, (void**)&waitingThread);
		
		if (waitingThread == thread) {
			found = 1;
		} else {
			queue_enqueue(sem->wakeupQueue, waitingThread);
		}
	}
	if (found) {
		preempt_enable();
		return 0;
	}
	
	queue_enqueue(sem->waitingQueue, thread);
	
	preempt_enable();
	
	uthread_block();
	
	return sem_down(sem);
}

int sem_up(sem_t sem)
{
        /* TODO Phase 3 */
	if (sem == NULL)
		return -1;
	
	preempt_disable();
	
	if (queue_length(sem->waitingQueue) > 0) {
		struct uthread_tcb *thread;
		queue_dequeue(sem->waitingQueue, (void**)&thread);
		
		queue_enqueue(sem->wakeupQueue, thread);
		
		uthread_unblock(thread);
	} else {
		sem->count++;
	}
	
	preempt_enable();
	
	return 0;
}
