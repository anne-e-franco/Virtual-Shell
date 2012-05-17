//
//  redirect.c
//  
//
//  Created by Anne Franco on 5/10/12.
//  Copyright (c) 2012 Western Michigan University. All rights reserved.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include "ourhdr.h"

extern int makeargv(char *, char *, char ***); //function definition found in makeargv.c

int main(){
	//std objects
	char *buffer = calloc(1, 120); //buffer to hold command line input
	pid_t processid;
	//pointer(s)
	char **argpath; //array of (input) commands returned by makeargv.c
	//workers
	int i,j,k, status;
	int queue[10], fdl[2], fdr[2] ;
	char prompt[] = "Virtual Shell > ";
    //REdirection 'flags'
    int fdin, fdout, redirout;
    char *fnin, *fnout;
    
	//Basic logic: always present the user with a prompt, until they exit the program.
	printf("%s", prompt); //once at the beginning (before the loop is entered)

    while (strcmp(fgets(buffer, 120, stdin), "exit\n")!=0){
        fnout = calloc(1,120);
        redirout = 0;
        k = makeargv(buffer, " \n", &argpath);
        j = 1;
		queue[0] = 0;
        for (i=0;i<k;i++){
            //Find the pipes, replace them with null characters (to stop execvp).
			if (strcmp(argpath[i], "|")==0){
				argpath[i] = NULL;
				queue[j] = i+1;
				j++;
			}
            //Find redirection requests
            if (strcmp(argpath[i], ">")==0 | strcmp(argpath[i],">>")==0) {
                //output result to file
                //fetch filename from argpath
                fnout = calloc(1,sizeof(k-i));
                strcpy(fnout,argpath[i+1]);
                if (strcmp(argpath[i],">")==0) {
                    redirout = 1;
                }
                else{
                    redirout = 2;
                }
                argpath[i] = NULL;
            }
           // if (strcmp(argpath[i],"<")==0) {
               // fnin = calloc(1,sizeof(k-i));
                //strcpy(fnin,argpath[i+1]);
                //redirout = 3;
               // argpath[i] = NULL;
            //}
        }
        processid = fork();
        if (processid == 0){
            if (redirout == 1 | redirout == 2) {
                printf("Redirecting output to file...\n Filename: '%s'\n",fnout);
                if (redirout == 1) {
                    printf("Mode: Create / Truncate\n");
                    fdout = open(fnout,O_RDWR | O_CREAT | O_TRUNC, 0777);
                }
                else{
                    printf("Mode: Create / Append\n");
                    fdout = open(fnout,O_RDWR | O_CREAT | O_APPEND, 0777);
                }
                fdout = dup2(fdout, STDOUT_FILENO);
            }
            
            if ((execvp(argpath[queue[0]], &argpath[queue[0]])!=0)) write(fdout,"Child fail\n",10);
        }
        else{
            waitpid(processid, &status, 0);
            close(fdout);
            printf("%s", prompt);
        }
    }
    printf("Bye!\n");
}