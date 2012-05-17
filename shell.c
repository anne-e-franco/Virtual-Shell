#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
    int fdin, fdout;
    char *fnin, *fnout;
    
	//Basic logic: always present the user with a prompt, until they exit the program.
	printf("%s", prompt); //once at the beginning (before the loop is entered)
	while (strcmp(fgets(buffer, 120, stdin), "exit\n")!=0){
        //printf("'%s'",buffer);
	//Detect pipes
		k = makeargv(buffer, " \n", &argpath);
		j = 1;
		queue[0] = 0;
        printf("%i vs %i\n",(int)(sizeof(argpath)/sizeof(char)),k);

		for (i=0;i<k;i++){
            //Find the pipes, replace them with null characters (to stop execvp).
			if (strcmp(argpath[i], "|")==0){
				argpath[i] = NULL;
				queue[j] = i+1;
				j++;
			}
           
            //Find redirection requests
            if (strcmp(argpath[i], ">")==0) {
                //output result to file
                //fetch filename from argpath
              //  printf ("%i", i);
                argpath[i] = NULL;
                fnout = calloc(1,sizeof(k-i));
                strcpy(fnout,argpath[i+1]);
                //printf("%s",fnout);
                //fdout = open(
            }
            if (strcmp(argpath[i], ">>")==0) {
                //append file with result
            }
            if (strcmp(argpath[i],"<")==0){
                //get info from file 
            }
            
            printf("argpath[%i]: %s\n",i,argpath[i]);
            
		}
	
		for(i=0;i<j;i++){
		printf("Command [%i]: %s\n",i, argpath[queue[i]]);
		}
		
		processid = fork(); //fork the current process (2 identical processes exist now). Let's use them.
//FORK	
		//Did we fail?
		if(processid < 0){
			err_sys("Failed to fork");
		}
//CHILD PROCESS - Shell's Child
		if(processid == 0){
			printf("Right Side\n");
            //if (j == 1) {
              //  execvp(argpath[j-1],&argpath[j-1]);
            //}
            else{
                j--;
                pipe(fdl);
                //Make a child.
                processid = fork();
                if(processid > 0){
                    //RM's World
                    //close one end of pipe
                    close(fdl[1]);
                    //dup2
                    fdl[0] = dup2(fdl[0],STDIN_FILENO);
                    if (((execvp(argpath[queue[j]],&argpath[queue[j]]))!=0)) write(1,"Right fail\n",12);
                    //execvp(argpath[queue[2]],argpath);
                    
                }
    //2nd CHILD
                if(processid == 0){
                    printf("Middle Children\n");
                    //connect ends of pipe
                    fdr[1] = fdl[1];
                    fdr[1] = dup2(fdr[1],STDOUT_FILENO);
                    fdr[0] = fdl[0];
                    close(fdr[0]);
                    
                    //Pipe!
                    pipe(fdl);
                    //Fork!
                    processid = fork();
                    
                    if(processid > 0){ 
                        while (j>2) {
                            //Use LM info, do ops on, pass to R:
                            j--;
                            //close one end of pipe
                            close(fdl[1]);
                            fdl[0] = dup2(fdl[0],STDIN_FILENO);
                            //write (1, "Ready to Exec in M\n",19);
                            if ((execvp(argpath[queue[j]], &argpath[queue[j]])!=0)) write(1,"Middle fail\n",13);
                        }                        
                    }
    //Youngest Child
                    if(processid == 0){
                        //printf("Left Side\n");
                        //fdr[0] = fdl[0];
                        fdr[1] = fdl[1]; close(fdr[0]);
                        fdr[1] = dup2(fdr[1],STDOUT_FILENO);
                        if ((execvp(argpath[queue[0]], &argpath[queue[0]])!=0)) write(1,"Left fail\n",10);
                    }//exit LM
                }//exit M
            }
        }
//PARENT PROCESS - Shell Level
		waitpid(processid, &status, 0);
		printf("%s",prompt);
	}
printf("Bye!\n");
}