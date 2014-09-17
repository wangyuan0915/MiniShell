#ifndef _SHELL_H
#define _SHELL_H

/* built-in commands */
#define BUILTIN_CD   1
#define BUILTIN_EXIT 2

typedef struct simple_command_t {
	char *in, *out, *err;    /* Files for redirection, optional */
	char **tokens;           /* Program and its parameters */
	int builtin;             /* Builtin commands, e.g., cd */
} simple_command;

typedef struct command_t {
	struct command_t *cmd1, *cmd2;  /* Two commands piped between each other; 
	                 In turn, each one can contain multiple commands itself; */
	simple_command* scmd; /* Simple command, no pipe */
	char oper[2];   /* In this assignment, consider only "|".
	                Optional: implement other operators: ";", "&&", etc. */
} command;

#endif

