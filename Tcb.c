#include <stdio.h>
#include <stdlib.h>
#include "TaskList.h"
#include "InstrList.h"
#include "Tcb.h"
#include "ThreadValues.h"



struct TCB NewTask(struct TaskList *p, struct TCB TaskExec){

    TaskExec.id = p->id;
    TaskExec.arrival_time = p->arrivalTime;
    TaskExec.instr_list = StartInstr();
    
    int counterList=0;
    struct PointersInstr *pi = p->list;

    while(CheckValidInstr(pi->head,counterList)){
        AddInstr(TaskExec.instr_list, GetTypeFlag(pi->head,counterList), GetLength(pi->head,counterList));
        counterList++;
    }

    TaskExec.pc = TaskExec.instr_list->head;
    TaskExec.state = NEW;
    return TaskExec;
}

struct TCB ReadyTask(struct TCB TaskExec){
 
        TaskExec.state = READY;
        return TaskExec;
}



struct TCB RunningTask(struct TCB TaskExec){

    TaskExec.state = RUNNING;
    return TaskExec;
}

void BlockedTask(struct TCB TaskExec, struct PointersQueue * pq, int blockedTimer){

        if(pq->head == NULL){
            
            pq->head = (struct TCBqueue *)malloc(sizeof(struct TCBqueue));
            pq->head->id = TaskExec.id;
            pq->head->pc = TaskExec.pc;
            pq->head->arrival_time = TaskExec.arrival_time;
            pq->head->instr_list = TaskExec.instr_list;
            pq->head->state = BLOCKED;
            pq->head->pun = NULL;
            pq->head->blockedTimer = blockedTimer;

            pq->tail = pq->head;
       
        }else{

            pq->tail->pun = (struct TCBqueue *)malloc(sizeof(struct TCBqueue));
            pq->tail = pq->tail->pun;
            pq->tail->id = TaskExec.id;
            pq->tail->pc = TaskExec.pc;
            pq->tail->arrival_time = TaskExec.arrival_time;
            pq->tail->instr_list = TaskExec.instr_list;
            pq->tail->state = BLOCKED;
            pq->tail->pun = NULL;
            pq->tail->blockedTimer = blockedTimer;

        }
}


void CountBlockedTask(struct PointersQueue * pq){

    if(pq->head == NULL){

    }else{
        struct TCBqueue *p = pq->head;
        while(p!=NULL){
            if(p->blockedTimer > 0){
                p->blockedTimer--;
            }   
            p = p->pun;
        }
    }
}



struct TCB ResumeTask(struct PointersQueue * pq){

    struct TCB task;
    task.id = pq->head->id;
    task.pc = pq->head->pc->pun;
    task.arrival_time = pq->head->arrival_time;
    task.instr_list = pq->head->instr_list;
    task.state = READY;

    struct TCBqueue *punt;
    punt = pq->head;
 
    if(pq->head->pun != NULL){
        pq->head = pq->head->pun;
    }else{
        pq->head = NULL;
        pq->tail = NULL;
    }
    free(punt);
    return task;
}


struct TCB ExitTask(struct TCB TaskExec){

    while(TaskExec.instr_list->head != NULL){
        RemoveHeadInstr(TaskExec.instr_list);
    }
    TaskExec.state = EXIT;
    return TaskExec;
}

void ReadyQueue(struct TCB TaskExec, struct PointersReady * pr, int usedClock){
       
    if(pr->head == NULL){
        pr->head = (struct TCBreadyQueue *)malloc(sizeof(struct TCBreadyQueue));
        pr->head->id = TaskExec.id;
        pr->head->pc = TaskExec.pc;
        pr->head->arrival_time = TaskExec.arrival_time;
        pr->head->instr_list = TaskExec.instr_list;
        pr->head->state = READY;
        pr->head->pun = NULL;
        pr->tail = pr->head;
       
        int remainingClock = pr->head->pc->length - usedClock;

        if(remainingClock == 0){
            pr->head->pc = pr->head->pc->pun;
        }else{
            pr->head->pc->length = remainingClock;
        }

    }else{

        pr->tail->pun = (struct TCBreadyQueue *)malloc(sizeof(struct TCBreadyQueue));
        pr->tail = pr->tail->pun;
        pr->tail->id = TaskExec.id;
        pr->tail->pc = TaskExec.pc;
        pr->tail->arrival_time = TaskExec.arrival_time;
        pr->tail->instr_list = TaskExec.instr_list;
        pr->tail->state = READY;
        pr->tail->pun = NULL;

        int remainingClock = pr->tail->pc->length - usedClock;
    
        if(remainingClock == 0){
            pr->tail->pc = pr->tail->pc->pun;
        }else{
            pr->tail->pc->length = remainingClock;
        }
    }
}

struct TCB RunningReadyQueue(struct PointersReady *pr){

    struct TCB task;
    task.id = pr->head->id;
    task.pc = pr->head->pc;
    task.arrival_time = pr->head->arrival_time;
    task.instr_list = pr->head->instr_list;
    task.state = RUNNING;

    struct TCBreadyQueue * punt = pr->head;
    if(pr->head->pun == NULL){
        
        free(punt);
        pr->head = NULL;
        pr->tail = NULL;
    }else{
        pr->head = pr->head->pun;
        
        free(punt);
    }

    return task;
}

void BlockedToReady(struct PointersQueue * pq, struct PointersReady *pr, double clock, FILE *fp, int core){

    if(pq->head == NULL){ 
    
    }else{
    if(pq->head->blockedTimer == 0){
          
        if(pr->head == NULL){
            
            pr->head = (struct TCBreadyQueue *)malloc(sizeof(struct TCBreadyQueue));
            pr->head->id = pq->head->id;
            pr->head->pc = pq->head->pc->pun;
            pr->head->arrival_time = pq->head->arrival_time;
            pr->head->instr_list = pq->head->instr_list;
            pr->head->state = READY;
            pr->head->pun = NULL;
            pr->tail = pr->head;
            
            fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, pr->head->id, "ready");

       
            if(pq->head->pun == NULL){
            free(pq->head);
            pq->head = NULL;
            }else{
            
                struct TCBqueue *p = pq->head;
                pq->head = pq->head->pun;
                free(p);
            }

        }else{  
            pr->tail->pun = (struct TCBreadyQueue *)malloc(sizeof(struct TCBreadyQueue));
            pr->tail = pr->tail->pun;
            pr->tail->id = pq->head->id;
            pr->tail->pc = pq->head->pc->pun;
            pr->tail->arrival_time = pq->head->arrival_time;
            pr->tail->instr_list = pq->head->instr_list;
            pr->tail->state = READY;
            pr->tail->pun = NULL;
          
            fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, pr->tail->id, "ready");

            if(pq->head->pun == NULL){
                free(pq->head);
                pq->head = NULL;
            }else{
                struct TCBqueue *p = pq->head;
                pq->head = pq->head->pun;
                free(p);
            }
        } 
    }else{

    }

    }

}


char * EnumValue(enum StateModel sm){

    switch(sm){

        case 0:
        return "new";
        
        case 1:
        return "ready";
        
        case 2:
        return "running";

        case 3:
        return "blocked";

        case 4:
        return "exit";

        default:
        return "error";
        
    }
}



