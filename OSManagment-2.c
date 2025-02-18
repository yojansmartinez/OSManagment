// OSManagment.cpp Yojans Martinez OS Managment
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <limits.h>



#define EXIT 0
#define INVALID -1
#define CPU 1
#define RACE 2
#define BANKER 3
#define PROCESS 5
#define RESOURCE 3
#define TRUE 1
#define FALSE 0
#define MEMORY 4
#define FIRST 0
#define BEST 1
#define WORST 2
#define NEXT 3
#define PAGE 5
#define FIFO 0
#define LRU 1
#define FRAMES 4
#define DISK 6
#define FCFS 0
#define SSTF 1
#define REQUEST 8


int pid[] = { 1, 2, 3, 4, 5 };
int arrivalt[] = { 0, 2, 4, 6, 7 };
int burstt[] = { 8, 5, 10, 2, 3 };
int resource = 5;

//updated function prototypes
void diskScheduling();
void diskFcfs(int requests[], int head);
void disk_Sstf(int requests[], int head);
//functions from previous assignments
void pageReplacement();
void lru();
int findLRU(int time[]);
void fifo();
void displayPages(int page, int allocation[]);
void memorymanagement();
void displayprocess(int allocation[], int processes, int processSize[]);
void firstfit(int blocksize[], int blocks, int process_size[], int processes);
void worstfit(int blocksize[], int blocks, int process_size[], int processes);
void bestfit(int blocksize[], int blocks, int process_size[], int processes);
void nextfit(int blocksize[], int blocks, int process_size[], int processes);
int displaymenu();
void cpuscheduling();
void fcfs(int pid[], int arrivalt[], int burstt[]);
void sjf(int pid[], int arrivalt[], int burstt[]);
void displayschedule(int pid[], int arrivalt[], int burstt[], int waitt[], int turnat[]);
void racecondition();
void threadfunc1(void* arg);
void threadfunc2(void* arg);
void bankersalgorithm();
void systemstate(int feasible[], int safe[]);
//main function
int main(void) {
    int choice = INVALID;

    while (choice != EXIT) {
        choice = displaymenu();

        switch (choice) {
        case CPU:
            cpuscheduling();
            break;
        case RACE:
            racecondition();
            break;
        case BANKER:
            bankersalgorithm();
            break;
        case MEMORY:
            memorymanagement();
            break;
        case PAGE:
            pageReplacement();
            break;
        case DISK: // Added Disk Scheduling
            diskScheduling();
            break;
        case EXIT:
            exit(0);
            break;
        default:
            printf("Invalid, please try again.\n");
            break;
        }
    }

    return EXIT;
}
//display menu for user input
int displaymenu() {
    int choice = INVALID;

    while (choice == INVALID) {
        printf("Select the OS program to run, enter the number of your choice.\n");
        printf("1 - CPU Scheduling\n");
        printf("2 - Race Condition\n");
        printf("3 - Banker's Algorithm\n");
        printf("4 - Memory Management\n");
        printf("5 - Page Replacement\n");
        printf("6 - Disk Scheduling\n");
        printf("0 - Exit\n");
        scanf("%d", &choice);

        if (choice < 0 || choice > DISK) {
            choice = INVALID;
        }
    }

    return choice;
}
//function for cpu scheudling chart
void cpuscheduling() {
    fcfs(pid, arrivalt, burstt);
    sjf(pid, arrivalt, burstt);
}
//fcfs algo function
void fcfs(int pid[], int arrivalt[], int burstt[]) {
    int waitt[PROCESS];
    int turnat[PROCESS];
    waitt[0] = 0;

    for (int i = 1; i < PROCESS; i++) {
        waitt[i] = burstt[i - 1] + waitt[i - 1];
    }

    for (int i = 0; i < PROCESS; i++) {
        turnat[i] = burstt[i] + waitt[i];
    }

    displayschedule(pid, arrivalt, burstt, waitt, turnat);
}
//sjf algo function
void sjf(int pid[], int arrivalt[], int burstt[]) {
    int waitt[PROCESS];
    int turnat[PROCESS];
    int idx, temp;

    for (int i = 0; i < PROCESS; i++) {
        idx = i;
        for (int j = i + 1; j < PROCESS; j++) {
            if (burstt[j] < burstt[idx]) {
                idx = j;
            }
        }

        temp = burstt[i];
        burstt[i] = burstt[idx];
        burstt[idx] = temp;

        temp = pid[i];
        pid[i] = pid[idx];
        pid[idx] = temp;

        temp = arrivalt[i];
        arrivalt[i] = arrivalt[idx];
        arrivalt[idx] = temp;
    }

    waitt[0] = 0;
    for (int i = 1; i < PROCESS; i++) {
        waitt[i] = burstt[i - 1] + waitt[i - 1];
    }

    for (int i = 0; i < PROCESS; i++) {
        turnat[i] = burstt[i] + waitt[i];
    }

    displayschedule(pid, arrivalt, burstt, waitt, turnat);
}
//display schedule func
void displayschedule(int pid[], int arrivalt[], int burstt[], int waitt[], int turnat[]) {
    int totwait = 0, totturnat = 0;
    float avgwaitt, avgturnat;

    printf("\nPID   \tAT          \tBT       \tWT        \tTAT         \n");
    for (int i = 0; i < PROCESS; i++) {
        totwait += waitt[i];
        totturnat += turnat[i];
        printf("%d\t%d\t\t%d\t\t%d\t\t%d\n", pid[i], arrivalt[i], burstt[i], waitt[i], turnat[i]);
    }

    avgwaitt = (float)totwait / PROCESS;
    avgturnat = (float)totturnat / PROCESS;

    printf("Average WT is %.2f\n", avgwaitt);
    printf("Average TAT is  %.2f\n", avgturnat);
}
//race condition func
void racecondition() {
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, threadfunc1, NULL);
    pthread_create(&thread2, NULL, threadfunc2, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Final value of shared resource is %d\n", resource);

    pthread_exit(NULL);
}
//first thread func
void threadfunc1(void* arg) {
    int threadid = 1;
    int f1 = resource;

    printf("Thread %d reads the value of shared resource as %d\n", threadid, f1);
    f1++;
    printf("Local update by Thread %d: %d\n", threadid, f1);
    sleep(1);
    resource = f1;
    printf("Value of shared resources updated by Thread %d is: %d\n", threadid, resource);

    pthread_exit(NULL);
}
//second func
void threadfunc2(void* arg) {
    int threadid = 2;
    int f2 = resource;

    printf("Thread %d reads resource as %d\n", threadid, f2);
    f2--;
    printf("Local update by Thread %d: %d\n", threadid, f2);
    sleep(1);
    resource = f2;
    printf("Value of shared resources updated by Thread %d is: %d\n", threadid, resource);

    pthread_exit(NULL);
}
//bankers algo function
void bankersalgorithm() {
    int maxm[RESOURCE][PROCESS] = { {4, 5, 3, 4, 7}, {3, 3, 2, 1, 6}, {3, 2, 1, 5, 2} };
    int allot[RESOURCE][PROCESS] = { {2, 1, 1, 0, 2}, {1, 0, 2, 1, 3}, {2, 1, 0, 4, 2} };
    int need[RESOURCE][PROCESS];
    int available[RESOURCE] = { 1, 0, 2 };
    int safe[PROCESS];
    int work[RESOURCE];
    int finish[PROCESS];

    for (int i = 0; i < PROCESS; i++) {
        finish[i] = FALSE;
    }

    for (int i = 0; i < RESOURCE; i++) {
        work[i] = available[i];
    }

    for (int i = 0; i < RESOURCE; i++) {
        for (int j = 0; j < PROCESS; j++) {
            need[i][j] = maxm[i][j] - allot[i][j];
        }
    }

    int count = 0;
    int safeSeq[PROCESS];
    while (count < PROCESS) {
        int found = FALSE;
        for (int p = 0; p < PROCESS; p++) {
            if (finish[p] == FALSE) {
                int j;
                for (j = 0; j < RESOURCE; j++) {
                    if (need[j][p] > work[j]) {
                        break;
                    }
                }

                if (j == RESOURCE) {
                    for (int k = 0; k < RESOURCE; k++) {
                        work[k] += allot[k][p];
                    }
                    safeSeq[count++] = p;
                    finish[p] = TRUE;
                    found = TRUE;
                }
            }
        }

        if (found == FALSE) {
            printf("\nThe system is not in a safe state\n");
            return;
        }
    }

    printf("\nThe system is in a safe state\n");
    printf("Safe sequence is: ");
    for (int i = 0; i < PROCESS; i++) {
        printf("%d ", safeSeq[i]);
    }
    printf("\n");

    systemstate(finish, safeSeq);
}
//funxtion for systemstate
void systemstate(int finish[], int safeSeq[]) {
    int maxm[RESOURCE][PROCESS] = { {4, 5, 3, 4, 7}, {3, 3, 2, 1, 6}, {3, 2, 1, 5, 2} };
    int allot[RESOURCE][PROCESS] = { {2, 1, 1, 0, 2}, {1, 0, 2, 1, 3}, {2, 1, 0, 4, 2} };
    int need[RESOURCE][PROCESS];
    int available[RESOURCE] = { 1, 0, 2 };
    int work[RESOURCE];

    for (int i = 0; i < RESOURCE; i++) {
        work[i] = available[i];
    }

    for (int i = 0; i < RESOURCE; i++) {
        for (int j = 0; j < PROCESS; j++) {
            need[i][j] = maxm[i][j] - allot[i][j];
        }
    }

    printf("\nProcess\t\tAllocation\tMax\t\tNeed\t\tAvailable\n");
    for (int i = 0; i < PROCESS; i++) {
        printf("P%d\t\t", i);
        for (int j = 0; j < RESOURCE; j++) {
            printf("%d ", allot[j][i]);
        }
        printf("\t\t");
        for (int j = 0; j < RESOURCE; j++) {
            printf("%d ", maxm[j][i]);
        }
        printf("\t\t");
        for (int j = 0; j < RESOURCE; j++) {
            printf("%d ", need[j][i]);
        }
        printf("\t\t");
        if (finish[i] == FALSE) {
            printf("Not finished\n");
        }
        else {
            printf("Finished\n");
        }
    }

    printf("\nCurrent available resources: ");
    for (int i = 0; i < RESOURCE; i++) {
        printf("%d ", work[i]);
    }
    printf("\n");

    printf("\nResource Allocation Graph:\n");
    printf("  |");
    for (int i = 0; i < PROCESS; i++) {
        printf(" P%d |", i);
    }
    printf("\n");

    for (int i = 0; i < RESOURCE; i++) {
        printf("--+");
        for (int j = 0; j < PROCESS; j++) {
            printf("----");
        }
        printf("\n");
        printf("R%d|", i);
        for (int j = 0; j < PROCESS; j++) {
            if (allot[i][j] != 0) {
                printf(" %d  |", allot[i][j]);
            }
            else {
                printf("    |");
            }
        }
        printf("\n");
    }

    printf("\n");
}
// I had to split up the memory management code into two functions because I couldn't get the memory management to work properly. If I displayed all at once it would put not allocated for so many block #'s.'
void memorymanagement() {
    int blocksize[] = { 70, 20, 45, 65, 40, 80 };
    int process_size[] = { 15, 35, 25, 45, 60, 20 };
    int blocks = 6;
    int process_num = 6;
    int fit;
    //Brings up options menue to choose which option
    printf("Please select which fit you would like to see \n");
    printf("Press 0 for First Fit\n");
    printf("Press 1 for Best Fit\n");
    printf("Press 2 for Worst Fit\n");
    printf("Press 3 for Next Fit\n");
    scanf("%d", &fit);

    switch (fit) {
    case FIRST:
        printf("\n******** First Fit ********\n");
        firstfit(blocksize, blocks, process_size, process_num);
        break;
    case BEST:
        printf("\n******** Best Fit ********\n");
        bestfit(blocksize, blocks, process_size, process_num);
        break;
    case WORST:
        printf("\n******** Worst Fit ********\n");
        worstfit(blocksize, blocks, process_size, process_num);
        break;
    case NEXT:
        printf("\n******** Next Fit ********\n");
        nextfit(blocksize, blocks, process_size, process_num);
        break;
    default:
        printf("Invalid choice, please try again.\n");
        break;
    }
}
//function for display of memory management
void displayprocess(int allocation[], int process_num, int process_size[]) {
    printf("\nProcess #\tProcess size\tBlock #\n");
    for (int i = 0; i < process_num; i++) {
        printf("%d\t\t        %d\t\t", i + 1, process_size[i]);
        if (allocation[i] != -1) {
            printf("      %d\n", allocation[i] + 1);
        }
        else {
            printf("Not Allocated\n");
        }
    }
}
//function for first fit
void firstfit(int blocksize[], int blocks, int process_size[], int process_num) {
    int allocation[process_num];
    memset(allocation, -1, sizeof(allocation));

    for (int i = 0; i < process_num; i++) {
        for (int j = 0; j < blocks; j++) {
            if (blocksize[j] >= process_size[i]) {
                allocation[i] = j;
                blocksize[j] -= process_size[i];
                break;
            }
        }
    }

    displayprocess(allocation, process_num, process_size);
}
//function for best fit
void bestfit(int blocksize[], int blocks, int process_size[], int process_num) {
    int allocation[process_num];
    memset(allocation, -1, sizeof(allocation));

    for (int i = 0; i < process_num; i++) {
        int bestIdx = -1;
        for (int j = 0; j < blocks; j++) {
            if (blocksize[j] >= process_size[i]) {
                if (bestIdx == -1)
                    bestIdx = j;
                else if (blocksize[bestIdx] > blocksize[j])
                    bestIdx = j;
            }
        }
        if (bestIdx != -1) {
            allocation[i] = bestIdx;
            blocksize[bestIdx] -= process_size[i];
        }
    }

    displayprocess(allocation, process_num, process_size);
}
//function for worst fit
void worstfit(int blocksize[], int blocks, int process_size[], int process_num) {
    int allocation[process_num];
    memset(allocation, -1, sizeof(allocation));

    for (int i = 0; i < process_num; i++) {
        int worstIdx = -1;
        for (int j = 0; j < blocks; j++) {
            if (blocksize[j] >= process_size[i]) {
                if (worstIdx == -1)
                    worstIdx = j;
                else if (blocksize[worstIdx] < blocksize[j])
                    worstIdx = j;
            }
        }
        if (worstIdx != -1) {
            allocation[i] = worstIdx;
            blocksize[worstIdx] -= process_size[i];
        }
    }

    displayprocess(allocation, process_num, process_size);
}
//function for next fit
void nextfit(int blocksize[], int blocks, int process_size[], int process_num) {
    int allocation[process_num];
    memset(allocation, -1, sizeof(allocation));

    int id = 0;

    for (int i = 0; i < process_num; i++) {
        while (id < blocks) {
            if (blocksize[id] >= process_size[i]) {
                allocation[i] = id;
                blocksize[id] -= process_size[i];
                break;
            }
            id = (id + 1) % blocks;
        }
    }

    displayprocess(allocation, process_num, process_size);
}
void pageReplacement() {
    //used to call both fifo and lru
    printf("\n******** Page Replacement ********\n");
    printf("******** First In First Out ********\n");
    printf("\nPage  F1 F2  F3  F4\n");
    fifo();

    printf("\n******** Least Recently Used ********\n");
    printf("\nPage  F1 F2  F3  F4\n");
    lru();
}
//function for fifo
void fifo() {
    int pageRequests[] = { 2, 3, 8, 4, 5, 6, 5, 7, 1, 8, 3, 1, 4, 2, 6 };
    int pageFaults = 0;
    int allocation[FRAMES];
    int present = 0;
    int pages = sizeof(pageRequests) / sizeof(pageRequests[0]);

    memset(allocation, -1, sizeof(allocation));

    for (int i = 0; i < pages; i++) {
        present = 0;
        for (int j = 0; j < FRAMES; j++) {
            if (allocation[j] == pageRequests[i]) {
                present = 1;
                break;
            }
        }
        if (present == 0) {
            allocation[pageFaults % FRAMES] = pageRequests[i];
            pageFaults++;
        }

        printf("\n%d     ", pageRequests[i]);
        for (int k = 0; k < FRAMES; k++) {
            if (allocation[k] == -1) {
                printf("-   ");
            }
            else {
                printf("%d   ", allocation[k]);
            }
        }
    }

    printf("\nTotal Page Faults: %d\n", pageFaults);
}
//function for lru
void lru() {
    int pageRequests[] = { 2, 3, 8, 4, 5, 6, 5, 7, 1, 8, 3, 1, 4, 2, 6 };
    int pageFaults = 0;
    int allocation[FRAMES];
    int counter = 0;
    int pages = sizeof(pageRequests) / sizeof(pageRequests[0]);
    int time[10];
    int flagone, flagtwo, position = 0;

    memset(allocation, -1, sizeof(allocation));
    memset(time, -1, sizeof(time));

    for (int i = 0; i < pages; i++) {
        flagone = 0;
        flagtwo = 0;
        for (int j = 0; j < FRAMES; j++) {
            if (allocation[j] == pageRequests[i]) {
                counter++;
                time[j] = counter;
                flagone = 1;
                flagtwo = 1;
                break;
            }
        }

        if (flagone == 0) {
            for (int j = 0; j < FRAMES; j++) {
                if (allocation[j] == -1) {
                    counter++;
                    pageFaults++;
                    allocation[j] = pageRequests[i];
                    time[j] = counter;
                    flagtwo = 1;
                    break;
                }
            }
        }

        if (flagtwo == 0) {
            position = findLRU(time);
            counter++;
            pageFaults++;
            allocation[position] = pageRequests[i];
            time[position] = counter;
        }

        displayPages(pageRequests[i], allocation);
    }

    printf("\nTotal Page Faults: %d\n", pageFaults);
}
//function fot findLRU
int findLRU(int time[]) {
    int position = 0;
    int minimum = time[0];

    for (int i = 0; i < FRAMES; i++) {
        if (time[i] < minimum) {
            minimum = time[i];
            position = i;
        }
    }

    return position;
}
//function display pages
void displayPages(int page, int allocation[]) {
    printf("\n%d     ", page);
    for (int i = 0; i < FRAMES; i++) {
        if (allocation[i] == -1) {
            printf("-   ");
        }
        else {
            printf("%d   ", allocation[i]);
        }
    }
}
//function for disk scheduling
void diskScheduling() {
    //data set provided
    int requests[] = { 146, 89, 24, 70, 102, 13, 51, 134 };
    int head = 50;
    //print function to display headers 
    printf("\n******** Disk Scheduling ********\n");
    printf("\n******** FCFS ********\n");
    diskFcfs(requests, head);

    printf("\n******** SSTF ********\n");
    disk_Sstf(requests, head);
}
//function for first disk function fcfs
void diskFcfs(int requests[], int head) {
    int seek = 0;

    printf("Seek sequence: %d", head);
    for (int i = 0; i < REQUEST; i++) {
        seek += abs(head - requests[i]);
        printf(" -> %d", requests[i]);
        head = requests[i];
    }

    printf("\nTotal seek operations: %d\n", seek);
}
//second function for disk scheduling, sstf
void disk_Sstf(int requests[], int head) {
    int sequence[REQUEST];
    int seek = 0;

    for (int i = 0; i < REQUEST; i++) {
        sequence[i] = requests[i];
    }

    for (int i = 0; i < REQUEST; i++) {
        int mindistance = INT_MAX;
        int minIdx = -1;

        for (int j = 0; j < REQUEST; j++) {
            if (sequence[j] != -1) {
                int dist = abs(head - sequence[j]);
                if (dist < mindistance) {
                    mindistance = dist;
                    minIdx = j;
                }
            }
        }

        printf("%d -> ", head);


        seek += mindistance;
        head = sequence[minIdx];

        sequence[minIdx] = -1;
    }

    printf("%d\n", head);

    printf("Total seek operations: %d\n", seek);
}
