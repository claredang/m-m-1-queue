# Simulation System

This repository contains a simulation system written in C that models a queueing system with event-based simulation. The system is modularized into separate source files and utilizes header files to declare function prototypes and structures for different components of the simulation.

## Project Structure

The project is structured into several modules:

### queue

- `queue.h`: Header file declaring queue-related data structures (`Queue`, `QueueNode`, `Server`) and function prototypes for queue operations (`Insert`, `Delete`, `InitializeQueue`, etc.).
- `queue.c`: Source file containing the implementation of queue operations.

### event

- `event.h`: Header file declaring event-related data structures (`EventQueue`, `EventQueueNode`) and function prototypes for event queue operations (`InsertEvent`, `EventDelete`, etc.).
- `event.c`: Source file containing the implementation of event queue operations.

### statistics

- `statistics.h`: Header file declaring function prototypes for computing and printing statistics (`GenerateComputedStatistics`, `PrintStatistics`).
- `statistics.c`: Source file containing the implementation of statistical computations.

### simulator

- `simulator.h`: Header file declaring function prototypes for main simulation functions (`Simulation`, `ProcessArrival`, `ProcessDeparture`).
- `simulator.c`: Source file containing the main simulation logic.

### main

- `main.c`: Main program that includes necessary headers and orchestrates the simulation.

## How to Build and Run

To compile and build the simulation system, use a C compiler (e.g., GCC). Here's an example compilation command:

```bash
gcc -o simulator main.c queue.c event.c statistics.c simulator.c
```

This will compile all the source files and generate an executable named simulator.

To run the simulation, execute the compiled simulator executable with the required command-line arguments:

```bash
./simulator <lambda> <mu> <print_period> <total_departures> <random_seed>
```

```bash
./simulator 0.5 0.8 100 1000 12345
```

## Simulation Output

The simulation will run and produce output based on the specified parameters. At the end of the simulation, computed statistics will be printed to the console.
