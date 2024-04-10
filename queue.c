#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

void Insert(struct Queue *q, double arrival_time, double service_time) {
    struct QueueNode *current = CreateNode(arrival_time, service_time);
    struct QueueNode *newNode = q->head;
    // Queue is empty
    if (newNode == NULL) {
        q->head = current;
        q->tail = current;
    }
    else {
        while (newNode != NULL) {
            newNode = newNode->next;
        }
        q->tail->next = current;
        q->tail = current;
    }
    return;
}

void Delete(struct Queue *q) {
    if (q->head != NULL) {
    struct QueueNode* toRemove = q->head;
    q->head = q->head->next;
    if (q->tail == toRemove) {
        q->tail = NULL;
    }
    free(toRemove);
  }
}

void InitializeQueue(struct Queue *elementQ, int seed, double lambda, double mu, int total_departures) {
    // Initialize random number
  srand(seed);

  struct Queue* elementQueue = (struct Queue*) malloc(sizeof (struct Queue));
  elementQueue->head = NULL;
  elementQueue->tail = NULL;
  elementQueue->first = NULL;
  elementQueue->last = NULL;
  elementQueue->waiting_count = 0;
  
  elementQueue->cumulative_response = 0;
  elementQueue->cumulative_waiting = 0;
  elementQueue->cumulative_idle_times = 0;
  elementQueue->cumulative_number = 0;

  elementQueue->number_in_system = 0;
  elementQueue->cumulative_busy_time = 0;

  double arrivalTimes[total_departures];

  // Generate arrival_times
  arrivalTimes[0] = generateRandom(lambda);
  for (int i = 1; i < total_departures; i++) {
      arrivalTimes[i] = arrivalTimes[i-1] + generateRandom(lambda);
  }

  // Generate service_times
  for (int i = 0; i < total_departures; i++) {
    Insert(elementQueue, arrivalTimes[i], generateRandom(mu));
  }

  // Intialize server 
  printf("Initialize server\n");
  elementQueue->server1 = (struct Server*) malloc(sizeof (struct Server));
  elementQueue->server1->status = 0;
  elementQueue->server1->available_time = 0;
  elementQueue->server2 = (struct Server*) malloc(sizeof (struct Server));
  elementQueue->server2->status = 0;
  elementQueue->server2->available_time = 0;
  elementQueue->currentServer = elementQueue->server1;

  return elementQueue;

void FreeQueue(struct Queue *elementQ) {
    while (elementQ->first != NULL) {
    DeleteLast(elementQ);
  }
   while (elementQ->head != NULL) {
    Delete(elementQ);
  }
  free(elementQ->server1);
  free(elementQ->server2);
  free(elementQ);
}
