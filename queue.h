#ifndef QUEUE_H
#define QUEUE_H

struct Server {
    int status;  // status = 1: busy, status = 0: idle
    double available_time;
};

struct QueueNode {
    double arrival_time;
    double service_time;
    struct QueueNode *next;
};

struct Queue {
    struct QueueNode *head;
    struct QueueNode *tail;
    struct QueueNode *first;
    struct QueueNode *last;
    int waiting_count;
    double cumulative_response;
    double cumulative_waiting;
    double cumulative_idle_times;
    double cumulative_number;
    int number_in_system;
    double cumulative_busy_time;
    struct Server *currentServer;
    struct Server *server1;
    struct Server *server2;
};

void Insert(struct Queue *q, double arrival_time, double service_time);
void Delete(struct Queue *q);
void InitializeQueue(struct Queue *elementQ, int seed, double lambda, double mu, int total_departures);
void FreeQueue(struct Queue *elementQ);

#endif // QUEUE_H
