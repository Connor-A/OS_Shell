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
	int go = 1; //controls if the program goes
	int debug = 0; //sets debug state

	while(go)
	{
		int args = 0; //will hold the length of the argument list
		char bashcommand[2000]; //holds input from user
		pid_t pid; //keeps track of process ID
		int status; //used for forking


		struct CommandData *data = &(struct CommandData){.numcommands = 0, .infile="", .outfile="", .background=0}; //default struct

		char* path = getenv("PATH"); //gets PATH
		const char * delim = ":"; //used for parsing
		
		char* Param_List[13]; //used for paramlist for execv
		char subPATH[100];    //used for parsing PATH

		char cwd[PATH_MAX];   //current working directory
		if (getcwd(cwd, sizeof(cwd)) != NULL) { //get cwd
			printf("%s$  ", cwd);
		} else {
			perror("getcwd() error");
			return 1;
		}

		if(scanf("%[^\n]" , bashcommand) == 1) //get command from user
			printf("\n");
		else
			printf("%s\n", "Failed");

		char unused = getchar(); //Werror made me do this.
		unused = unused + unused;

		if(strcmp(bashcommand, "exit") == 0){ //Exit the program!
			go = 0;
		}
		
		if (strcmp(bashcommand, "DEBUG=yes") == 0 ) //sets debug state to yes
		{
			printf("Entering debug mode\n\n");
			debug = 1;
		}		
		else if (strcmp(bashcommand, "DEBUG=no") == 0 ) //sets debug state to no
		{
			printf("Exiting debug mode\n\n");
			debug = 0;
		}
		else
		{
			ParseCommandLine(bashcommand, data); //populate the struct
		
				if (strcmp(data->TheCommands[0].command, "cd") == 0 && data->TheCommands[0].numargs == 0 )//built in commands
				{

					if (chdir(getenv("HOME")) != 0)
						printf("Err\n");
				}
				else if (strcmp(data->TheCommands[0].command, "cd") == 0 && data->TheCommands[0].numargs == 1)
				{
					if (chdir(strcat(strcat(cwd,"/"),data->TheCommands[0].args[0]) ) != 0)
						printf("Err\n");
				}

				else if (strcmp(data->TheCommands[0].command, "pwd") == 0 )//print working directory
				{
					printf("%s\n",cwd);
				}		
				else if (strcmp(data->TheCommands[0].command, "exit") == 0 )//exit the program
				{
					printf("The program will now terminate\n\n");
				}		
				else if (strcmp(data->TheCommands[0].command, "set") == 0 ) //print the envirment variables
				{
					for(char **env = envp; *env != 0; env++)
					{
						char *thisEnv = *env;
						printf("%s\n", thisEnv);
					}
				}

				else 
				{
					pid = fork();
					if(pid < 0)
					{
						printf("Fork Failed\n");
						exit(-1);
					}
					else if(pid == 0)
					{
						int f2 = 0, fdi, fdo;
						if(data -> numcommands == 2)
						{
							int fdpipe[2];
							if(pipe(fdpipe) != 0)
							{
								exit(1);
							}
							f2 = fork();
							if (f2 == 0)
							{
								close(fdpipe[0]);
								dup2(fdpipe[1],1);
								close(fdpipe[1]);
								
								if(data->infile != NULL)
								{
									fdi = open(data->infile, O_RDONLY ,0);
									dup2(fdi, 0);
									close(fdi);
								}
							}
							else
							{
								close(fdpipe[1]);
								dup2(fdpipe[0],0);
								close(fdpipe[0]);
								if(data->outfile != NULL)
								{
									fdo = open(data->outfile, O_WRONLY|O_CREAT|O_TRUNC, 0666);
									dup2(fdo, STDOUT_FILENO);
									close(fdo);
								}
							}
						}
						else
						{
							if(data->infile != NULL)
							{
								fdi = open(data->infile, O_RDONLY ,0); //input redirection
								dup2(fdi, 0);
								close(fdi);
							}
							if(data->outfile != NULL)
							{
								fdo = open(data->outfile, O_WRONLY|O_CREAT|O_TRUNC, 0666); //output redirection
								dup2(fdo, STDOUT_FILENO);
								close(fdo);
							}
						}
						if(f2 == 0) //child!
						{
							Param_List[0] = data->TheCommands[0].command; 
				
							for (int i = 0; i < data->TheCommands[0].numargs; ++i)
								Param_List[i+1] = 	data->TheCommands[0].args[i];
				
							Param_List[data->TheCommands[0].numargs+1] = NULL;

							if(execvp(data->TheCommands[0].command, Param_List) < 0)
							{
								strcpy(subPATH,strtok(path, delim));
								while( subPATH != NULL )
								{
									strcat(subPATH, "/");
									strcat(subPATH, data->TheCommands[0].command);
									execvp(subPATH, Param_List);
									strcpy(subPATH,strtok(NULL, delim));

								}
							}
						}
						else if(f2 != 0)//parent!
						{
							Param_List[0] = data->TheCommands[1].command; 
				
							for (int i = 0; i < data->TheCommands[1].numargs; ++i)
								Param_List[i+1] = 	data->TheCommands[1].args[i];
				
							Param_List[data->TheCommands[1].numargs+1] = NULL;
							
							if(execvp(data->TheCommands[1].command, Param_List) < 0)
							{
								strcpy(subPATH,strtok(path, delim));
								while( subPATH != NULL )
								{
									strcat(subPATH, "/");
									strcat(subPATH, data->TheCommands[1].command);
									execvp(subPATH, Param_List);
									strcpy(subPATH,strtok(NULL, delim));

								}
							}	
						}
					}
					else if(data->background == 0)//run in background?
					{
						wait(&status);
					}
				}



			if(debug == 1)//print debug information
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
