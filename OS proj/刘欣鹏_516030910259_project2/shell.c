#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#define MAX_LINE 80
#define BUFFER_SIZE 11
char buffer[BUFFER_SIZE][MAX_LINE/+1][80];
int l, r;
int flag;
int llength[BUFFER_SIZE];
void handle_SIGINT(){
	int i = l, j;
	char tmp[20];
	
	strcpy(tmp, "\n");
	write(STDOUT_FILENO, tmp, strlen(tmp));
	while (i != r) {
		strcpy(tmp, " ");
		for (j = 0; j < llength[i]; j++){
			write(STDOUT_FILENO, buffer[i][j], strlen(buffer[i][j]));
			write(STDOUT_FILENO, tmp, strlen(tmp));
		}
		strcpy(tmp, "\n");
		write(STDOUT_FILENO, tmp, strlen(tmp));
		i = (i + 1) % 11;
	}
	strcpy(tmp, "Output Done!\n");
	write(STDOUT_FILENO, tmp, strlen(tmp));
	flag=1;
}

int setup(char inputBuffer[], char *args[], int *background){
	int length, i, start, ct, j;
	char tmp[50];
	char re[10];
	ct = 0;
	length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
	if (flag==1) return 0;
	start = -1;
	
	if (length == 0) exit(0);
	else if (length < 0){
		perror("error reading the command\n");
		exit(-1);
	}
	
	for (i = 0; i < length; i++){
		switch (inputBuffer[i]){
			case ' ':
			case '\t':
				inputBuffer[i]='\0';
				if (start!=-1){
					args[ct] = &inputBuffer[start];
					ct++;
				}
				start=-1;
				break;
			case '\n':
				if (start!=-1){
					inputBuffer[i]='\0';
					args[ct] = &inputBuffer[start];
					ct++;
				}
				args[ct] = NULL;
				break;
			default:
				if (start == -1) start = i;
				if (inputBuffer[i] == '&') {
					*background = 1;
					inputBuffer[i] = '\0';
				}
				break;
		}
	}
	args[ct] = NULL;
	if (ct <= 0 || args[ct] != NULL) return 0;
	
	strcpy(re, "r");
	if (ct == 1 && !strcmp(re,args[0])){
		for (i=0;i<llength[r-1];i++){
			args[i]=&buffer[r-1][i];
		}
		ct=llength[r-1];
		args[ct]=NULL;
		*background=0;
	}
	else if (ct==2 && !strcmp(re,args[0]) && strlen(args[1])==1){
		i=r;
		int found=0,k;
		while (i!=l){
			i--;
			if (i==-1) i=10;
			if (args[1][0]==buffer[i][0][0]) {found=1;k=i;break;}
		}

		if (!found) return 0;

		for (i=0;i<llength[k];i++){
			args[i]=&buffer[k][i];
		}
		ct=llength[k];
		args[ct]=NULL;
		*background=0;
		
	}
	
	for (i = 0; i < ct; i++){
		for (j = 0; j < strlen(args[i]); j++)
			buffer[r][i][j] = args[i][j];
		buffer[r][i][j] = '\0';
	}
	llength[r] = ct;
	r = (r + 1) % 11;
	if (l == r) l = (l + 1) % 11;
	return 1;
}

int main(void){
	char inputBuffer[MAX_LINE];
	char consts[40];
	char cd[4];
	char ch;
	int background;
	char *args[MAX_LINE/+1];
	int valid;
	signal(SIGINT, handle_SIGINT);
	l = 0;
	r = 0;
	flag = 0;
	strcpy(cd,"cd");
	while (1){
		background = 0;
		strcpy(consts,"COMMAND->");
		write(STDOUT_FILENO, consts, strlen(consts));
		valid = setup(inputBuffer, args, &background);
		if (flag == 1) {
			flag = 0;
			continue;
		}
		if (!valid){
			strcpy(consts,"Invalid instruction!\n");
			write(STDOUT_FILENO, consts, strlen(consts));
			continue;
		}
		pid_t pid;
		if (!strcmp(args[0],cd)){
			int xx=r-1;
			if (xx==-1) xx=10;
			if (llength[xx]==2) chdir(args[1]);
		}
		else{
			pid=fork();
			if (pid == 0) {
				execvp(args[0], args);
				write(STDOUT_FILENO, consts, strlen(consts));
				exit(0);
			}
			else if (background == 0) waitpid(pid);
		}	
	}
}

