#include "statistics.h"
#include <stdio.h>

static double computed_stats[4];  // Store computed statistics: [E(n), E(r), E(w), p0]
static double simulated_stats[4]; // Store simulated statistics [n, r, w, sim_p0]
int departure_count = 0;         // current number of departures from queue
double current_time = 0;          // current time during simulation
double last_event = 0;

void GenerateComputedStatistics(double lambda, double mu) {
    int m = 2;
    double p = lambda / (2*mu); // p
    double p0 = 1 / ( 1 + ( ((m * p) * (m*p)) / (2*(1-p)) + (m*p) ));
    double w = p0 * (m*p)*(m*p) / ( 2 * (1-p));

    computed_stats[0] = (p*w)/(1-p) + m*p; // n
    computed_stats[1] = computed_stats[0] / lambda; // r
    computed_stats[2] = w / (m*mu*(1-p)); // w
    computed_stats[3] = p0; // p0
}

void PrintStatistics(struct Queue *elementQ, int total_departures) {
    printf("\n");
    if(departure_count == total_departures) printf("End of Simulation - after %d departures\n", departure_count);
    else printf("After %d departures\n", departure_count);

    printf("Mean n = %.4f (Simulated) and %.4f (Computed)\n", simulated_stats[0], computed_stats[0]);
    printf("Mean r = %.4f (Simulated) and %.4f (Computed)\n", simulated_stats[1], computed_stats[1]);
    printf("Mean w = %.4f (Simulated) and %.4f (Computed)\n", simulated_stats[2], computed_stats[2]);
    printf("p0 = %.4f (Simulated) and %.4f (Computed)\n", simulated_stats[3], computed_stats[3]);
}
