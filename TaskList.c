#include <stdio.h>
#include <stdlib.h>
#include "TaskList.h"
#include "InstrList.h"
#include <stdbool.h>



struct PointersTask *StartTask(){

    struct TaskList *p;
    struct PointersTask *po;

    p = (struct TaskList *)malloc(sizeof(struct TaskList));
    p->i=0;

    po = (struct PointersTask *)malloc(sizeof(struct PointersTask));
    po->head = p;
    po->tail = p;

    return po;
}



void AddTask(struct PointersTask *po, int id, int arrivalTime){

    if(po->head->i == 0){
        
        po->head->id = id;
        po->head->arrivalTime = arrivalTime;
        po->head->list = StartInstr();
        po->head->i = 1;
        po->head->pun = NULL;
        
    }else{
     
        struct TaskList *punt;
        punt = po->tail;
        punt->pun = (struct TaskList*)malloc(sizeof(struct TaskList));
        punt = punt->pun;
        punt->id = id;
        punt->arrivalTime = arrivalTime;
        punt->list = StartInstr();
        punt->pun = NULL;

        po->tail = punt;
    }
}



void AddInstrList(struct PointersTask *po, int x, int y){

    if(po->head->i == 0){
        AddInstr(po->head->list, x, y);
    }else{
        AddInstr(po->tail->list, x, y);
    }
}



void RemoveHeadTask(struct PointersTask *p){
    
    struct TaskList *p1, *p2;
    p1 = p->head;
    p2 = p->head;
  
    while(p->head->list->head!=NULL){
        RemoveHeadInstr(p->head->list);
    }

    if(p1->pun!=NULL){
        p1=p1->pun;
        free(p2);
        p->head = p1;
    }else{
        free(p->head);
        p->head=NULL;
    }
}

