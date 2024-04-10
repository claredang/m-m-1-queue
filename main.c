#include "queue.h"
#include "event.h"
#include "statistics.h"
#include "simulator.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc >= 6) {
        double lambda = atof(argv[1]);
        double mu = atof(argv[2]);
        int print_period = atoi(argv[3]);
        int total_departures = atoi(argv[4]);
        int random_seed = atoi(argv[5]);

        // Initialize the queue
        struct Queue *elementQ = malloc(sizeof(struct Queue));
        InitializeQueue(elementQ, random_seed, lambda, mu, total_departures);

        // Start simulation
        Simulation(elementQ, lambda, mu, print_period, total_departures);

        // Compute and print statistics
        GenerateComputedStatistics(lambda, mu);
        PrintStatistics(elementQ, total_departures);

        // Free allocated memory
        FreeQueue(elementQ);
    } else {
        printf("Insufficient number of arguments provided!\n");
    }

    return 0;
}
