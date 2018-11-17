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
#include <limits.h>
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"


int main(int argc, char *argv[])
{
	int args = 0; //will hold the length of the argument list
	int go = 1;
	char bashcommand[2000];
	
	struct CommandData *data = &(struct CommandData){.numcommands = 0, .infile="", .outfile="", .background=0};

	while(go)
	{

		printf("test\n");
		char* path = getenv("PATH");
		const char * delim = ":";
		printf("PATH :%s\n",(path!=NULL)? path : "getenv returned NULL");
		printf("end test\n");

		char *const parmList[] = {""};


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
		

		ParseCommandLine(bashcommand, data); //populate the struct
		
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


		char *subPATH = strtok(path, delim);
		while(subPATH !=NULL)
		{
			printf("Path:%s",path);
			printf("\n----------------------------------------------------------\n");
			printf("NEWPATH :%s\n",(subPATH!=NULL)? subPATH: "returned NULL");
			strcat(subPATH, data->TheCommands[0].command);
			execv(subPATH, parmList);

			subPATH = strtok(NULL, delim);
		}







		/*
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
		if( (strcmp(holder, "cd") == 0)  || (strcmp(holder, "pwd") == 0) | 
			(strcmp(holder, "set") == 0) || (strcmp(holder, "exit") == 0) |
			(strcmp(holder, "DEBUG=yes") == 0) || (strcmp(holder, "DEBUG=no") == 0))
		{
			printf("built in\n");
		}
		else
			printf("not built in\n");*/
   }

    exit(0);
}

