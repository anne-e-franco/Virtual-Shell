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
    int fdin, fdout, redirout, redirin, background;
    char *fnin, *fnout;
    
	//Basic logic: always present the user with a prompt, until they exit the program.
	printf("%s", prompt); //once at the beginning (before the loop is entered)
    
    while (strcmp(fgets(buffer, 120, stdin), "exit\n")!=0){
        fnout = calloc(1,120);
        redirout = 0;
        k = makeargv(buffer, " \n", &argpath); //makeargv returns the number of tokens.
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
            else if (strcmp(argpath[i], ">")==0 | strcmp(argpath[i],">>")==0) {
                //Request: output result to file
                //fetch filename from argpath
                fnout = calloc(1,sizeof(k-i));
                strcpy(fnout,argpath[i+1]);
                //set flag(s)
                if (strcmp(argpath[i],">")==0) {
                    redirout = 1;
                }
                else{
                    redirout = 2;
                }
                argpath[i] = NULL;
            }
            else if (strcmp(argpath[i], "<") == 0) {
                //Request: read input from file
                fnin = calloc(1,sizeof(k-i));
                argpath[i] = NULL;
                strcpy(fnin,argpath[queue[j-1]]);
                redirin = 1; //set flag(s)
                queue[j-1] = i+1; //correct queue entry (replace filename with command)
            }
        }
        if (strcmp(argpath[k-1], "&") == 0) {
            background = 1;
            argpath[k-1] = NULL;
        }
        else{
            background = 0;
        }
		
		processid = fork(); //fork the current process (2 identical processes exist now). Let's use them.
//FORK	
		//Did we fail?
		if(processid < 0){
			err_sys("Failed to fork");
		}
//CHILD PROCESS - Shell's Child AKA Right Side
		if(processid == 0){
            i = j-1; //turn i into index.
                pipe(fdl);
                //Make a child.
                processid = fork();
                if(processid > 0){
                    close(fdl[1]);  //close one end of pipe
                    //dup2: get input from standard in.
                    fdl[0] = dup2(fdl[0],STDIN_FILENO);
                    if (redirin == 1 & i == 0) {
                        printf("Reading input from file...\n Filename: '%s'\n", fnin);
                        fdin = open(fnin, O_RDONLY, 0777);
                        fdin = dup2(fdin, STDIN_FILENO);
                    }

                    if (redirout == 1 | redirout ==2) {
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
                    if (((execvp(argpath[queue[i]],&argpath[queue[i]]))!=0)) write(1,"Right fail\n",12);                    
                }
    //2nd CHILD - AKA Middle
                if(processid == 0){
                    i--; 
                    fdr[1] = fdl[1]; //connect to right-hand pipe.
                    fdr[1] = dup2(fdr[1],STDOUT_FILENO); //Intercept stdout, reroute into pipe
                    fdr[0] = fdl[0]; //connect to right-hand pipe (for symmetry)
                    close(fdr[0]); //not reading from pipe.
                    
                    while (i > 0) {
                        //Pipe!
                        pipe(fdl);
                        //Fork!
                        processid = fork(); //hatch youngest child.
                        
                        if(processid > 0){ 
                            //close one end of pipe
                            close(fdl[1]); //no writing
                            fdl[0] = dup2(fdl[0],STDIN_FILENO); //read from sdtin
                            //write (1, "Ready to Exec in M\n",19);
                            if ((execvp(argpath[queue[i]], &argpath[queue[i]])!=0)) write(1,"Middle fail\n",13);
                        } 
                        i--;
                    }
                      
    //Youngest Child - AKA Left Side
                    if(processid == 0){
                        fdr[0] = fdl[0];
                        fdr[1] = fdl[1]; close(fdr[0]);
                        fdr[1] = dup2(fdr[1],STDOUT_FILENO);
                        if (redirin == 1) {
                            printf("Reading input from file...\n Filename: '%s'\n", fnin);
                            fdin = open(fnin, O_RDONLY, 0777);
                            fdin = dup2(fdin, STDIN_FILENO);
                        }
                        if ((execvp(argpath[queue[i]], &argpath[queue[i]])!=0)) write(1,"Left fail\n",10);
                    }//exit LM
                }//exit M
        }//exit RM
//PARENT PROCESS - Shell Level
		if (background == 0) {
            waitpid(processid, &status, 0);
        }
        close(fdin);
        close(fdout);
		printf("%s",prompt);
	}
printf("Bye!\n");
}