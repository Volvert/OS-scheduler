// This is where you should implement most of your code.
// You will have to add function declarations in the header file as well to be
// able to call them from simulation.c.

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "computer.h"
#include "schedulingLogic.h"
#include "utils.h"
#include "schedulingAlgorithms.h"

#define NB_WAIT_QUEUES 1

/* --------------------------- struct definitions -------------------------- */

struct Scheduler_t
{
    // This is not the ready queues, but the ready queue algorithms
    SchedulingAlgorithm **readyQueueAlgorithms;
    int readyQueueCount;

};

/* ---------------------------- static functions --------------------------- */

// Fonction pour obtenir l'ID du processus actuellement en cours d'exécution
static int getCurrentRunningProcessId() 
{
    // Parcourez tous les processus du workload
    for (int i = 0; i < getProcessCount(workload); i++) 
    {
        int pid = getPIDFromWorkload(workload, i);
        // Vérifiez si le processus est en cours d'exécution (c'est-à-dire si son avancement est inférieur à sa durée)
        if (getProcessAdvancementTime(workload, pid) < getProcessDuration(workload, pid)) 
        {
            return pid; // Retournez l'ID du processus actuellement en cours d'exécution
        }
    }
    return -1; // Aucun processus n'est actuellement en cours d'exécution
}

// Fonction pour obtenir l'ID du prochain processus prêt à être exécuté
static int getNextReadyProcessId() 
{
    int currentTime = getCurrentTime(); // Obtenez le temps actuel de la simulation
    int shortestStartTime = INT_MAX;
    int nextReadyProcessId = -1;

    // Parcourez tous les processus du workload
    for (int i = 0; i < getProcessCount(workload); i++) 
    {
        int pid = getPIDFromWorkload(workload, i);
        int startTime = getProcessStartTime(workload, pid);
        // Vérifiez si le processus est prêt à être exécuté (c'est-à-dire si son heure de début est inférieure ou égale au temps actuel)
        if (startTime <= currentTime && getProcessAdvancementTime(workload, pid) < getProcessDuration(workload, pid)) 
        {
            // Vérifiez si l'heure de début de ce processus est la plus courte jusqu'à présent
            if (startTime < shortestStartTime) 
            {
                shortestStartTime = startTime;
                nextReadyProcessId = pid; // Mettez à jour l'ID du prochain processus prêt à être exécuté
            }
        }
    }
    return nextReadyProcessId;
}


/* -------------------------- getters and setters -------------------------- */

int getWaitQueueCount(void)
{
    return NB_WAIT_QUEUES;
}

/* -------------------------- init/free functions -------------------------- */

Scheduler *initScheduler(SchedulingAlgorithm **readyQueueAlgorithms, int readyQueueCount)
{
    Scheduler *scheduler = malloc(sizeof(Scheduler));
    if (!scheduler)
    {
        return NULL;
    }

    scheduler->readyQueueAlgorithms = readyQueueAlgorithms;
    scheduler->readyQueueCount = readyQueueCount;

    return scheduler;
}

void freeScheduler(Scheduler *scheduler)
{
    for (int i = 0; i < scheduler->readyQueueCount; i++)
    {
        free(scheduler->readyQueueAlgorithms[i]);
    }
    free(scheduler->readyQueueAlgorithms);
    free(scheduler);
}

/* -------------------------- scheduling functions ------------------------- */

// Fonction SJF (Shortest Job First)
PCB* sjf(Workload *workload) 
{
    int shortestTimeLeft = INT_MAX;
    PCB *shortestProcess = NULL;

    // Parcourez la liste des processus disponibles dans le workload
    for (int i = 0; i < getProcessCount(workload); i++) 
    {
        int pid = getPIDFromWorkload(workload, i);
        int timeLeft = getProcessCurEventTimeLeft(workload, pid);

        // Vérifiez si le processus a un temps d'exécution restant plus court
        if (timeLeft < shortestTimeLeft) 
        {
            shortestTimeLeft = timeLeft;
            shortestProcess = getProcessFromWorkload(workload, pid);
        }
    }

    return shortestProcess;
}// Fin de sjf


// Fonction RR (Round Robin)
PCB* rr(Workload *workload, int *currentTime, int timeSlice) 
{
    int currentPid = getCurrentRunningProcessId(); // Fonction pour obtenir l'ID du processus en cours d'exécution
    int nextPid = getNextReadyProcessId(); // Fonction pour obtenir l'ID du prochain processus prêt

    // Vérifiez si le processus en cours d'exécution a terminé sa slice time
    if (*currentTime % timeSlice == 0 && currentPid != -1) 
    {
        // Vérifiez s'il y a un autre processus prêt à être exécuté
        if (nextPid != -1) 
        {
            // Le processus actuel est préempté par un autre processus
            // Commencez une nouvelle slice time à partir de 0
            *currentTime = 0;
            return getProcessFromWorkload(workload, nextPid);
        }
        // Aucun autre processus prêt, donc le processus actuel continue avec une nouvelle slice time
        return getProcessFromWorkload(workload, currentPid);
    }
    // Aucune slice time terminée ou processus actif, continue avec le processus suivant s'il y en a un
    if (nextPid != -1) 
    {
        return getProcessFromWorkload(workload, nextPid);
    }
    // Aucun processus prêt, reste sur le processus actuel (s'il y en a un)
    if (currentPid != -1) 
    {
        return getProcessFromWorkload(workload, currentPid);
    }
    // Aucun processus en cours d'exécution et aucun processus prêt
    return NULL;
}// Fin de rr
