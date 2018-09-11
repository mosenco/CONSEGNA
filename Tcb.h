
enum StateModel{
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    EXIT
};

struct TCB{
    int id;
    struct InstrList *pc;
    int arrival_time;
    struct PointersInstr *instr_list; 
    enum StateModel state; 
};

struct TCBreadyQueue{
    int id;
    struct InstrList *pc;
    int arrival_time;
    struct PointersInstr *instr_list; 
    enum StateModel state; 
    struct TCBreadyQueue *pun;

};

struct PointersReady{
    struct TCBreadyQueue *head;
    struct TCBreadyQueue *tail;
};

struct TCBqueue{
    int id;
    struct InstrList *pc;
    int arrival_time;
    struct PointersInstr *instr_list;
    enum StateModel state;
    struct TCBqueue *pun;

    int blockedTimer;
};

struct PointersQueue{
    struct TCBqueue *head;
    struct TCBqueue *tail;
};


struct TCB NewTask(struct TaskList *p, struct TCB TaskExec);
struct TCB ReadyTask(struct TCB TaskExec);
struct TCB RunningTask(struct TCB TaskExec);
void BlockedTask(struct TCB TaskExec, struct PointersQueue * queue, int blockedTimer);
void CountBlockedTask(struct PointersQueue * pq);
struct TCB ResumeTask(struct PointersQueue * pq);
struct TCB ExitTask(struct TCB TaskExec);
void ReadyQueue(struct TCB TaskExec, struct PointersReady * pr, int usedClock);
struct TCB RunningReadyQueue(struct PointersReady *pr);
void BlockedToReady(struct PointersQueue * pq, struct PointersReady *pr, double clock, FILE *fp, int core);
char * EnumValue(enum StateModel sm);

