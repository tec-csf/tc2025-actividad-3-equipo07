// Code from Operating Systems class was taken as reference
// Coded by professor Jorge
// Operating Systems Spring 2020

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

//Time slice of CPU
#define QUANTUM 5

int countProcess = 0;
int countGang = 0;

typedef struct Processes
{
    int id;
    int remainingTime;
    struct Processes *next;
} Process;

typedef struct Gangs
{
    int id;
    int numberOfProcesses;
    struct Processes *initialProcess;
    struct Gangs *nextGang;

} Gang;

Gang *front;

Process *createProcess(int id, int remainingTime)
{
    Process *toAdd = (Process *)malloc(sizeof(Process));
    toAdd->id = id;
    toAdd->remainingTime = remainingTime;
    toAdd->next = NULL;
    return toAdd;
}
Gang *createGang(int id, int numberOfProcesses, Process *initialProcess)
{
    Gang *toAdd = (Gang *)malloc(sizeof(Gang));
    toAdd->id = id;
    toAdd->initialProcess = initialProcess;
    toAdd->nextGang = NULL;
    toAdd->numberOfProcesses = numberOfProcesses;
    return toAdd;
}
void enqueueProcess(Process *toAdd, Gang *gang)
{
    if (gang->initialProcess == NULL)
    {
        gang->initialProcess = toAdd;
    }
    else
    {
        Process *temp = gang->initialProcess;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = toAdd;
    }
    countProcess++;
}
void enqueueGang(Gang *gang)
{
    if (front == NULL)
    {
        front = gang;
    }
    else
    {
        Gang *temp = front;
        while (temp->nextGang != NULL)
        {
            temp = temp->nextGang;
        }
        temp->nextGang = gang;
    }
    countGang++;
}

Process *dequeueProcess(Gang *gang)
{
    Process *toReturn = gang->initialProcess;
    gang->initialProcess = gang->initialProcess->next;
    countProcess--;
    toReturn->next = NULL;
    return toReturn;
}
Gang *dequeueGang()
{
    Gang *toReturn = front;
    front = front->nextGang;
    toReturn->nextGang = NULL;
    countGang--;
    return toReturn;
}
//CPU process
void computeProcessByGang(Gang *gang, int cpus)
{
    printf("---------------- Gang %d ---------------\n", gang->id);
    int totalProcesses = gang->numberOfProcesses;
    for (int i = 0; i < cpus; i++)
    {
        if (totalProcesses != 0)
        {
            Process *aux = dequeueProcess(gang);
            printf("Executing process number %d in cpu %d\n", aux->id, i);
            aux->remainingTime -= QUANTUM;
            sleep(3);
            if (aux->remainingTime > 0)
            {
                printf("Process %d CPU time execution finished and got preempted. Remaining time: %d\n", aux->id, aux->remainingTime);
                enqueueProcess(aux, gang);
            }
            else
            {
                //Process finished
                printf("Process %d finished.\n", aux->id);
                gang->numberOfProcesses--;
                // Freeing memory
                free(aux);
            }
            //Decreading number of processes in the gang
            totalProcesses--;
        }
    }
}

int main()
{
    srand(time(0));

    printf("Time slice is %d\n", QUANTUM);

    int processes;
    int cpus;
    int gangs;
    int processesInGang;
    int processTime;

    printf("How many processes you want?\n");
    scanf("%d", &processes);

    printf("How many cpus you want?\n");
    scanf("%d", &cpus);

    printf("How many gangs\n");
    scanf("%d", &gangs);

    Gang *newGang;
    int processID = 1;
    // Creating gangs
    for (int i = 0; i < gangs; ++i)
    {
        printf("Enter number of process in the gang %d:\n", i);
        scanf("%d", &processesInGang);

        newGang = createGang(i, processesInGang, NULL);
        // Creating processes in gang
        Process *aux;
        for (int j = 0; j < processesInGang; ++j)
        {
            printf("How many time of CPU does process %d will consume?\n", j+1);
            scanf("%d", &processTime);
            aux = createProcess(processID, processTime);
            processID++;
            enqueueProcess(aux, newGang);
        }
        // Gang packed up
        enqueueGang(newGang);
    }
    
    Gang *currentGang;
    while (countGang > 0)
    {
        currentGang = dequeueGang();

        computeProcessByGang(currentGang, cpus);

        // Checking for any processes left in gang
        if (currentGang->initialProcess != NULL)
        {
            enqueueGang(currentGang);
        }
        else
        {
            // Freeing gang
            free(currentGang);
            countGang--;
        }
    }

    return 0;
}