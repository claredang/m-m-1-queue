#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "queue.h"
#include "event.h"

void Simulation(struct Queue *elementQ, double lambda, double mu, int print_period, int total_departures);
struct QueueNode *ProcessArrival(struct Queue *elementQ, struct QueueNode *arrival, struct EventQueue *eventQ);
void ProcessDeparture(struct Queue *elementQ, struct QueueNode *arrival, struct EventQueue *eventQ);

#endif // SIMULATOR_H
