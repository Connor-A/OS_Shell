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


int main(int argc, char *argv[])
{
	int args = 0; //will hold the length of the argument list
	int go = 1;
	char bashcommand[2000];
	int debug = 0;
	pid_t pid, ret_pid;
	int status;


	struct CommandData *data = &(struct CommandData){.numcommands = 0, .infile="", .outfile="", .background=0};

	while(go)
	{
		char* path = getenv("PATH");
		const char * delim = ":";
		//printf("PATH :%s\n",(path!=NULL)? path : "getenv returned NULL");
		//printf("end test\n");

		char* Param_List[13];
		char subPATH[100];

		char cwd[PATH_MAX];
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			printf("%s$  ", cwd);
		} else {
			perror("getcwd() error");
			return 1;
		}

		if(scanf("%[^\n]" , bashcommand) == 1)
			printf("\n");
		else
			printf("%s\n", "Failed");

		char unused = getchar();
		unused = unused + unused;

		if(strcmp(bashcommand, "exit") == 0){
			go = 0;
		}
		
		if (strcmp(bashcommand, "DEBUG=yes") == 0 )
		{
			printf("Entering debug mode\n\n");
			debug = 1;
		}		
		else if (strcmp(bashcommand, "DEBUG=no") == 0 )
		{
			printf("Exiting debug mode\n\n");
			debug = 0;
		}
		else
		{
			ParseCommandLine(bashcommand, data); //populate the struct
			
			strcpy(subPATH,strtok(path, delim));
			Param_List[0] = data->TheCommands[0].command; 
			for (int i = 0; i < data->TheCommands[0].numargs; ++i)
				Param_List[i+1] = 	data->TheCommands[0].args[i];
			Param_List[data->TheCommands[0].numargs+1] = NULL;

			if (strcmp(data->TheCommands[0].command, "cd") == 0 && data->TheCommands[0].numargs == 0 )
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
				printf("%s\n",cwd);
			}		
			else if (strcmp(data->TheCommands[0].command, "exit") == 0 )
			{
				printf("The program will now terminate\n\n");
			}		
			else if (strcmp(data->TheCommands[0].command, "set") == 0 )
			{
				printf("SET!\n\n");
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
					int fdi, fdo;
					if(data->infile != NULL)
					{
						fdi = open(data->infile, O_RDONLY ,0);
						dup2(fdi, STDIN_FILENO);
						close(fdi);
					}
					if(data->outfile != NULL)
					{
						fdo = open(data->outfile, O_WRONLY|O_CREAT|O_TRUNC, 0666);
						dup2(fdo, STDOUT_FILENO);
						close(fdo);
					}
					
					if(execvp(data->TheCommands[0].command, Param_List) == -1)
					{
						while( subPATH != NULL )
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
				}
				else
				{
					ret_pid = wait(&status);
					printf("Child complete: %d\n", ret_pid);
				}
			}





			if(debug == 1)
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

