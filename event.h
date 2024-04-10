#ifndef EVENT_H
#define EVENT_H

#include "queue.h"

struct EventQueueNode {
    double event_time;
    int event_type;
    struct QueueNode *qnode;
    struct EventQueueNode *next;
};

struct EventQueue {
    struct EventQueueNode *head;
    struct EventQueueNode *tail;
};

void InsertEvent(struct EventQueue *eventQ, struct QueueNode *qnode, double event_time, int event_type);
void EventDelete(struct EventQueue *eventQ);
void FreeEventQueue(struct EventQueue *eventQ);

#endif // EVENT_H
