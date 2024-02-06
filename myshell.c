#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
// arglist - a list of char* arguments (words) provided by the user
// it contains count+1 items, where the last item (arglist[count]) and *only* the last is NULL
// RETURNS - 1 if should continue, 0 otherwise
int process_arglist(int count, char** arglist);

// prepare and finalize calls for initialization and destruction of anything required
int prepare(void);
int finalize(void);
int executeBackground(int count,char **arglist);
int executeInputFile(int count, char **arglist);
int find_symbol(int count, char** arglist, char ch);

int prepare(void){
	///////// put here proper signal reactions
	if(signal(SIGINT, SIG_IGN)==SIG_ERR){
        perror("errer\n");
		return -1;
	}///////zombies
	return 0;
}

int finalize(){
	return 0;
}

int process_arglist(int count, char** arglist){
    int pipe_location = -1;
    if(strcmp(arglist[count-1],'&')==0){
        arglist[count-1] = NULL;
        if(executeBackground(count,arglist)==-1){
            arglist[count-1] = "&";
            perror("errer\n");
            return 0;
        }
        arglist[count-1] = "&";
        return 1;
    }
    if(find_symbol(count,arglist,'<')!=-1){
        arglist[count-2] = NULL;
        if(executeInputFile(count,arglist)==-1){
            arglist[count-2] = "<";
            perror("errer\n");
            return 0;
        }
        arglist[count-2] = "<";
        return 1;
    }
    if(find_symbol(count,arglist,'>')!=-1){
        arglist[count-2] = NULL;
        if(executeOutputFile(count,arglist)==-1){
            arglist[count-2] = ">";
            perror("errer\n");
            return 0;
        }
        arglist[count-2] = ">";
        return 1;
    }
    pipe_location = find_symbol(count,arglist,'|');
    if(pipe_location!=-1){
        arglist[pipe_location] = NULL;
        if(executePipe(pipe_location,count,arglist)==-1){///////////////////////////////////////////////
            arglist[pipe_location] = "|";
            perror("errer\n");
            return 0;
        }
        arglist[pipe_location] = "|";
        return 1;
    }
    if(executeNone(count,arglist)==-1){///////////////////////////////////////////////
        perror("errer\n");
        return 0;
    }
    return 1;
}

int executePipe(int pipe_location, int count,char **arglist){
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
			perror("errer\n");
            exit(1);
		}
	    close(pipe_fd[0]);
        if (dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
			perror("errer\n");
			exit(1);
		}
        close(pipe_fd[1]);
		if (execvp(arglist[0],arglist) == -1) {
			perror("errer\n");
			exit(1);
		}
    }
    second_child_pid = fork();
	if (second_child_pid == -1) {
        return -1;
    }
    else{
	    if (second_child_pid == 0) {
	    	if(signal(SIGINT, SIG_DFL)==SIG_ERR){
	    		perror("errer\n");
	    		exit(1);
	    	}
	    	close(pipe_fd[1]);
	    	if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
			perror("errer\n");
			exit(1);
	    	}
	    	close(pipe_fd[0]);
	    	if (execvp(arglist[0],arglist) == -1) {
	    		perror("errer\n");
	    		exit(1);
	    	}
        }
        else{
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
    }
    return 0;
}

int executeOutputFile(int count,char **arglist){
    pid_t pid = fork();
	if(pid == -1){
		return -1;
	}
	if(pid ==0){
		int file = open(arglist[count-1],O_WRONLY | O_CREAT | O_TRUNC , 0777);
		if(signal(SIGINT, SIG_DFL)==SIG_ERR){
			perror("errer\n");
			exit(1);
		}
        if(file == -1){
			perror("errer\n");
			exit(1);
		}
        if (dup2(file, STDOUT_FILENO) == -1) {
            close(file);
			perror("errer\n");
			exit(1);
        }
        close(file);
		if(execvp(arglist[0],arglist)==-1){
			perror("errer\n");
			exit(1);
		}

    }
    else{
		pid_t child_pid = waitpid(pid,NULL,0);
		if(child_pid == -1){ ////////////////////////////// read about special error reasons
			if(errno!= ECHILD && errno!= EINTR){
				return -1;
			}
		}
        return 0;
    }
    return 0;
}

int find_symbol(int count, char** arglist, char ch){
    int i=0;
    for(i=0;i<count;i++){
        if(strlen(arglist[i])>0 && arglist[i][0] == ch){
            return i;
        }
    }
    return -1;
}

int executeBackground(int count,char **arglist){
    pid_t pid = fork();
	if(pid == -1){
		return -1;
	}
	if(pid ==0){
		if(execvp(arglist[0],arglist)==-1){
			perror("errer\n");
			exit(1);
		}
	}
	else{
		return 0;
	}
    return 0;
}

int executeInputFile(int count, char **arglist){ ///////////////////////new and needs checking
    pid_t pid = fork();
	if(pid == -1){
		return -1;
	}
    if(pid ==0){
		FILE* file = fopen(arglist[count-1], "r");
		if(signal(SIGINT, SIG_DFL)==SIG_ERR){
			perror("errer\n");
			exit(1);
		}
		if(file == NULL){
			perror("errer\n");
			exit(1);
		}
		if (dup2(fileno(file), STDIN_FILENO) == -1) {
            fclose(file);
			perror("errer\n");
			exit(1);
        }
		fclose(file);
		if(execvp(arglist[0],arglist)==-1){
			perror("errer\n");
			exit(1);
		}
	}
    else{
		pid_t child_pid = waitpid(pid,NULL,0);
		if(child_pid == -1){ ////////////////////////////// read about special error reasons
			if(errno!= ECHILD && errno!= EINTR){
				return -1;
			}
		}
        return 0;
    }
    return 0;
}