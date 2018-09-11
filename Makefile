all: Main.c TaskList.c InstrList.c Tcb.c
	gcc Main.c TaskList.c InstrList.c Tcb.c -o simulator -pthread


clean:
	$(RM) simulator



