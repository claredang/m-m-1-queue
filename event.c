#include "event.h"
#include <stdio.h>
#include <stdlib.h>

void InsertEvent(struct EventQueue *eventQ, struct QueueNode *qnode, double event_time, int event_type) {
    // printf("Inside insertEvent\n");
    struct EventQueueNode *current = CreateEventNode(qnode, event_time, event_type);
    struct EventQueueNode *newNode = eventQ->head;
    // Queue is empty
    if (newNode == NULL) {
        eventQ->head = current;
        eventQ->tail = current;
    }
    else {
        while (newNode != NULL) {
            newNode = newNode->next;
        }
        eventQ->tail->next = current;
        eventQ->tail = current;
    }
    dumpEventQueue(eventQ);
    sortList(eventQ);
    return;
}

void EventDelete (struct EventQueue *q) {
    if (q->head != NULL) {
    struct EventQueueNode* toRemove = q->head;
    q->head = q->head->next;
    if (q->tail == toRemove) {
        q->tail = NULL;
    }
    free(toRemove);
  }
}

void FreeEventQueue(struct EventQueue* eventQ) {
   while (eventQ->head != NULL) {
    EventDelete(eventQ);
  }
  free(eventQ);
}
