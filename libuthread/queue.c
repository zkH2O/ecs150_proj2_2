#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* node stuff*/
struct node {
	void *data;
	struct node *next;
};

/* queue thing */
struct queue {
	struct node *head;
	struct node *tail;
	int length;
};

queue_t queue_create(void)
{
        /* TODO Phase 1 */
	queue_t queue = malloc(sizeof(struct queue));
	if (queue == NULL)
		return NULL;
	
	queue->head = NULL;
	queue->tail = NULL;
	queue->length = 0;
	
	return queue;
}

int queue_destroy(queue_t queue)
{
        /* TODO Phase 1 */
	if (queue == NULL)
		return -1;
	
	if (queue->length > 0)
		return -1;
	
	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
        /* TODO Phase 1 */
	if (queue == NULL || data == NULL)
		return -1;
	
	struct node *newNode = malloc(sizeof(struct node));
	if (newNode == NULL)
		return -1;
	
	newNode->data = data;
	newNode->next = NULL;
	
	if (queue->tail == NULL) {
		queue->head = newNode;
		queue->tail = newNode;
	} else {
		queue->tail->next = newNode;
		queue->tail = newNode;
	}
	
	queue->length++;
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
        /* TODO Phase 1 */
	if (queue == NULL || data == NULL || queue->head == NULL)
		return -1;
	
	struct node *temp = queue->head;
	*data = temp->data;
	
	queue->head = queue->head->next;
	if (queue->head == NULL) {
		queue->tail = NULL;
	}
	
	free(temp);
	queue->length--;
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
        /* TODO Phase 1 */
	if (queue == NULL || data == NULL || queue->head == NULL)
		return -1;
	
	struct node *current = queue->head;
	struct node *prev = NULL;
	
	while (current != NULL) {
		if (current->data == data)
			break;
		prev = current;
		current = current->next;
	}
	
	if (current == NULL)
		return -1;
	
	if (prev == NULL) {
		queue->head = current->next;
	} else {
		prev->next = current->next;
	}
	
	if (current->next == NULL) {
		queue->tail = prev;
	}
	
	free(current);
	queue->length--;
	return 0;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
        /* TODO Phase 1 */
	if (queue == NULL || func == NULL)
		return -1;
	
	struct node *current = queue->head;
	struct node *next;
	
	while (current != NULL) {
		next = current->next;
		func(queue, current->data);
		current = next;
	}
	
	return 0;
}

int queue_length(queue_t queue)
{
        /* TODO Phase 1 */
	if (queue == NULL)
		return -1;
	
	return queue->length;
}
