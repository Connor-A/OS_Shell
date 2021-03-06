/*
 * Headstart for Assignment 1
 * CS 4420/5420
 * 
 *
 * Student Name:  Nathan Egan and Connor Abla 
 * DEBUG=no and DEBUG=yes don't work. I think
 * it's due to a parser issue.
 *
 */

#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

#define READ 0
#define WRITE 1


int main(int argc, char *argv[], char **envp)
{
	int go = 1; //tracks if the program should end or not
	int debug = 0; //tracks if debug mode is on
		
	while(go)
	{
		//We define these in the loop to make sure they're cleaned on each loop. Its inefficient, but it works!
		
		int args = 0; //will hold the length of the argument list
		char bashcommand[2000]; //buffer for input commands
		pid_t pid; //tracks the pid of the process. Used for forking
		int status; //used for forking
		const char * delim = ":"; //used for parsing the path


		struct CommandData *data = &(struct CommandData){.numcommands = 0, .infile="", .outfile="", .background=0}; //creates the struct for tracking input commands
		char* path = getenv("PATH"); //Gets the PATH 

		char* Param_List[13]; //list of parameters
		char subPATH[100]; //used for parsing each file directory in the path

		char cwd[PATH_MAX]; //current working directory
		if (getcwd(cwd, sizeof(cwd)) != NULL) { //getting the working directory
			printf("%s$  ", cwd);
		} else {
			perror("getcwd() error");
			return 1;
		}

		if(scanf("%[^\n]" , bashcommand) == 1) //getting the command from the user
			printf("\n");
		else
			printf("%s\n", "Failed");

		char unused = getchar(); //beating -Werror BECUASE ITS STUPID
		unused = unused + unused;

		if(strcmp(bashcommand, "exit") == 0){
			go = 0; //stopping the program loop
		}
		
		if (strcmp(bashcommand, "DEBUG=yes") == 0 ) //activate debug
		{
			printf("Entering debug mode\n\n");
			debug = 1;
		}		
		else if (strcmp(bashcommand, "DEBUG=no") == 0 ) //deactivate debug
		{
			printf("Exiting debug mode\n\n");
			debug = 0;
		}
		else
		{
			ParseCommandLine(bashcommand, data); //populate the struct
			

				Param_List[0] = data->TheCommands[0].command; 
				
				for (int i = 0; i < data->TheCommands[0].numargs; ++i)  //creates a arg list thats compatible with execv
					Param_List[i+1] = 	data->TheCommands[0].args[i];
				
				Param_List[data->TheCommands[0].numargs+1] = NULL;

				if (strcmp(data->TheCommands[0].command, "cd") == 0 && data->TheCommands[0].numargs == 0 ) //checking built in commands
				{

					if (chdir(getenv("HOME")) != 0)
						printf("Err\n");
				}
				else if (strcmp(data->TheCommands[0].command, "cd") == 0 && data->TheCommands[0].numargs == 1)
				{
					if (chdir(strcat(strcat(cwd,"/"),data->TheCommands[0].args[0]) ) != 0)
						printf("Err\n");
				}

				else if (strcmp(data->TheCommands[0].command, "pwd") == 0 )
				{
					printf("%s\n",cwd); //print working directory
				}		
				else if (strcmp(data->TheCommands[0].command, "exit") == 0 )
				{
					printf("The program will now terminate\n\n");
				}		
				else if (strcmp(data->TheCommands[0].command, "set") == 0 )
				{
					for(char **env = envp; *env != 0; env++) //printing enviroment vars
					{
						char *myEnv = *env;
						printf("%s\n", myEnv);
					}
				}

				else 
				{
					int tmpin = dup(0); //temp holders for stdin, stdout
					int tmpout = dup(1);

					int fdin, fdout;
					if(data -> infile != NULL)
						fdin = open(data -> infile, O_RDONLY, 0); //input redirection
					else
						fdin = dup(0);

					pid = fork();
					if(pid < 0) //failed fork
					{
						printf("Fork Failed\n");
						exit(-1);
					}
					else if(pid == 0)
					{
						for(int i = 0; i < data -> numcommands; ++i)
						{
							dup2(fdin, 0);
							close (fdin);
							if (i == data -> numcommands -1)
							{
								if (data -> outfile != NULL)
								{
									fdout = open(data->outfile, O_WRONLY|O_CREAT|O_TRUNC, 0666); //output redirection
								}
								else
								{
									fdout = dup(tmpout);
								}
							}
							else
							{
								int fdpipe[2]; //pipe to another process
								if(pipe(fdpipe) == -1)
									printf("Pipe failed");
								fdin = fdpipe[0];
								fdout = fdpipe[1];
							}
							dup2(fdout, 1);
							close(fdout);
							int pipeid = fork();
							if(pipeid == 0) //child
							{
								if(execvp(data->TheCommands[0].command, Param_List) < 0)
								{
									strcpy(subPATH,strtok(path, delim));
									while( subPATH != NULL ) //Try another path in PATH - I know we didn't need to do this but...
									{
										strcat(subPATH, "/");
										strcat(subPATH, data->TheCommands[0].command);
										execvp(subPATH, Param_List);
										strcpy(subPATH,strtok(NULL, delim));

									}
								}
								else
								{
									printf("Exec failed\n");
									exit(-1);
								}
								exit(0);
							}
							else
							{
								dup2(0,tmpin);
								dup2(1,tmpin);
							}
						}
						exit(0);
					}
					else if(data->background == 0) //parent, with wait!
					{
						wait(&status);
					}
				}



			if(debug == 1) //prints debug info
			{
					//iterte over each command and rint it
				for(int j = 0; j < data->numcommands; j++)
				{
					printf("Command #"); printf("%u",j); printf(": ");
					printf("%s\n", data->TheCommands[j].command);
					args = data->TheCommands[j].numargs;

					//if there are arguments for this command, list them

					//iterate over each argument for this command
					for(int i = 0; i < args; i++)
					{
						if(args!=0)
							printf("\targs # %u: ", i);
						printf("%s", data->TheCommands[j].args[i]);

						//comma seperate, except the last arg

						if(i != (args-1))
							printf("\n");
					}
					printf("\n");
				}


					//print input file
				printf("Input File: ");
				printf("%s", data->infile);

					//print output file
				printf("\nOutput File: ");
				printf("%s", data->outfile);
				printf("\n");

					//print foreground/background mode
				printf("This process is running in the:  ");
				if(data -> background == 0)	
					printf("foreground\n");
				else
					printf("background\n");

				char *holder;
				holder = (data -> TheCommands[0].command);
				printf("This process is:  ");
				if( (strcmp(holder, "cd") == 0)  || (strcmp(holder, "pwd") == 0) || 
					(strcmp(holder, "set") == 0) || (strcmp(holder, "exit") == 0) ||
					(strcmp(holder, "DEBUG=yes") == 0) || (strcmp(holder, "DEBUG=no") == 0))
				{
					printf("built in\n");
				}
				else
					printf("not built in\n");
			}
		}
	}

	exit(0);
}

