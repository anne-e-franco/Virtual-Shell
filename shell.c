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
	char prompt[] = "Virtual Shell > ";
	//Basic logic: always present the user with a prompt, until they exit the program.
	printf("%s", prompt); //once at the beginning (before the loop is entered)
	while (strcmp(fgets(buffer, 120, stdin), "exit\n")!=0){
	//Detect pipes
		k = makeargv(buffer, " \n", &argpath);
		for (i=0;i<k;i++){
			//if (strcmp(argpath[i], "|\n")!=0){
			
			//}
			printf("Command [%i]: %s\n",i,argpath[i]);
		}
	
		processid = fork(); //fork the current process (2 identical processes exist now). Let's use them.
		
		switch(processid){
			case -1:
				err_sys("Failed to fork");
				break;
			case 0: //Child Process
		        execvp(argpath[0],argpath);write(1,"exec fail\n",10);exit(1);
				break;
			default: //Parent Process
				waitpid(processid, NULL, 0);
				printf("%s", prompt);
				break;
		}
	}
	printf("Bye!\n");
}