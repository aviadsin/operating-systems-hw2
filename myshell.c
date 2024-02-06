#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// arglist - a list of char* arguments (words) provided by the user
// it contains count+1 items, where the last item (arglist[count]) and *only* the last is NULL
// RETURNS - 1 if should continue, 0 otherwise
int process_arglist(int count, char** arglist);

// prepare and finalize calls for initialization and destruction of anything required
int prepare(void);
int finalize(void);

int main(void)
{
	if (prepare() != 0)
		exit(1);
	
	while (1)
	{
		char** arglist = NULL;
		char* line = NULL;
		size_t size;
		int count = 0;

		if (getline(&line, &size, stdin) == -1) {
			free(line);
			break;
		}
    
		arglist = (char**) malloc(sizeof(char*));
		if (arglist == NULL) {
			printf("malloc failed: %s\n", strerror(errno));
			exit(1);
		}
		arglist[0] = strtok(line, " \t\n");
    
		while (arglist[count] != NULL) {
			++count;
			arglist = (char**) realloc(arglist, sizeof(char*) * (count + 1));
			if (arglist == NULL) {
				printf("realloc failed: %s\n", strerror(errno));
				exit(1);
			}
      
			arglist[count] = strtok(NULL, " \t\n");
		}
    
		if (count != 0) {
			if (!process_arglist(count, arglist)) {
				free(line);
				free(arglist);
				break;
			}
		}
    
		free(line);
		free(arglist);
	}
	
	if (finalize() != 0)
		exit(1);

	return 0;
}

#define _POSIX_C_SOURCE 200809L
#include <signal.h>

int prepare(){
	///////// put here proper signal reactions
	signal(SIGINT, SIG_IGN);
	return 0;
}

int finalize(){
	return 0;
}


int process_arglist(int count, char** arglist){
	char *** details = extract_details(count,arglist); ////// error in this method
	if(details == NULL){
		fprintf(stderr, "This is an error message.\n");
		return 0;
	}
	if(details[1][0] == NULL){
		if(executeNull(details)==-1)
		{
			fprintf(stderr, "This is an error message.\n");
			return 0;
		}
	}
	if(strcmp(details[1][0],"&") == 0){
		if(execute1(details)==-1)
		{
			fprintf(stderr, "This is an error message.\n");
			return 0;
		}
	}
	if(strcmp(details[1][0],"<") == 0){
		if(execute2(details)==-1)
		{
			fprintf(stderr, "This is an error message.\n");
			return 0;
		}
	}
	if(strcmp(details[1][0],">") == 0){
		if(execute3(details)==-1)
		{
			fprintf(stderr, "This is an error message.\n");
			return 0;
		}
	}
	if(strcmp(details[1][0],"|") == 0){
		if(execute4(details)==-1)
		{
			fprintf(stderr, "This is an error message.\n");
			return 0;
		}
	}
	return 1;
}

int executeNull(char*** details);
int executeNull(char*** details){
	pid_t pid = fork();
	if(pid == -1){
		return -1;
	}
	if(pid ==0){
		if(signal(SIGINT, SIG_DFL)==SIG_ERR){
			fprintf(stderr, "This is an error message.\n");
			exit(1);
		}
		if(execvp(details[0][0],details[0])==-1){
			fprintf(stderr, "This is an error message.\n");
			exit(1);
		}
		fprintf(stderr, "This is an error message.\n");
		exit(1);
	}
	else{
		int status;
		pid_t child_pid = waitpid(pid,&status,0);
		if(child_pid == -1){ ////////////////////////////// read about special error reasons
			if(errno!= ECHILD && errno!= EINTR){
				return -1;
			}
		}
	}
	return 0;
}

int execute1(char*** details);
int execute1(char*** details){
	pid_t pid = fork();
	if(pid == -1){
		return -1;
	}
	if(pid ==0){
		if(execvp(details[0][0],details[0])==-1){
			fprintf(stderr, "This is an error message.\n");
			exit(1);
		}
		fprintf(stderr, "This is an error message.\n");
		exit(1);
	}
	else{
		return 0;
	}
}

int execute2(char*** details);
int execute2(char*** details){
	pid_t pid = fork();
	if(pid == -1){
		return -1;
	}
	if(pid ==0){
		FILE* file = fopen(details[2][0], "r");
		if(signal(SIGINT, SIG_DFL)==SIG_ERR){
			fprintf(stderr, "This is an error message.\n");
			exit(1);
		}
		if(file == NULL){
			fprintf(stderr, "This is an error message.\n");
			exit(1);
		}
		if (dup2(fileno(file), STDIN_FILENO) == -1) {
            fclose(file);
			fprintf(stderr, "This is an error message.\n");
            exit(1);
        }
		fclose(file);
		if(execvp(details[0][0],details[0])==-1){
			fprintf(stderr, "This is an error message.\n");
			exit(1);
		}
		fprintf(stderr, "This is an error message.\n");
		exit(1);
	}
	else{
		int status;
		pid_t child_pid = waitpid(pid,&status,0);
		if(child_pid == -1){ ////////////////////////////// read about special error reasons
			if(errno!= ECHILD && errno!= EINTR){
				return -1;
			}
		}
	}
	return 0;
}

int execute3(char*** details);
int execute3(char*** details){
	pid_t pid = fork();
	if(pid == -1){
		return -1;
	}
	if(pid ==0){
		FILE* file = fopen(details[2][0], "w");
		if(signal(SIGINT, SIG_DFL)==SIG_ERR){
			fprintf(stderr, "This is an error message.\n");
			exit(1);
		}
		if(file == NULL){
			fprintf(stderr, "This is an error message.\n");
			exit(1);
		}
		if (dup2(fileno(file), STDOUT_FILENO) == -1) {
            fclose(file);
			fprintf(stderr, "This is an error message.\n");
            exit(1);
        }
		fclose(file);
		if(execvp(details[0][0],details[0])==-1){
			fprintf(stderr, "This is an error message.\n");
			exit(1);
		}
		fprintf(stderr, "This is an error message.\n");
		exit(1);
	}
	else{
		int status;
		pid_t child_pid = waitpid(pid,&status,0);
		if(child_pid == -1){ ////////////////////////////// read about special error reasons
			if(errno!= ECHILD && errno!= EINTR){
				return -1;
			}
		}
	}
	return 0;
}


int execute4(char*** details);
int execute4(char*** details){

	int pipe_fd[2];
	pid_t first_child_pid;
	pid_t second_child_pid;
	pid_t wait1_pid;
	pid_t wait2_pid;
	if (pipe(pipe_fd) == -1) {
		return -1;
	}
	first_child_pid = fork();
	if (first_child_pid == -1) {
		return -1;
	}
	if (first_child_pid == 0) {
		if(signal(SIGINT, SIG_DFL)==SIG_ERR){
			fprintf(stderr, "This is an error message.\n");
			exit(1);
		}
		close(pipe_fd[0]);
		if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
			fprintf(stderr, "This is an error message.\n");
			exit(1);
		}
		close(pipe_fd[1]);
		if (execvp(details[0][0],details[0]) == -1) {
			fprintf(stderr, "This is an error message.\n");
			exit(1);
		}
		fprintf(stderr, "This is an error message.\n");
		exit(1);
	}
	second_child_pid = fork();
	if (second_child_pid == -1) {
        return -1;
    }
	if (second_child_pid == 0) {
		if(signal(SIGINT, SIG_DFL)==SIG_ERR){
			fprintf(stderr, "This is an error message.\n");
			exit(1);
		}
		close(pipe_fd[1]);
		if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
			fprintf(stderr, "This is an error message.\n");
			exit(1);
		}
		close(pipe_fd[0]);
		if (execvp(details[2][0],details[2]) == -1) {
			fprintf(stderr, "This is an error message.\n");
			exit(1);
		}
		fprintf(stderr, "This is an error message.\n");
		exit(1);
	}
	close(pipe_fd[0]);
    close(pipe_fd[1]);
	wait1_pid = waitpid(first_child_pid, NULL, 0);
	if(wait1_pid == -1){ ////////////////////////////// read about special error reasons
		if(errno!= ECHILD && errno!= EINTR){
			return -1;
		}
	}
    wait2_pid =waitpid(second_child_pid, NULL, 0);
	if(wait2_pid == -1){ ////////////////////////////// read about special error reasons
		if(errno!= ECHILD && errno!= EINTR){
			return -1;
		}
	}
	return 0;
}


char ***extract_details(int count, char** arglist);
char ***extract_details(int count, char** arglist){
	char ***res;
	int size1=0;
	int size2=0;
	int i=0;

	res = (char***)malloc(3 * sizeof(char**));
	if(res == NULL){
		return NULL;
	}
	res[0] = (char**)malloc(1 * sizeof(char*));
	if(res[0] == NULL){
		free(res);
		return NULL;
	}
	res[1] = NULL;
	res[2] = NULL;

	while(count !=i && strcmp(arglist[i],">")!=0 && strcmp(arglist[i],"<")!=0 && strcmp(arglist[i],"&")!=0 && strcmp(arglist[i],"|")!=0)
	{
		if(i==0){
			res[0][0] = (char*)malloc((strlen(arglist[0])+1)*sizeof(char));
			if(res[0][0] == NULL){
				free(res[0]);
				free(res);
				return NULL;
			}
			strcpy(res[0][0],arglist[0]);
		}
		else{
			res[0] = (char**)realloc(res[0],(size1+1)*sizeof(char*));
			if(res[0][0] == NULL){
				free(res);
				return NULL;
			}
			res[0][size1] = (char*)malloc((strlen(arglist[i])+1)*sizeof(char));
			if(res[0][size1] == NULL){
				int j=0;
				for(j=0;j<size1;j++)
				{
					free(res[0][j]);
				}
				free(res[0]);
				free(res);
				return NULL;
			}
			strcpy(res[0][size1],arglist[i]);
		}
		i++;
		size1++;
	}
	res[0] = (char**)realloc(res[0],(size1+1)*sizeof(char*));
	if(res[0] == NULL){
		free(res);
		return NULL;
	}
	res[0][size1] = NULL;
	if(i!=count){
		res[1] = (char**)malloc(1 * sizeof(char*));
		if(res[1] == NULL){
			int j;
			for(j=0;j<size1;j++)
			{
				free(res[0][j]);
			}
			free(res[0]);
			free(res);
			return NULL;
		}
		res[1][0] = (char*)malloc((strlen(arglist[i])+1)*sizeof(char));

		if(res[1][0] == NULL){
			int j;
			for(j=0;j<size1;j++)
			{
				free(res[0][j]);
			}
			free(res[0]);
			free(res[1]);
			free(res);
			return NULL;
		}

		strcpy(res[1][0],arglist[i]);
		i++;
	}
	if(i!=count){
		res[2] = (char**)malloc(1 * sizeof(char*));

		if(res[2] == NULL){
			int j;
			for(j=0;j<size1;j++)
			{
				free(res[0][j]);
			}
			free(res[0]);
			free(res[1][0]);
			free(res[1]);
			free(res);
			return NULL;
		}

		while(count !=i)
		{
			if(size2==0){
				res[2][0] = (char*)malloc((strlen(arglist[i])+1)*sizeof(char));

				if(res[2][0] == NULL){
					int j;
					for(j=0;j<size1;j++)
					{
						free(res[0][j]);
					}
					free(res[0]);
					free(res[1][0]);
					free(res[1]);
					free(res[2]);
					free(res);
					return NULL;
				}

				strcpy(res[2][0],arglist[i]);
			}
			else{
				res[2] = (char**)realloc(res[2],(size2+1)*sizeof(char*)); //////// memory error

				if(res[2] == NULL){
					int j;
					for(j=0;j<size1;j++)
					{
						free(res[0][j]);
					}
					free(res[0]);
					free(res[1][0]);
					free(res[1]);
					free(res);
					return NULL;
				}

				res[2][size2] = (char*)malloc((strlen(arglist[i])+1)*sizeof(char));

				if(res[2][size2] == NULL){
					int j;
					for(j=0;j<size1;j++)
					{
						free(res[0][j]);
					}
					free(res[0]);
					free(res[1][0]);
					free(res[1]);
					for(j=0;j<size2;j++)
					{
						free(res[2][j]);
					}
					free(res[2]);
					free(res);
					return NULL;
				}

				strcpy(res[2][size2],arglist[i]);
			}
			i++;
			size2++;
		}
		res[2] = (char**)realloc(res[2],(size2+1)*sizeof(char*)); //////// memory error

		if(res[2]== NULL){
			int j;
			for(j=0;j<size1;j++)
			{
				free(res[0][j]);
			}
			free(res[0]);
			free(res[1][0]);
			free(res[1]);
			free(res);
			return NULL;
		}

		res[2][size2] = NULL;
	}
	return res;
}