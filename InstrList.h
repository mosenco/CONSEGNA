
struct InstrList{

    int i;
    int typeFlag;
    int length;
    struct InstrList *pun;
};

struct PointersInstr{
    struct InstrList *head;
    struct InstrList *tail;
};


struct PointersInstr *StartInstr();
void AddInstr(struct PointersInstr *po, int typeFlag, int length);
int GetTypeFlag(struct InstrList *p, int x);
int GetLength(struct InstrList *p, int x);
void RemoveHeadInstr(struct PointersInstr *p);
_Bool CheckValidInstr(struct InstrList *p, int x);

