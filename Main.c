#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/shm.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>
#include "TaskList.h"
#include "InstrList.h"
#include "Tcb.h"

pthread_mutex_t lock;

struct param{
    int x;
    struct PointersTask *taskList;
    struct PointersQueue *blockedQueue;
    int core;
    FILE *fp;
    struct PointersReady *readyQueue;
};

void *Preemptive(void* param){

    int check = 0;
    time_t t;
    srand((unsigned) time(&t));
    struct param * paramValues = (struct param *) param;
    double clock = 0;
    int quanto  = 0;
    int spentTime = 0;

    struct TCB TaskExec; 
    struct PointersTask *pt = paramValues->taskList;
    struct PointersQueue *blockedQueue = paramValues->blockedQueue;
    struct PointersReady *readyQueue = paramValues->readyQueue;

    int core = paramValues->core;

    FILE *fp;
    fp = paramValues->fp;


    while(pt->head != NULL || readyQueue->head != NULL || blockedQueue->head != NULL){

    if(pt->head != NULL && pt->head->arrivalTime <= clock){

        pthread_mutex_lock(&lock);   
            TaskExec =  NewTask(pt->head, TaskExec);
            RemoveHeadTask(pt);
            fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, TaskExec.id, "new");
            CountBlockedTask(blockedQueue);
            BlockedToReady(blockedQueue,readyQueue, clock, fp, core);
        pthread_mutex_unlock(&lock);
        clock++;
            
        
        
        
        TaskExec = ReadyTask(TaskExec);
        pthread_mutex_lock(&lock);
            fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, TaskExec.id, "ready");
            CountBlockedTask(blockedQueue);
            BlockedToReady(blockedQueue,readyQueue, clock, fp, core);
        pthread_mutex_unlock(&lock);
        clock++;


        TaskExec = RunningTask(TaskExec);
        pthread_mutex_lock(&lock);
            fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, TaskExec.id, "running");
            CountBlockedTask(blockedQueue);
            BlockedToReady(blockedQueue,readyQueue, clock, fp, core);
        pthread_mutex_unlock(&lock);
        clock++;

    }else if(readyQueue->head != NULL){
        pthread_mutex_lock(&lock);
            TaskExec = RunningReadyQueue(readyQueue);
            fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, TaskExec.id, "running");
            CountBlockedTask(blockedQueue); 
            BlockedToReady(blockedQueue,readyQueue, clock, fp, core); 
        pthread_mutex_unlock(&lock);
        clock++;
      

    }else{

        pthread_mutex_lock(&lock);
            CountBlockedTask(blockedQueue);
            BlockedToReady(blockedQueue,readyQueue, clock, fp, core);
        pthread_mutex_unlock(&lock);
        clock++;
     
        continue;
    }

    quanto = 0;
    check = 0;
    while(TaskExec.pc != NULL){
        if(TaskExec.pc->typeFlag == 0){
            spentTime = 0;
            for(int i=0; i<TaskExec.pc->length; i++){
              
                check = 0;
                spentTime++;
                quanto++;
                pthread_mutex_lock(&lock);
                    CountBlockedTask(blockedQueue);
                    BlockedToReady(blockedQueue,readyQueue, clock, fp, core);
                pthread_mutex_unlock(&lock);
                
                clock++;
    
                if((quanto == 10 && TaskExec.pc->length - spentTime != 0) ||
                   (quanto == 10 && TaskExec.pc->length - spentTime == 0 && TaskExec.pc->pun != NULL)){
                   
                    check = 1;
                    break;
                             
                }
            }
        }else{
            check = 2;
        }
            
        if(check == 0){
           
              

                TaskExec.pc = TaskExec.pc->pun;
                pthread_mutex_lock(&lock);
                    CountBlockedTask(blockedQueue);
                    BlockedToReady(blockedQueue,readyQueue, clock, fp, core);
                pthread_mutex_unlock(&lock);
                clock++;

                continue;

        }else if(check == 1){

                pthread_mutex_lock(&lock); 
                    ReadyQueue(TaskExec, readyQueue, spentTime); 
                    fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, TaskExec.id, "ready");
                    CountBlockedTask(blockedQueue);
                    BlockedToReady(blockedQueue,readyQueue, clock, fp, core);
                pthread_mutex_unlock(&lock);
                clock++;

                break;

        }else if(check == 2){
                
                int blockedTimer = (rand() % TaskExec.pc->length)+1;
                pthread_mutex_lock(&lock); 
                    BlockedTask(TaskExec, blockedQueue, blockedTimer);
                    fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, TaskExec.id, "blocked");
                    CountBlockedTask(blockedQueue);
                    BlockedToReady(blockedQueue,readyQueue, clock, fp, core);
                pthread_mutex_unlock(&lock);
                clock++;
      
                break;      
        }
    }

    if(check == 1 || check == 2){
        continue;
    } 

        TaskExec = ExitTask(TaskExec);
        pthread_mutex_lock(&lock);
            fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, TaskExec.id, "exit");
            CountBlockedTask(blockedQueue);
            BlockedToReady(blockedQueue,readyQueue, clock, fp, core);
        pthread_mutex_unlock(&lock);
        clock++;
 
        fflush(fp);
        
    }
    return NULL;
}



void* NotPreemptive(void* param){

    time_t t;
    srand((unsigned) time(&t));
    struct param * paramValues = (struct param *) param;
    double clock = 0;
    struct TCB TaskExec;
    int check = 0;
    struct PointersTask *pt = paramValues->taskList;
    struct PointersQueue *blockedQueue = paramValues->blockedQueue;
    int core = paramValues->core;

    FILE *fp;
    fp = paramValues->fp;

    while(pt->head != NULL || blockedQueue->head !=NULL){

       if(blockedQueue->head != NULL && blockedQueue->head->blockedTimer == 0){

            pthread_mutex_lock(&lock); 
                TaskExec = ResumeTask(blockedQueue); 
                fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, TaskExec.id, "ready");
                CountBlockedTask(blockedQueue);
            pthread_mutex_unlock(&lock);    
            clock++;

            TaskExec = RunningTask(TaskExec);
            pthread_mutex_lock(&lock);
                fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, TaskExec.id, "running");
                CountBlockedTask(blockedQueue);
            pthread_mutex_unlock(&lock);
            clock++;

        }else if(pt->head != NULL && pt->head->arrivalTime <= clock){

            pthread_mutex_lock(&lock);   
                TaskExec =  NewTask(pt->head, TaskExec);
                RemoveHeadTask(pt);
                fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, TaskExec.id, "new");
                CountBlockedTask(blockedQueue);
            pthread_mutex_unlock(&lock);
            clock++;
            
        
            TaskExec = ReadyTask(TaskExec);
            pthread_mutex_lock(&lock);
                fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, TaskExec.id, "ready");
                CountBlockedTask(blockedQueue);
            pthread_mutex_unlock(&lock);
            clock++;


            TaskExec = RunningTask(TaskExec);
            pthread_mutex_lock(&lock);
                fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, TaskExec.id, "running");
                CountBlockedTask(blockedQueue);
            pthread_mutex_unlock(&lock);
            clock++;

        }else{
            pthread_mutex_lock(&lock);
                CountBlockedTask(blockedQueue);
            pthread_mutex_unlock(&lock);
            clock++;
     
        continue;

        }

        check = 0;
        while(TaskExec.pc != NULL){

            if(TaskExec.pc->typeFlag == 0){
                for(int i=0; i<TaskExec.pc->length; i++){
                    check = 0;
                    pthread_mutex_lock(&lock);
                        CountBlockedTask(blockedQueue);
                    pthread_mutex_unlock(&lock);
                    clock++;
           
                }
            }else{
                check = 1;
            }


            if(check == 0){
                TaskExec.pc = TaskExec.pc->pun;
                pthread_mutex_lock(&lock);
                    CountBlockedTask(blockedQueue);
                pthread_mutex_unlock(&lock);
                clock++;

                continue;

            }else if(check == 1){
                int blockedTimer = (rand() % TaskExec.pc->length)+1;
                pthread_mutex_lock(&lock); 
                    BlockedTask(TaskExec, blockedQueue, blockedTimer);
                    fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, TaskExec.id, "blocked");
                    CountBlockedTask(blockedQueue);
                pthread_mutex_unlock(&lock);
                clock++;

                break;
            }

        }

        if(check == 1){
            continue;
        }

        TaskExec = ExitTask(TaskExec);
        pthread_mutex_lock(&lock);
            fprintf(fp,"core%d,%.0lf,%d,%s\n", core, clock, TaskExec.id, "exit");
            CountBlockedTask(blockedQueue);
        pthread_mutex_unlock(&lock);
        clock++;

        fflush(fp);   
    }
    return NULL;
}




void errorMsg(){
    printf("\n");
    printf("Guida all'utilizzo del programma.\n\n\n");
    printf("Dopo ./nomeprogramma inserire solo una volta i seguenti parametri: \n\n");
    printf("-op oppure --output-preemption seguito dal nome del file in cui si vorrebbe scrivere l'output per lo scheduler con preemption\n\n");
    printf("-on oppure --output-no-preemption seguito dal nome del file in cui si vorrebbe scrivere l'output per lo scheduler senza preemption\n\n");
    printf("-i oppure --input seguito dal nome del file di input contenente la lista di task\n\n");
    printf("-h oppure --help stampa l'helper di come usare il programma.\n");
    printf("Unica eccezione va fatta per -h o --help, che non e' necessario scriverlo per forza per l'esecuzione del programma. \n\n");
    printf("Esempio:\n");
    printf("./simulatore -op filedestinazionepreemptive.csv -on filedestinazionenopreemptive.csv -i 01_tasks.csv\n\n");    
}

int main(int argc, char* argv[]){
    
    if(argv[1]==NULL){
        return 0;
    }

    int i=1;
    int checkOp = 0, checkOn = 0, checkI = 0;
    int preemptiveFile;
    int nopreemptiveFile;
    int inputFile;
    
    while(argv[i] != NULL){

            if(!strcmp(argv[i],"-op") || !strcmp(argv[i],"--output-preemption")){
            checkOp++;
            i++;
            preemptiveFile = i;
                if(argv[i]==NULL){
                errorMsg();
                return 0;
                }
            }
            else if(!strcmp(argv[i],"-on") || !strcmp(argv[i],"--output-no-preemption")){
            checkOn++;
            i++;
            nopreemptiveFile = i;
                if(argv[i]==NULL){
                    errorMsg();
                    return 0;
                }
            }
            else if(!strcmp(argv[i],"-i") || !strcmp(argv[i],"--input")){
            checkI++;
            i++;
            inputFile = i;
                if(argv[i]==NULL){
                    errorMsg();
                    return 0;
                }
            }
            else if(!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help")){
            errorMsg();
            return 0;
            }
            else{
            errorMsg();
            return 0;
            }
        
        i++;
    }

    if(checkOn!= 1 || checkOp != 1 || checkI != 1){
        errorMsg();
        return 0;
    }

    pthread_mutex_init(&lock, NULL);
    
    pid_t pid1, pid2;

    int pipeOn[2];
    int pipeOp[2];

    pipe(pipeOp);
    
    pid1 = fork();
     
    if(pid1 == 0){

        close(pipeOp[1]);
        FILE *stream = fdopen(pipeOp[0], "r");
    
        struct PointersTask *pt = StartTask();
        int valueOne, valueTwo;
        char ch;

    while(fscanf(stream, "%c", &ch)!= EOF){

        if(ch == 't'){

        fscanf(stream, "%c", &ch);
        fscanf(stream, "%d", &valueOne);
        fscanf(stream, "%c", &ch);
        fscanf(stream, "%d", &valueTwo);
        AddTask(pt, valueOne, valueTwo);
        }
      
        if(ch == 'i'){

        fscanf(stream, "%c", &ch);
        fscanf(stream, "%d", &valueOne);
        fscanf(stream, "%c", &ch);
        fscanf(stream, "%d", &valueTwo);
        AddInstrList(pt, valueOne, valueTwo);
        }
    }

        FILE *Pree;
        Pree = fopen(argv[preemptiveFile], "a");

        struct param coreOne, coreTwo;

        struct PointersQueue * blockedQueue =  (struct PointersQueue*)malloc(sizeof(struct PointersQueue));
        blockedQueue->head = NULL;
        blockedQueue->tail = NULL;

        struct PointersReady * readyQueue = (struct PointersReady*)malloc(sizeof(struct PointersReady));
        readyQueue->head = NULL;
        readyQueue->tail = NULL;

        coreOne.taskList = pt;
        coreOne.blockedQueue = blockedQueue;
        coreOne.core = 1;
        coreOne.readyQueue = readyQueue;
        coreOne.fp = Pree;

        coreTwo.taskList = pt;
        coreTwo.blockedQueue = blockedQueue;
        coreTwo.core = 2;
        coreTwo.readyQueue = readyQueue;
        coreTwo.fp = Pree;

        pthread_t firstThread, secondThread;

        pthread_create(&firstThread,NULL,&Preemptive,&coreOne);
        pthread_create(&secondThread,NULL,&Preemptive,&coreTwo);
        
        pthread_join(firstThread,NULL);
        pthread_join(secondThread,NULL);

        fclose(Pree);
        free(blockedQueue);
        free(readyQueue);

        while(pt->head != NULL){
            RemoveHeadTask(pt);
        }

        free(pt);
        exit(0);
    
    }else{

        pipe(pipeOn);
        pid2 = fork();

        if(pid2 == 0){

            close(pipeOn[1]);
            close(pipeOp[1]);
            close(pipeOp[0]);

            FILE *stream = fdopen(pipeOn[0], "r");

            struct PointersTask *pt = StartTask();
            int valueOne, valueTwo;
            char ch;

            while(fscanf(stream, "%c", &ch)!= EOF){

            if(ch == 't'){

                fscanf(stream, "%c", &ch);
                fscanf(stream, "%d", &valueOne);
                fscanf(stream, "%c", &ch);
                fscanf(stream, "%d", &valueTwo);
                AddTask(pt, valueOne, valueTwo);
            }
      
            if(ch == 'i'){

                fscanf(stream, "%c", &ch);
                fscanf(stream, "%d", &valueOne);
                fscanf(stream, "%c", &ch);
                fscanf(stream, "%d", &valueTwo);
                AddInstrList(pt, valueOne, valueTwo);
            }
        }

        FILE *NoPree;
        NoPree = fopen(argv[nopreemptiveFile],"a");
        
        struct PointersQueue * blockedQueue =  (struct PointersQueue*)malloc(sizeof(struct PointersQueue));
        blockedQueue->head = NULL;
        blockedQueue->tail = NULL;
        
        struct param coreOne, coreTwo;

        coreOne.taskList = pt;
        coreOne.blockedQueue = blockedQueue;
        coreOne.core = 1;
        coreOne.fp = NoPree;
        
        coreTwo.taskList = pt;
        coreTwo.blockedQueue = blockedQueue;
        coreTwo.core = 2;
        coreTwo.fp = NoPree;

        pthread_t firstThread, secondThread;
        pthread_create(&firstThread,NULL,&NotPreemptive,&coreOne);
        pthread_create(&secondThread,NULL,&NotPreemptive,&coreTwo);
  
        pthread_join(firstThread,NULL);
        pthread_join(secondThread,NULL);

        fclose(NoPree);
        free(blockedQueue);

        while(pt->head != NULL){
            RemoveHeadTask(pt);
        }

        free(pt);
        exit(0);
        }
    } 



    close(pipeOp[0]);
    close(pipeOn[0]);

    FILE *streamOp = fdopen(pipeOp[1], "w");
    FILE *streamOn = fdopen(pipeOn[1], "w");
    FILE *file = fopen(argv[inputFile],"r");
    if(file == NULL){
        errorMsg();
        return 0;
    }
    char x;
    while(fscanf(file, "%c", &x) != EOF){
        fprintf(streamOp,"%c",x);
        fprintf(streamOn,"%c",x);
    }

    fclose(streamOp);    
    fclose(streamOn); 

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
    


    


 
}