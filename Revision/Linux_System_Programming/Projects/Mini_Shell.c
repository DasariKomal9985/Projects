#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define Max_Input 100
#define Max_Args 10

int main()
{
	char input[Max_Input];
	char *args[Max_Args];

	while(1)
	{
		printf("My Shell");
		fflush(stdout);
		if(fgets(input, sizeof(input), stdin) == NULL)
		{
			break;
		}
		input[strcspn(input, "\n")] = '\0';
		if(strlen(input) == 0)
		{
			continue;
		}
		if(strcmp(input, "exit") == 0)
		{
			printf("Exit Shell");
			break;
		}
		int count = 0;
		char *token = strtok(input, " ");
		while(token != NULL && count < Max_Args - 1)
		{
			args[count] = token;
			count++;
			token = strtok(NULL, " ");
		}
		args[count] = NULL;
		pid_t pid = fork();

		if(pid < 0)
		{
			perror("fork");
			continue;
		}
		if(pid == 0)
		{
			execvp(args[0], args);
			perror("execvp");
			exit(1);
		}
		else
		{
			waitpid(pid, NULL, 0);
		}
	}
	return 0;
}
