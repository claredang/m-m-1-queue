#ifndef STATISTICS_H
#define STATISTICS_H

#include "queue.h"

void GenerateComputedStatistics(double lambda, double mu);
void PrintStatistics(struct Queue *elementQ, int total_departures);

#endif // STATISTICS_H
