#include<stdio.h>
#include<time.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>
#include<assert.h>
#include<float.h>


// Definition of a Queue Node including arrival and service time
struct Server {
  int status;  // status = 1: busy, status = 0: idle
  double available_time;
};

struct QueueNode {
    double arrival_time;  // customer arrival time, measured from time t=0, inter-arrival times exponential
    double service_time;  // customer service time (exponential) 
    struct QueueNode *next;  // next element in line; NULL if this is the last element
};

// Suggested queue definition
// Feel free to use some of the functions you implemented in HW2 to manipulate the queue
// You can change the queue definition and add/remove member variables to suit your implementation
struct Queue {

    struct QueueNode* head;    // Point to the first node of the element queue
    struct QueueNode* tail;    // Point to the tail node of the element queue

    struct QueueNode* first;  // Point to the first arrived customer that is waiting for service
    struct QueueNode* last;   // Point to the last arrrived customer that is waiting for service
    int waiting_count;     // Current number of customers waiting for service

    double cumulative_response;  // Accumulated response time for all effective departures
    double cumulative_waiting;  // Accumulated waiting time for all effective departures
    double cumulative_idle_times;  // Accumulated times when the system is idle, i.e., no customers in the system
    double cumulative_number;   // Accumulated number of customers in the system

    // Add on
    int number_in_system;
    double cumulative_busy_time;
    struct Server* currentServer;
    struct Server* server1;
    struct Server* server2;
};

struct EventQueueNode {
  double event_time; // event start time
  int event_type; // Type 1: Arrival, 2: Start Service, 3: Departure
  struct QueueNode* qnode;
  struct EventQueueNode* next;
};

struct EventQueue { // What event happens next
  struct EventQueueNode* head;
  struct EventQueueNode* tail;
};

// ------------Global variables------------------------------------------------------
// Feel free to add or remove 
static double computed_stats[4];  // Store computed statistics: [E(n), E(r), E(w), p0]
static double simulated_stats[4]; // Store simulated statistics [n, r, w, sim_p0]
int departure_count = 0;         // current number of departures from queue
double current_time = 0;          // current time during simulation
double last_event = 0;

//-----------------Queue Functions------------------------------------
// Feel free to add more functions or redefine the following functions

// The following function initializes all "D" (i.e., total_departure) elements in the queue
// 1. It uses the seed value to initialize random number generator
// 2. Generates "D" exponentially distributed inter-arrival times based on lambda
//    And inserts "D" elements in queue with the correct arrival times
//    Arrival time for element i is computed based on the arrival time of element i+1 added to element i's generated inter-arrival time
//    Arrival time for first element is just that element's generated inter-arrival time
// 3. Generates "D" exponentially distributed service times based on mu
//    And updates each queue element's service time in order based on generated service times
// 4. Returns a pointer to the generated queue

// Linked list insert, delete, print
void dumpQueue(struct Queue *q)
{
  struct QueueNode *current = q->head;
  while (current != NULL) {
    printf("{%f,%f} \n", current->arrival_time, current->service_time);
    current = current->next;
  }
  printf("\n");
}

void dumpQueueLast(struct Queue *q)
{
  printf("\n === Inside dumpQueueLast === \n");
  struct QueueNode *current = q->first;
  while (current != NULL) {
    printf("{%f,%f} \n", current->arrival_time, current->service_time);
    current = current->next;
  }
  printf("\n");
}

void dumpEventQueue(struct EventQueue *q)
{
  printf("\n === Inside dumpEventQueue\n");
  struct EventQueueNode* current = q->head;
  while (current != NULL) {
    printf("Event time and type{%f, %d} \n", current->event_time, current->event_type);
    current = current->next;
  }
  printf("\n");
  
}

void sortList(struct EventQueue* eventQ) {  
  struct EventQueueNode *current = eventQ->head;
  struct EventQueueNode *index = NULL;
  double temp;
  double temp2;

  if(eventQ->head == NULL) {  
      return;  
  }  
  else {  
    while(current != NULL) {  
        index = current->next;  
        while(index != NULL) {  
            // Priority sort: by time then by type. Type 3 > Type 2 > Type 1
            if( (current->event_time > index->event_time)
            || ( (current->event_time == index->event_time) && (current->event_type < index->event_type) ) ) {    
                temp = current->event_time;  
                current->event_time = index->event_time;  
                index->event_time = temp;  
                temp2 = current->event_type;
                current->event_type = index->event_type;
                index->event_type = temp2;
            }  
            index = index->next;  
        }  
        current = current->next;  
    }      
  }  
}  

struct QueueNode* CreateNode(double arrival_time, double service_time) {
    struct QueueNode *ptr = malloc(sizeof(struct QueueNode));
    if (ptr) {
      ptr->arrival_time = arrival_time;
      ptr->service_time = service_time;
      ptr->next = NULL;
    }
    return ptr;
}

struct EventQueueNode* CreateEventNode(struct QueueNode* qnode, double event_time, int event_type) {
    struct EventQueueNode *ptr = malloc(sizeof(struct EventQueueNode));
    if (ptr) {
      ptr->event_time = event_time;
      ptr->event_type = event_type;
      ptr->qnode = NULL;
      ptr->next = NULL;
    }
    return ptr;
}

void Insert (struct Queue *q, double arrival_time, double service_time) {
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

void insertLast (struct Queue *q, struct QueueNode *arrival) {
  printf("Inside insert last \n");
  struct QueueNode* newNode = CreateNode(arrival->arrival_time, arrival->service_time);
  if (newNode != NULL) {
      if (q->first == NULL) {
          // no item in the list (the list is empty)
          q->first = newNode;
          q->last = newNode;
      }
      else {
          q->last->next = newNode;
          q->last = newNode;
      }
  }
}

void InsertEvent (struct EventQueue *eventQ, struct QueueNode* qnode, double event_time, int event_type) {
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

void Delete (struct Queue *q) {
  if (q->head != NULL) {
    struct QueueNode* toRemove = q->head;
    q->head = q->head->next;
    if (q->tail == toRemove) {
        q->tail = NULL;
    }
    free(toRemove);
  }
}

void DeleteLast (struct Queue *q) {
  if (q->first != NULL) {
    struct QueueNode* toRemove = q->first;
    q->first = q->first->next;
    if (q->last == toRemove) {
        q->last = NULL;
    }
    free(toRemove);
  }
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

int CountNodes (struct Queue *q) {
    int count = 0;
    struct QueueNode *newNode = q->first;

    while (newNode != NULL) {
        count++;
        newNode = newNode->next;
    }
    return count;
}

struct QueueNode* FindNodeAfterTime(struct Queue *q, double t) {
    struct QueueNode *newNode = q->head;
    while (newNode != NULL) {
        if (newNode->arrival_time >= t) {
            return newNode;
        }
        newNode = newNode->next;
    }
    return NULL;
}

double generateRandom(double denominator) {
    double u = ((double) rand()) / (RAND_MAX + 1.0);
    double rv = -log(1 - u) / denominator;

    // int rv = rand() % 4 + 1;
    return rv;
}

struct Queue* InitializeQueue(int seed, double lambda, double mu, int total_departures){
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
}

struct EventQueue* InitializeEventQueue() {
  struct EventQueue* eventQueue = (struct EventQueue*) malloc(sizeof (struct EventQueue));
  eventQueue->head = NULL;
  eventQueue->tail = NULL;
  return eventQueue;
}

// Use the M/M/1 formulas from class to compute E(n), E(r), E(w), p0
void GenerateComputedStatistics(double lambda, double mu){
  int m = 2;
  double p = lambda / (2*mu); // p
  double p0 = 1 / ( 1 + ( ((m * p) * (m*p)) / (2*(1-p)) + (m*p) ));
  double w = p0 * (m*p)*(m*p) / ( 2 * (1-p));

  computed_stats[0] = (p*w)/(1-p) + m*p; // n
  computed_stats[1] = computed_stats[0] / lambda; // r
  computed_stats[2] = w / (m*mu*(1-p)); // w
  computed_stats[3] = p0; // p0
}

// This function should be called to print periodic and/or end-of-simulation statistics
// Do not modify output format
void PrintStatistics(struct Queue* elementQ, int total_departures, int print_period, double lambda){
  printf("\n");
  if(departure_count == total_departures) printf("End of Simulation - after %d departures\n", departure_count);
  else printf("After %d departures\n", departure_count);

  printf("Mean n = %.4f (Simulated) and %.4f (Computed)\n", simulated_stats[0], computed_stats[0]);
  printf("Mean r = %.4f (Simulated) and %.4f (Computed)\n", simulated_stats[1], computed_stats[1]);
  printf("Mean w = %.4f (Simulated) and %.4f (Computed)\n", simulated_stats[2], computed_stats[2]);
  printf("p0 = %.4f (Simulated) and %.4f (Computed)\n", simulated_stats[3], computed_stats[3]);
}


// Choose server based on status and time available
// struct Server* selectorServer(struct Queue* q) {
void selectorServer(struct Queue* q) {
    // printf("\nserver 2 status %d", q->server2->status);
  printf("\n === Inside selector Server ===");
  printf("\n Current time %f", current_time);

  if (q->server1->status == 0 && q->server2->status == 0 && q->server1->available_time <= q->server2->available_time) {
    printf("\nchoose server 1");
    q->currentServer = q->server1;
  }
  else if (q->server1->status == 0 && q->server2->status == 0 && q->server1->available_time > q->server2->available_time) {
    printf("\n choose server 2");
    q->currentServer = q->server2;
  }
  else if (q->server1->status == 0 && q->server2->status == 1) {
    printf("\nchoose server 1 again");
    q->currentServer = q->server1;
  } 
  else if (q->server1->status == 1 && q->server2->status == 0) {
    printf("\nchoose server 2 again");
    q->currentServer = q->server2;
  }
}

// This function is called from simulator if next event is "start_service"
//  Should update queue statistics
void StartService(struct Queue* elementQ, struct QueueNode* arrival, struct EventQueue* eventQ){
  printf("\n == Inside Start Service == ");
  // Change server status to busy
  // selectorServer(elementQ);
  elementQ->currentServer->status = 1;
  printf("\nCurrent time = clock = %f", current_time);
  double arrival_time = arrival->arrival_time;
  printf("\n Node arrival time %f", arrival_time);
  double service_time = arrival->service_time;
  printf("\n Node service time %f", service_time);

  // double last_departure_that_made_queue_empty;
  double wait_time = 0;
  // Need this to test
  double response_time = 0;

  // double start_time = current_time;
  double start_time = 0;

  double arrival_time_original = arrival->arrival_time;
  
  if (elementQ->first != NULL) {
    if (arrival->arrival_time >= elementQ->first->arrival_time) {
      printf("\n Process queue first");
      arrival_time_original = elementQ->first->arrival_time;
      arrival_time = elementQ->first->arrival_time;
      service_time = elementQ->first->service_time;
      printf("\n Node arrival time %f", arrival_time);
      printf("\n Node service time %f", service_time);
      // last_departure_that_made_queue_empty = elementQ->first->arrival_time;
      // printf("\n last departure that made queue empty %f", last_departure_that_made_queue_empty);
      DeleteLast(elementQ);
    }
  }

  printf("\n Arrival time original %f", arrival_time_original);

  if (arrival_time <= current_time) {
        start_time = current_time;
  } else {
    start_time = arrival_time;
  }

  double idle_time = 0;

  if (arrival_time_original >= start_time) {
    if (arrival_time_original - elementQ->server1->available_time > 0) {
      idle_time = arrival_time_original - elementQ->server1->available_time;
      printf("\n IDLE TIME S1 %f", idle_time);
    }
    if (arrival_time_original - elementQ->server2->available_time > 0) {
      double idle_time_s2 = arrival_time_original - elementQ->server2->available_time;
      idle_time += idle_time_s2;
      printf("\n IDLE TIME S2 %f", idle_time_s2);
    }
  } 

  printf("\n Start time %f", start_time);
  printf("\n Last event %f", last_event);

  wait_time = start_time - arrival_time;
  response_time = service_time + wait_time;
  
  elementQ->cumulative_waiting += (start_time - arrival_time);
  elementQ->cumulative_response += (service_time + wait_time);
  // elementQ->cumulative_busy_time += service_time;
  elementQ->currentServer->available_time = start_time + service_time;
  elementQ->cumulative_idle_times += idle_time;

  printf("\n Finish time %f", elementQ->currentServer->available_time);
  printf("\n WAITING %f", wait_time);
  printf("\n RESPONSE %f", response_time);
  printf("\n CUMULATIVE IDLE TIME %f", elementQ->cumulative_idle_times);
  printf("\n Departure event at %f", current_time + service_time);

  struct QueueNode* findNode = FindNodeAfterTime(elementQ, (current_time + service_time));
  // Default case: Insert Depature Event for this node (case 3)
  InsertEvent(eventQ, findNode, current_time + service_time, 3);
  printf("\n Server1 available time and status {%f, %d}", elementQ->server1->available_time, elementQ->server1->status);
  printf("\n Check Server2 available time and status {%f, %d} \n", elementQ->server2->available_time, elementQ->server2->status);
}

// This function is called from simulator if the next event is an arrival
// Should update simulated statistics based on new arrival
// Should update current queue nodes and various queue member variables
// *arrival points to queue node that arrived
// Returns pointer to node that will arrive next
struct QueueNode* ProcessArrival(struct Queue* elementQ, struct QueueNode* arrival, struct EventQueue* eventQ){
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

void setSelectorStatus (double current_time, struct Queue* elementQ) {
    if (current_time < elementQ->server1->available_time) {
    elementQ->server1->status = 1;
  }
  if (current_time < elementQ->server2->available_time) {
    elementQ->server2->status = 1;
  }
  if (current_time >= elementQ->server1->available_time) {
    elementQ->server1->status = 0;
  }
  if (current_time >= elementQ->server2->available_time) {
    elementQ->server2->status = 0;
  }
}
// // This function is called from simulator if the next event is a departure
// // Should update simulated queue statistics 
// // Should update current queue nodes and various queue member variables
void ProcessDeparture(struct Queue* elementQ, struct QueueNode* arrival, struct EventQueue* eventQ){
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

void FreeEventQueue(struct EventQueue* eventQ) {
   while (eventQ->head != NULL) {
    EventDelete(eventQ);
  }
  free(eventQ);
}

// This is the main simulator function
// Should run until departure_count == total_departures
// Determines what the next event is based on current_time
// Calls appropriate function based on next event: ProcessArrival(), StartService(), ProcessDeparture()
// Advances current_time to next event
// Updates queue statistics if needed
// Print statistics if departure_count is a multiple of print_period
// Print statistics at end of simulation (departure_count == total_departure) 
void Simulation(struct Queue* elementQ, double lambda, double mu, int print_period, int total_departures){
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

// Free memory allocated for queue at the end of simulation
void FreeQueue(struct Queue* elementQ) {
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

// Program's main function
int main(int argc, char* argv[]){

  // input arguments lambda, mu, D1, D, S
  if(argc >= 6){

    double lambda = atof(argv[1]);
    double mu = atof(argv[2]);
    int print_period = atoi(argv[3]);
    int total_departures = atoi(argv[4]);
    int random_seed = atoi(argv[5]);
   
  //  // Add error checks for input variables here, exit if incorrect input
  //  // If no input errors, generate M/M/1 computed statistics based on formulas from class
  //    GenerateComputedStatistics(lambda, mu);

  //  // Start Simulation
  //  printf("Simulating M/M/1 queue with lambda = %f, mu = %f, D1 = %d, D = %d, S = %d\n", lambda, mu, print_period, total_departures, random_seed); 
    struct Queue* elementQ = InitializeQueue(random_seed, lambda, mu, total_departures);
    // dumpQueue(elementQ);
    printf("print period %d \n", print_period);

    Simulation(elementQ, lambda, mu, print_period, total_departures);

    printf("\n ==== Computed number ==== \n ");
    GenerateComputedStatistics(lambda, mu);
    printf("n = %f\n", computed_stats[0]);
    printf("r = %f\n", computed_stats[1]);
    printf("w = %f\n", computed_stats[2]);
    printf("p0 = %f\n", computed_stats[3]);

    printf("\n Simulated statistics \n");
    // simulated_stats[0] = elementQ->cumulative_response / total_departures * lambda; // n
    simulated_stats[0] = elementQ->cumulative_number / current_time; // n
    simulated_stats[1] = elementQ->cumulative_response / total_departures; // r
    simulated_stats[2] = elementQ->cumulative_waiting / total_departures; // w
    simulated_stats[3] = elementQ->cumulative_idle_times / total_departures; // p0
    printf("n = %f\n", simulated_stats[0]);
    printf("r = %f\n", simulated_stats[1]);
    printf("w = %f\n", simulated_stats[2]);
    printf("p0 = %f\n", simulated_stats[3]);

    FreeQueue(elementQ);
  }
  else printf("Insufficient number of arguments provided!\n");
   
  return 0;
}
