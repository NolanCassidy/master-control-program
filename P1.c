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

void removeNewLine(char *input){
    for(;;){
        if(*input == '\0'){
            input--;
            if(*input == '\n'){
                *input = '\0';
            }
            break;
        }
        input++;
    }
}

int main(int argc, char *argv[]) {
		if(argc !=2){
			exit(0);
		}

		char* file = NULL;
  	file = argv[1];

		if(file==NULL){
			exit(0);
		}

		int i;
    int index;
    int processCount;
		int count;
  	int fd;
		fd = open(file, 0);

		char fileBuffer[BUFFER];
		char *args[BUFFER];
		char command[BUFFER];
		pid_t pid[BUFFER];

		int result;
		for(processCount=0; (result = p1getline(fd, fileBuffer, BUFFER)) != 0; processCount++){
        index = p1getword(fileBuffer,0, command);
        args[0] = p1strdup(command);

				for(count=1;(index = p1getword(fileBuffer, index, command)) != -1;count++){
            args[count] = p1strdup(command);
        }

        char* input = args[count-1];
        removeNewLine(input);
        args[count] = NULL;

        pid[processCount] = fork();
        if(pid[processCount] == 0){
            execvp(args[0], args);
        }

        for(i = 0; i < count; i++){
            free(args[i]);
        }
    }

    for(i = 0; i < processCount; i++){
        waitpid(pid[i], 0, 0);
    }
    return 0;
}
