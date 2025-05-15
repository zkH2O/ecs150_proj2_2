#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

/* threadstates */
typedef enum {
	RUNNING,
	READY,
	BLOCKED,
	EXITED
} uthread_state;

struct uthread_tcb {
        /* TODO Phase 2  */
	uthread_ctx_t context;
	void *stackPointer;
	uthread_state state;
};

static queue_t readyQueue = NULL;
static struct uthread_tcb *currentThread = NULL;

struct uthread_tcb *uthread_current(void)
{
        /* TODO Phase 2/3  */
	return currentThread;
}

void uthread_yield(void)
{
        /* TODO Phase 2  */
	if (queue_length(readyQueue) == 0)
		return;

	struct uthread_tcb *nextThread;
	queue_dequeue(readyQueue, (void**)&nextThread);

	struct uthread_tcb *prevThread = currentThread;
	prevThread->state = READY;
	queue_enqueue(readyQueue, prevThread);

	currentThread = nextThread;
	currentThread->state = RUNNING;

	uthread_ctx_switch(&prevThread->context, &currentThread->context);
}

void uthread_exit(void)
{
        /* TODO Phase 2  */
	if (queue_length(readyQueue) == 0) {
		return;
	}
	
	struct uthread_tcb *nextThread;
	queue_dequeue(readyQueue, (void**)&nextThread);

	struct uthread_tcb *prevThread = currentThread;
	prevThread->state = EXITED;

	uthread_ctx_destroy_stack(prevThread->stackPointer);
	free(prevThread);

	currentThread = nextThread;
	currentThread->state = RUNNING;

	uthread_ctx_switch(&prevThread->context, &currentThread->context);

	fprintf(stderr, "Error: uthread_exit reached unreachable code\n");
	exit(1);
}

int uthread_create(uthread_func_t func, void *arg)
{
        /* TODO Phase 2  */
	struct uthread_tcb *tcb = malloc(sizeof(struct uthread_tcb));
	if (tcb == NULL)
		return -1;

	tcb->stackPointer = uthread_ctx_alloc_stack();
	if (tcb->stackPointer == NULL) {
		free(tcb);
		return -1;
	}

	if (uthread_ctx_init(&tcb->context, tcb->stackPointer, func, arg) != 0) {
		uthread_ctx_destroy_stack(tcb->stackPointer);
		free(tcb);
		return -1;
	}

	tcb->state = READY;

	if (queue_enqueue(readyQueue, tcb) == -1) {
		uthread_ctx_destroy_stack(tcb->stackPointer);
		free(tcb);
		return -1;
	}

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
        /* TODO Phase 2  */
	readyQueue = queue_create();
	if (readyQueue == NULL)
		return -1;

	struct uthread_tcb *mainThread = malloc(sizeof(struct uthread_tcb));
	if (mainThread == NULL) {
		queue_destroy(readyQueue);
		return -1;
	}
	
	if (getcontext(&mainThread->context) != 0) {
		free(mainThread);
		queue_destroy(readyQueue);
		return -1;
	}
	
	mainThread->stackPointer = NULL;
	mainThread->state = RUNNING;
	
	currentThread = mainThread;

	preempt_start(preempt);

	if (uthread_create(func, arg) != 0) {
		preempt_stop();
		queue_destroy(readyQueue);
		free(mainThread);
		return -1;
	}

	while (queue_length(readyQueue) > 0) {
		uthread_yield();
	}

	preempt_stop();

	queue_destroy(readyQueue);
	free(mainThread);

	return 0;
}

void uthread_block(void)
{
        /* TODO Phase 3  */
	if (queue_length(readyQueue) == 0)
		return;

	struct uthread_tcb *nextThread;
	queue_dequeue(readyQueue, (void**)&nextThread);

	struct uthread_tcb *prevThread = currentThread;
	prevThread->state = BLOCKED;

	currentThread = nextThread;
	currentThread->state = RUNNING;

	uthread_ctx_switch(&prevThread->context, &currentThread->context);
}

void uthread_unblock(struct uthread_tcb *uthread)
{
        /* TODO Phase 3  */
	if (uthread == NULL)
		return;

	if (uthread->state != BLOCKED)
		return;

	uthread->state = READY;

	queue_enqueue(readyQueue, uthread);
}
