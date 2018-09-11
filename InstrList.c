#include <stdio.h>
#include <stdlib.h>
#include "InstrList.h"
#include <stdbool.h>


struct PointersInstr *StartInstr(){

    struct InstrList *p;
    struct PointersInstr *po;
    p = (struct InstrList *)malloc(sizeof(struct InstrList));
    p->i=0;
    
    po = (struct PointersInstr *)malloc(sizeof(struct PointersInstr));
    po->head = p;
    po->tail = p;

    return po;
}



void AddInstr(struct PointersInstr *po, int typeFlag, int length){

    if(po->head->i == 0){
        po->head->typeFlag = typeFlag;
        po->head->length = length;
        po->head->i = 1;
        po->head->pun = NULL;
    }else{
        struct InstrList *punt;
        punt = po->tail;
        punt->pun = (struct InstrList*)malloc(sizeof(struct InstrList));
        punt = punt->pun;
        punt->typeFlag = typeFlag;
        punt->length = length;
        punt->pun = NULL;
        po->tail = punt;
    }
}


int GetTypeFlag(struct InstrList *p, int x){

    for(int i = 0; i<x;i++)
    {
        p = p->pun; 
    }

  
    return p->typeFlag;
}


int GetLength(struct InstrList *p, int x){

    for(int i = 0; i<x;i++)
    {
        p = p->pun; 
    }

   

    return p->length;
}




void RemoveHeadInstr(struct PointersInstr *p){
    
    struct InstrList *p1;
    struct InstrList *p2;
    p1 = p->head;
    p2 = p->head;

    if(p2->pun!=NULL){ 
        p->head = p->head->pun;
        p1->pun = NULL;
        free(p1);
    }else{
        p->head = NULL;
        free(p1);
    }
}

bool CheckValidInstr(struct InstrList *p, int x){

    for(int i = 0; i<x;i++){
            p = p->pun;
        }

    if(p == NULL){
        return false;
    }else{
        return true;
    }

}