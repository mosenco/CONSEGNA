struct TaskList{

    int i;
    int id;
    int arrivalTime;
    struct PointersInstr *list;
    struct TaskList *pun;
};

struct PointersTask{
    struct TaskList *head;
    struct TaskList *tail;
};



struct PointersTask *StartTask();
void AddTask(struct PointersTask *po, int id, int arrivalTime);
void AddInstrList(struct PointersTask *po, int x, int y);
void RemoveHeadTask(struct PointersTask *p);
