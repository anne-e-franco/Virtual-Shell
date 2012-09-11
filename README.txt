/*****************************************
*		README.txt		 *
*					 *
*	VirtualShell version 1.0		 *
*	Author: Anne Franco		 *
*	Course: System Prog. Concepts	 *
*					 *
*****************************************/

--File Manifest--
	* shell.c
	* makeargv.c
	* error.c
	* ourhdr.h
	* README.txt
	* a.out

-- Overview --
The Virtual Shell is a miniature 'shell' program written in C in partial fulfillment of the course requirements for CS2240 - System Programming Concepts.

-- How to Run --
A working copy of the current shell should be included with your download as 'a.out'. 

If not, run the following from the command-line:
	
	gcc -combine shell.c makeargv.c error.c

Run the program by calling './a.out' from the command-line.

-- Features --
The Virtual Shell can handle: 
	* An arbitrary number of commands.
	* Redirection requests '<', '>' & '>>'
	* Background requests (using '<command> &')

-- Limitations --
The Virtual Shell cannot handle:
	* Command-line input longer than 120 characters
	* Redirection '<' other than at the first command
	* Redirection '>' or '>>' other than at the last command.

-- Basic Methodology --
The Virtual Shell reads in a string from the command line and passes this string to the makeargv function. makeargv() transforms the input string into an array of tokens (delimited by space). The token array is then scanned for pipes and redirection requests, and a list of valid commands is built in queue[]. 

Each instance of a pipe denotes a new command. The address of the next command is stored in the queue, and the pipe replaced with a NULL character in order to stop the execution of execvp. 

Each instance of a redirection request detects the type of redirection and creates a corresponding flag to be handled just before (last for output, first for input) command execution. The redirection request character is replaced with a NULL character in order to stop the execution of execvp.

Backgrounding requests are checked for last, and only checks the last character in the input string. A flag is set to be checked for in the parent process to determine whether wait() should be executed.

The topmost process is the shell. After the above queueing and flag creation, the shell creates a pipe, and forks. The parent process is responsible for waiting (if '&' is not used), closing files, and displaying the prompt.

The Eldest Child is aka Right-hand. It is responsible for creating as many children are necessary to handle all the commands in the queue. If the command is the first (and last), check for input redirection requests. Otherwise, always read from the pipe, check for output redirection requests and execute the last command in the queue.

The Middle Child is responsible for respawning as many times as necessary to handle all but the first and last commands. The Middle child utilizes both ends of the pipe in order to act as liaison between the Left and Right Sides. As a result, this child can respawn easily. 

	Note: the Middle child will not fork unless necessary (queue length is greater than 2).

The Youngest Child (aka Left Side) is responsible for handling the first command in the queue when queue length is greater than 1. Left side checks for input redirection requests, and writes to the pipe instead of stdout.

(EOF)
