#include "simulator.h"
#include <stdio.h>

static double computed_stats[4];  // Store computed statistics: [E(n), E(r), E(w), p0]
static double simulated_stats[4]; // Store simulated statistics [n, r, w, sim_p0]
int departure_count = 0;         // current number of departures from queue
double current_time = 0;          // current time during simulation
double last_event = 0;

void Simulation(struct Queue *elementQ, double lambda, double mu, int print_period, int total_departures) {
    struct EventQueue* eventQ = InitializeEventQueue();
  printf("Total number in system %f \n", elementQ->cumulative_number);
  dumpQueue(elementQ);

  struct QueueNode* next_arrival = elementQ->head;
  InsertEvent(eventQ, elementQ->head, elementQ->head->arrival_time, 1);

  while (departure_count < total_departures) {
    printf("\n    ===== Start of a new loop ===== \n");
    if (elementQ->server1->status == 1) {
      elementQ->number_in_system++;
    }   
    if (elementQ->server2->status == 1) {
      elementQ->number_in_system++;
    }
    if (elementQ->first != NULL) {
      elementQ->number_in_system += CountNodes(elementQ);
    }

    printf("Total number in system %d \n", elementQ->number_in_system);

    current_time = eventQ->head->event_time;
    elementQ->cumulative_number += (current_time - last_event) * elementQ->number_in_system;
    last_event = current_time;
    printf("\n Current time %f \n", current_time);
    printf("\n Check Server1 available time and status {%f, %d}", elementQ->server1->available_time, elementQ->server1->status);
    printf("\n Check Server2 available time and status {%f, %d} \n", elementQ->server2->available_time, elementQ->server2->status);
    // printf("Last event {%d, %f} \n", eventQ->head->event_type, eventQ->head->event_time);
    printf("Head Type and event time {%d, %f} \n", eventQ->head->event_type, eventQ->head->event_time);

    if (eventQ->head->event_type == 1 ) {
      printf("\n ==== 1. Case Process Arrival ====\n");
      next_arrival = ProcessArrival(elementQ, next_arrival, eventQ);
      if (next_arrival == NULL){
        break;
      }
      printf("\n !!!!!!! Next_arrival %f \n", next_arrival->arrival_time);
      InsertEvent(eventQ, next_arrival, next_arrival->arrival_time, 1);
    }
    else if (eventQ->head->event_type == 3) {
      printf("\n ==== 3. Case Departure ====\n");
      ProcessDeparture(elementQ, FindNodeAfterTime(elementQ, eventQ->head->event_time), eventQ);
    }
    dumpQueueLast(elementQ);
    dumpEventQueue(eventQ);
    EventDelete(eventQ);
  }
  FreeEventQueue(eventQ);
}

struct QueueNode *ProcessArrival(struct Queue *elementQ, struct QueueNode *arrival, struct EventQueue *eventQ) {
    printf("\n == Inside Process Arrival == ");
  printf("\n Current time %f \n", current_time);
  // printf("\n elementQ first arrival time %f \n", elementQ->first->arrival_time);
  // InsertEvent(eventQ, arrival, current_time, 1);

  // elementQ->number_in_system++;

  // In case both the servers are busy. Insert element into Queue: first & last
  // In case there still have people in line before you
  printf("\n 1.Check Server1 available time and status {%f, %d}", elementQ->server1->available_time, elementQ->server1->status);
  printf("\n 2.Check Server2 available time and status {%f, %d} \n", elementQ->server2->available_time, elementQ->server2->status);
  printf("\n 3. Arrival time & service time of this node {%f, %f}\n", arrival->arrival_time, arrival->service_time);
  // if (elementQ->first != NULL) {
  //   printf("Inside here");
  //   if (arrival->arrival_time >= elementQ->first->arrival_time) {
  //     printf("  \nINSERT IT IN QUEUE 1.0 \n");
  //     insertLast(elementQ, arrival);
  //     dumpEventQueue(eventQ);
  //   }
  // }
  if ( elementQ->server1->status == 1 && elementQ->server2->status == 1) {
    printf("  \nINSERT IT IN QUEUE \n");
    insertLast(elementQ, arrival);
  } 
  else {
    selectorServer(elementQ);
    StartService(elementQ, arrival, eventQ);
  }
  return arrival->next;
}

void ProcessDeparture(struct Queue *elementQ, struct QueueNode *arrival, struct EventQueue *eventQ) {
    printf("\n == Inside Process Departure == ");
    // elementQ->number_in_system--;
    printf("\n Number of customer in system %d ", elementQ->number_in_system);
    printf("\n Current time %f \n", current_time);
    printf("\n Current server time %f \n", elementQ->currentServer->available_time);
    departure_count++;
    elementQ->currentServer->status = 0;

    setSelectorStatus(current_time, elementQ);

    if (elementQ->head != NULL) {
        Delete(elementQ);
    };
    printf("\n Check Server1 available time and status {%f, %d}", elementQ->server1->available_time, elementQ->server1->status);
    printf("\n Check Server2 available time and status {%f, %d}", elementQ->server2->available_time, elementQ->server2->status);
    printf("\n == End Process Departure == ");
}
