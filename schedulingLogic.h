#ifndef schedulingLogic_h
#define schedulingLogic_h

#include <stdbool.h>

#include "graph.h"
#include "stats.h"
#include "simulation.h"

// Cannot include computer.h because of circular dependency
// -> forward declaration of Computer
typedef struct Computer_t Computer;


/* ---------------------------- Scheduler struct ---------------------------- */

typedef struct Scheduler_t Scheduler;


/* ---------------------------- Static function ---------------------------- */

static int getCurrentRunningProcessId(); // Fonction pour obtenir l'ID du processus actuellement en cours d'exécution


static int getNextReadyProcessId(); // Fonction pour obtenir l'ID du prochain processus prêt à être exécuté


/* -------------------------- getters and setters -------------------------- */

int getWaitQueueCount(void);

/* -------------------------- init/free functions -------------------------- */

Scheduler *initScheduler(SchedulingAlgorithm **readyQueueAlgorithms, int readyQueueCount);
void freeScheduler(Scheduler *scheduler);


/* -------------------------- scheduling functions ------------------------- */


PCB* sjf(Workload *workload); // Fonction SJF (Shortest Job First)


PCB* rr(Workload *workload, int *currentTime, int timeSlice); // Fonction RR (Round Robin)

#endif // schedulingLogic_h
