/* To Do: Scan input tokens for '>' and '|' characters first. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "ourhdr.h"

extern int makeargv(char *, char *, char ***); //function definition found in makeargv.c

int main(){
	//std objects
	char buffer[120]; //buffer to hold command line input
	pid_t processid;
	//pointer(s)
	char **argpath; //array of (input) commands returned by makeargv.c
	//workers
	int i,j,k;
	int queue[10];
	char prompt[] = "Virtual Shell > ";
	//Basic logic: always present the user with a prompt, until they exit the program.
	printf("%s", prompt); //once at the beginning (before the loop is entered)
	while (strcmp(fgets(buffer, 120, stdin), "exit\n")!=0){
	//Detect pipes
		k = makeargv(buffer, " \n", &argpath);
		//Find the pipes, replace them with null characters (to stop execvp).
		j = 1;
		queue[0] = 0;
		for (i=0;i<k;i++){
			if (strcmp(argpath[i], "|")==0){
				argpath[i] = "\0";
				queue[j] = i+1;
				j++;
			}
		}
	
	for(i=0;i<j;i++){
	printf("Command [%i]: %s\n",i, argpath[queue[i]]);
	}
		//processid = fork(); //fork the current process (2 identical processes exist now). Let's use them.

/*
		switch(processid){
			case -1:
				err_sys("Failed to fork");
				break;
			case 0: //Child Process
			printf("Command [%i]: %s\n",i, argpath[queue[i]]);
		        execvp(argpath[queue[i]],argpath);write(1,"exec fail\n",10);exit(1);
				break;
			default: //Parent Process
				waitpid(processid, NULL, 0);
				printf("%s", prompt);
				break;
		}*/
	}
	printf("Bye!\n");
}