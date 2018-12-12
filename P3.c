#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include "p1fxns.h"
#include "p1fxns.c"
#include <fcntl.h>

#define BUFFER 1024

int USR1 = 0;
int ALRM = 0;

typedef struct process Process;
typedef struct process {
		Process *next;
		Process *prev;
	  pid_t pid;
    int start;
} Process;

typedef struct pcb {
		Process *head;
		Process *tail;
    int processCount;
} PCB;

Process *runNow;
int runningChild = 0;
void handleSignals(int sig) {
    switch (sig) {
				case SIGUSR1:
						USR1++;
						break;
        case SIGALRM:
            kill(runNow->pid, SIGSTOP);
            ALRM++;
            break;
        case SIGCHLD:
            if (waitpid(runNow->pid, NULL, WNOHANG) != 0) {
							  ALRM++;
                runningChild = 1;
            }
    }
}

void initProc(Process *p){
	p->pid = fork();
	p->start = 1;
	p->next = NULL;
	p->prev = NULL;
}

void initPCB(PCB *processes){
	processes->head = NULL;
	processes->tail = NULL;
	processes->processCount = 0;
}

void addProcess(Process *p, PCB *processes){
	if (processes->processCount == 0) {
			processes->head = p;
			processes->tail = p;
			processes->processCount++;
	} else {
			processes->tail->next = p;
			p->prev = processes->tail;
			processes->tail = p;
			processes->tail->next = NULL;
			processes->processCount++;
	}
}

void startRunning(Process *runNext){
	while(runNext != NULL) {
			if(runNext->next == NULL){
				free(runNext);
				runNext = NULL;
			}else {
				Process *temp = runNext->next;
				free(runNext);
				runNext = temp;
			}
	}
}

void removeNewLine(char *input) {
    for(;;) {
        if (*input == '\0') {
            input--;
            if (*input == '\n') {
                *input = '\0';
            }
            break;
        }
        input++;
    }
}

void workload(int fd, PCB *processes){
	char fileBuffer[BUFFER];
	char *args[BUFFER];
	char command[BUFFER];
	int count;

	int i;
	int index;
	int result;
	while ((result = p1getline(fd, fileBuffer, BUFFER)) != 0) {
			index = p1getword(fileBuffer, 0, command);
			args[0] = p1strdup(command);
			for(count=1;(index = p1getword(fileBuffer, index, command)) != -1;count++){
					args[count] = p1strdup(command);
			}

			char *input = args[count - 1];
			removeNewLine(input);
			args[count] = NULL;

			Process *p;
			p = (Process *) malloc(sizeof(Process));
			if (p== NULL){exit(0);}

			initProc(p);
			addProcess(p,processes);

			int pid;
			pid = processes->tail->pid;
			if ( pid == 0) {
					while (!USR1){
						usleep(1);
					}
					if (execvp(args[0], args) < 0) {
							for (i = 0; i < count; i++) {
									free(args[i]);
							}
							startRunning(processes->head);
							free(processes);
							exit(0);
					}
			}else if (pid < 0){
					p1perror(1, "Failed to fork process");
			}

			for (i = 0; i < count; i++) {
					free(args[i]);
			}
	}
}

void checkSignals(){
	if (signal(SIGALRM, handleSignals) == SIG_ERR) {
			p1perror(1, "Faild Alarm");
			exit(0);
	}
	if (signal(SIGUSR1, handleSignals) == SIG_ERR) {
			p1perror(1, "Failed USR1");
			exit(0);
	}
	if (signal(SIGCHLD, handleSignals) == SIG_ERR) {
			p1perror(1, "Failed Child");
			exit(0);
	}
}

void setTimer(struct itimerval it_val){
	it_val.it_value.tv_sec = 1;
	it_val.it_value.tv_usec = 1;
	it_val.it_interval = it_val.it_value;
	if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
			p1perror(1, "Failed to set timer");
			exit(0);
	}
}

void processChild(Process *runNext,PCB *processes){
	if (runNow == processes->head) {
			processes->head = runNow->next;
			if (processes->head != NULL)
					processes->head->prev = NULL;
	} else if (runNow == processes->tail) {
			processes->tail = runNow->prev;
			if (processes->tail != NULL)
					processes->tail->next = NULL;
	} else {
			runNow->prev->next = runNow->next;
			runNow->next->prev = runNow->prev;
	}
	processes->processCount--;
}

void runPCB(PCB *processes){
	Process *runNext;
	runNext = processes->head;
	while (processes->processCount > 0) {
			if (runNext == NULL) {
					runNext = processes->head;
			}

			runNow = runNext;
			if (runNow->start) {
					kill(runNow->pid, SIGUSR1);
					runNow->start = 0;
			} else {
					kill(runNow->pid, SIGCONT);
			}

			while (!ALRM){
				continue;
			}

			if(runningChild){
					processChild(runNext,processes);
					Process *temp = runNext;
					runNext = runNext->next;
					free(temp);
					runningChild = 0;
			} else if (processes->processCount != 0) {
					runNext = runNext->next;
			}
			ALRM = 0;
	}
}

int main(int argc, char *argv[]) {
		if(argc !=2){
			exit(0);
		}

		char* file = NULL;
		int fd;
		struct itimerval it_val;
		PCB *processes = malloc(sizeof(PCB));

		file = argv[1];
		if(file==NULL){
			exit(0);
		}
		fd = open(file, 0);

		checkSignals();
    setTimer(it_val);
    initPCB(processes);
		workload(fd,processes);
		runPCB(processes);
    free(processes);
   	return 0;
}
