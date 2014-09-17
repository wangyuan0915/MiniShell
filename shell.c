#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <mcheck.h>

#include "parser.h"
#include "shell.h"

/**
 * Program that simulates a simple shell.
 * The shell covers basic commands, including builtin commands (cd and exit only), 
 * standard I/O redirection and piping (|). 
 * 
 * You should not have to worry about more complex operators such as "&&", ";", etc.
 * Your program does not have to address environment variable substitution (e.g., $HOME), 
 * double quotes, single quotes, or back quotes.
 */

#define MAX_DIRNAME 100
#define MAX_COMMAND 1024
#define MAX_TOKEN 128

/* Functions to implement, see below after main */
int execute_cd(char** words);
int execute_nonbuiltin(simple_command *s);
int execute_simple_command(simple_command *cmd);
int execute_complex_command(command *cmd);


int main(int argc, char** argv) {
	
	char cwd[MAX_DIRNAME];           /* Current working directory */
	char command_line[MAX_COMMAND];  /* The command */
	char *tokens[MAX_TOKEN];         /* Command tokens (program name, parameters, pipe, etc.) */

	while (1) {

		/* Display prompt */		
		getcwd(cwd, MAX_DIRNAME-1);
		printf("%s> ", cwd);
		
		/* Read the command line */
		gets(command_line);
		
		/* Parse the command into tokens */
		parse_line(command_line, tokens);

		/* Empty command */
		if (!(*tokens))
			continue;
		
		/* Exit */
		if (strcmp(tokens[0], "exit") == 0)
			exit(0);
				
		/* Construct chain of commands, if multiple commands */
		command *cmd = construct_command(tokens);
		//print_command(cmd, 0);
    
		int exitcode = 0;
		if (cmd->scmd) {
			exitcode = execute_simple_command(cmd->scmd);
			if (exitcode == -1)
				break;
		}
		else {
			exitcode = execute_complex_command(cmd);
			if (exitcode == -1)
				break;
		}
		release_command(cmd);
	}
    
	return 0;
}


/**
 * Changes directory to a path specified in the words argument;
 * For example: words[0] = "cd"
 *              words[1] = "csc209/assignment2/"
 * Your command should handle both relative paths to the current 
 * working directory, and absolute paths relative to root,
 * e.g., relative path:  cd csc209/assignment2/
 *       absolute path:  cd /u/bogdan/csc209/assignment2/
 * You don't have to handle paths containing "~" or environment 
 * variables like $HOME, etc.. 
 */
int execute_cd(char** words) {
	
	/** 
	 * TODO: 
	 * The first word contains the "cd" string, the second one contains the path.
	 * Check possible errors:
	 * - The words pointer could be NULL, the first string or the second string
	 *   could be NULL, or the first string is not a cd command
	 * - If so, return an EXIT_FAILURE status to indicate something is wrong.
	 */
	 
	 //if the first token is not cd, return the 
	 if (words == NULL || strcmp(words[0], "cd") != 0) 
	 {
	 	return -1;
	 }

	 //only cd
	 //cd to home
	 if (words[1] == NULL)
	 {
	 	chdir(getenv("HOME"));
	 	return 0;
	 } 

	 
	 //chdir can work both relative or absolute path
	 else
	 {
	 	if (chdir(words[1]) == -1)
	 	{
	 		perror(words[0]);
	 	}
	 	return 0;
	 }

	/**
	 * TODO: 
	 * The safest way would be to first determine if the path is relative 
	 * or absolute (see is_relative function provided).
	 * - If it's not relative, then simply change the directory to the path 
	 * specified in the second word in the array.
	 * - If it's relative, then make sure to get the current working directory, 
	 * append the path in the second word to the current working directory and 
	 * change the directory to this path.
	 * Hints: see chdir and getcwd man pages.
	 * Return the success/error code obtained when changing the directory.
	 */

	 


	//judge relative or absolute


	 //not relative
// 	 if (!is_relative(words[1]))
// 	 {
// 		if(chdir(words[1]))
// 		{
// 			perror("chdir");
// 			return -1;
// 		}
// 	 }
	 
	 
// 	 //relative
// 	 else
// 	 {
//		if (chdir(words[1]))
//		{
//			perror("chdir");
//			return -1;
//		}

// 	 }	 
// 	 return 0;	 
 }


/**
 * Executes a regular command, based on the tokens provided as 
 * an argument.
 * For example, "ls -l" is represented in the tokens array by 
 * 2 strings "ls" and "-l", followed by a NULL token.
 * The command "ls -l | wc -l" will contain 5 tokens, 
 * followed by a NULL token. 
 */
int execute_command(char **tokens) {
	
	/**
	 * TODO: execute a regular command, based on the tokens provided.
	 * The first token is the command name, the rest are the arguments 
	 * for the command. 
	 * Hint: see execlp/execvp man pages.
	 * 
	 * - In case of error, make sure to use "perror" to indicate the name
	 *   of the command that failed.
	 *   You do NOT have to print an identical error message to what would happen in bash.
	 *   If you use perror, an output like: 
	 *      my_silly_command: No such file of directory 
	 *   would suffice.
	 * - Function returns the result of the execution.
	 */
	if (execvp(tokens[0], tokens) == -1)
    {
       	perror(tokens[0]);
    	//perror(": No such file or directory");
    	return -1;
    }

    return 0;
}


/**
 * Executes a non-builtin command.
 */
int execute_nonbuiltin(simple_command *s)
{
	/**
	 * TODO: Check if the in, out, and err fields are set (not NULL),
	 * and, IN EACH CASE:
	 * - Open a new file descriptor (make sure you have the correct flags,
	 *   and permissions);
	 * - Set the file descriptor flags to 1 using fcntl - to avoid leaving 
	 *   the file descriptor open across an execve. Don't worry about this
	 *   too much, just use this: 
	 *         fcntl(myfd, F_SETFD, 1);
	 *   where myfd is the file descriptor you just opened.
	 * - redirect stdin/stdout/stderr to the corresponding file.
	 *   (hint: see dup2 man pages).
	 */

	int filedes;

	//in set
	if (s->in != NULL) 
	{
		filedes = open(s->in, O_RDONLY, 0700);
		fcntl(filedes, F_SETFD, 1);
		
		//connect file descriptor to stdin
		if (dup2(filedes, fileno(stdin)) == -1)
		{
			perror("dup2");
			//exit(3);
			return -1;
		}

	}

	//out set
	if (s->out != NULL)
	{
		filedes = open(s->out, O_CREAT | O_TRUNC | O_RDWR, 0700);
		fcntl(filedes, F_SETFD, 1);
		
		//connect file descriptor to stdout
		if (dup2(filedes,  fileno(stdout)) == -1)
		{
			perror("dup2");
			//exit(3);
			return -1;
		}
		
	}

	
	//error set
	if (s->err != NULL)
	{
		
		//if out are set, append after stdout
		if (s->out != NULL) 
		{
			filedes = open(s->err, O_APPEND | O_RDWR, 0700);
		}
		
		//if out are not set
		else
		{
			filedes = open(s->out, O_CREAT | O_TRUNC | O_RDWR, 0700);
		}
		
		
		
		fcntl(filedes, F_SETFD, 1);
		if (dup2(filedes,  fileno(stderr)) == -1)
		{
			perror("dup2");
			//exit(3);
			return -1;
		}

	}


	/**
	 * TODO: Finally, execute the command using the tokens 
	 * (see execute_command function provided)
	 */
	if(execute_command(s->tokens) == -1)
	{
		//exit(1);
		return -1;
	}
	
	

	/**
	 * TODO: Close all filedescriptors needed and return status generated 
	 * by the command execution.
	 */

	if((close(filedes)) == -1) 
	{
        perror("close");
        //exit(1);
        return -1;
    }
	 

	//exit(0);
	return 0; 
	
}





/**
 * Executes a simple command (no pipes).
 */
int execute_simple_command(simple_command *cmd) {
	pid_t pid;
	int status;
	int result = 0;

	/**
	 * TODO: 
	 * Check if the command is builtin.
	 * 1. If it is, then handle BUILTIN_CD (see execute_cd function provided) 
	 *    and BUILTIN_EXIT (simply return an appropriate exit status).
	 * 2. If it isn't, then you must execute the non-builtin command. 
	 * - Fork a process to execute the nonbuiltin command 
	 *   (see execute_nonbuiltin function provided).
	 * - The parent should wait for the child.
	 *   (see wait man pages).
	 */
	 if (cmd->builtin == 1)
	 {
	 	result = execute_cd(cmd->tokens);
	 	return result;
	 }

	 

	 else if (cmd->builtin == 2)
	 {
	 	return -1;
	 }

	 

	 //no-builtin
	 else
	 {
	 	
	 	/*parent*/
	 	if ((pid = fork()) > 0)
	 	{
	 		
	 		wait(&status);
	 		// //run successfully
	 		// if (WEXITSTATUS(status) == 0)
	 		// {
	 		// 	return 0;
	 		// }

	 		// //something problem
	 		// else
	 		// {
	 		// 	return -1;
	 		// }

	 	}

	 	/*child*/
	 	else if (pid == 0)
	 	{
	 		result = execute_nonbuiltin(cmd);
	 		return result;
	 	}

	 	

	 	else
	 	{
	 		perror("fork");
	 		return -1;
	 	}
	 }


	return 0;
}


/**
 * Executes a complex command - it can have pipes (and optionally, other operators).
 */
int execute_complex_command(command *c) {
	
	/**
	 * TODO:
	 * Check if this is a simple command, using the scmd field.
	 * Remember that this will be called recursively, so when you encounter
	 * a simple command you should act accordingly.
	 * Execute nonbuiltin commands only. If it's exit or cd, you should not 
	 * execute these in a piped context, so simply ignore builtin commands. 
	 */
	 if (c->scmd)
	 {
	 	if (execute_nonbuiltin(c->scmd) == -1) 
			{
				return -1;
			}
	 	
	 	return 0;
	 }
	

	/** 
	 * Optional: if you wish to handle more than just the 
	 * pipe operator '|' (the '&&', ';' etc. operators), then 
	 * you can add more options here. 
	 */

	if (!strcmp(c->oper, "|")) {
		
		
		/**
		 * TODO: Create a pipe "pfd" that generates a pair of file descriptors, 
		 * to be used for communication between the parent and the child.
		 * Make sure to check any errors in creating the pipe.
		 */
		int pfd[2], pid, pid_2;
		int status1,status2;
		

		if (pipe(pfd) == -1)
		{
			perror("pipe");
			return -1;
		}


			
		/**
		 * TODO: Fork a new process.
		 * In the child:
		 *  - close one end of the pipe pfd and close the stdout file descriptor.
		 *  - connect the stdout to the other end of the pipe (the one you didn't close).
		 *  - execute complex command cmd1 recursively. 
		 * In the parent: 
		 *  - fork a new process to execute cmd2 recursively.
		 *  - In child 2:
		 *     - close one end of the pipe pfd (the other one than the first child), 
		 *       and close the standard input file descriptor.
		 *     - connect the stdin to the other end of the pipe (the one you didn't close).
		 *     - execute complex command cmd2 recursively. 
		 *  - In the parent:
		 *     - close both ends of the pipe. 
		 *     - wait for both children to finish.
		 */

		 /* child */
		 if ((pid = fork()) == 0)
		 	{
		 		
		 		//close one end of pipe
		 		if (close(pfd[0]) == -1)
		 		{
		 			perror("close");
		 			return -1;
		 		}

		 		//close stdout
		 		if (close(fileno(stdout)) == -1)
		 		{
		 			perror("close");
		 			return -1;
		 		}

				//connect the stdout to the other end of the pipe
		 		if (dup2(pfd[1], fileno(stdout)) == -1)
		 		{
		 			perror("dup2");
		 			return -1;
		 		}

		 		

				//execute complex command cmd1 recursively
		 		if (execute_complex_command(c->cmd1) == -1)
		 		{
		 			return -1;
		 		}


		 		//close another end of pipe
		 		if (close(pfd[1]) == -1)
		 		{
		 			perror("close");
		 			return -1;	
		 		}
		 		
		 		exit(0);

		 	}

		 
		 
		 else if (pid > 0)
			{
								
				/* child2 */
				if ((pid_2 = fork()) == 0)
		 		{
		 		
		 			//close one end of pipe
		 			if (close(pfd[1]) == -1)
		 			{
		 				perror("close");
		 				//exit(1);
		 				return -1;
		 			}
					
					//close stdin
		 			if (close(fileno(stdin)) == -1)
		 			{
		 				perror("close");
		 				return -1;
		 			}

		 		
					//connect the stdin to the other end of the pipe
		 			if (dup2(pfd[0], fileno(stdin)) == -1)
		 			{
		 				perror("dup2");
		 				//exit(3);
		 				return -1;
		 			}

		 			

					//execute complex command cmd1 recursively
		 			if (execute_complex_command(c->cmd2) == -1)
		 			{
		 				//exit(6);
		 				return -1;
		 			}

		 			//close another end of pipe
		 			if (close(pfd[0]) == -1)
		 			{
		 				perror("close");
		 				//exit(1);	
		 				return -1;
		 			}
		 			
		 			exit(0);

		 		}

		 		/* parent */
		 		else if (pid_2 > 0)
		 		{
		 			
		 			//close both end of pipes
		 			if (close(pfd[1]) == -1)
		 			{
		 				perror("close");
		 				//exit(1);
		 				return -1;	
		 			}

		 			if (close(pfd[0]) == -1)
		 			{
		 				perror("close");
		 				//exit(1);
		 				return -1;	
		 			}
					
					//wait for two child
		 			wait(&status1);
		 			wait(&status2);
		 			return 0;

		 		}
		 		
		 		
		 		//fork error between child2 & parent
		 		else
		 		{
					perror("fork");
					return -1;
				}
				
			}

			//fork error between child & parent
			else
			{
				perror("fork");
				return -1;
			}

			return 0;
		
	}

	return 0;
}
