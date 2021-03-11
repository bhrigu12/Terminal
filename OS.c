#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include  <sys/types.h>
#include <fcntl.h>
#include <signal.h>


int sizepipe = 0;
int sizeredirect = 0;

int cd(char **args)
{
  int y = chdir(args[1]);
  if (args[1] == NULL || y!= 0) {
     printf("wrong command\n");
  } 
  return 1;
}

 void Handler(int sig_num) 
{ 
//    signal(SIGINT, sigintHandler); 
    
}


void parsespace(char s[500] , char *parse_array[1000] , char *parse_array1[1000]){
	char *str = strtok(s," ");
	int i=0;
	while(str){
		char a[1000] = "";
		strcpy(a,str);
		//printf("%c\n",a[0] );
		if(a[0]=='<'||a[0]=='>'|| a[0]=='1' || a[0]=='2'){
			break;
		}
		parse_array[i++]=str;
		str = strtok(NULL, " ");
	}
	parse_array[i++]=NULL;
	int j=0;
	while(str){
		parse_array1[j++]=str;
		str = strtok(NULL, " ");
	}
	parse_array1[j++]=NULL;
	sizeredirect = j-1;
}

char *parsepipe(char s[1000] , char *parse_array[1000]){
	char *str = strtok(s,"|");
	int i=0;
	while(str){
		parse_array[i++]=str;
		str = strtok(NULL, "|");
	}
	parse_array[i++]=NULL;
	sizepipe = i-1;
}


int main(){
	int i = 1 ;
	printf("Welcome to my shell\n");
	
	signal(SIGINT, Handler);

	do{

		
		int fd[2];
		printf(">> ");
		// char input_arr[500]; 
		// scanf("%[^\t\n]",input_arr);
		
		char input_arr[1000];
	    fgets(input_arr, 1000, stdin);
	   	input_arr[strlen(input_arr) - 1] = '\0';
		
		char *input_store[1000];
		parsepipe(input_arr,input_store);

		char *redirect_store[1000];
		if(sizepipe==1){
			parsespace(input_arr,input_store,redirect_store);
			int pid;
			int status;
			if(input_store[0] == NULL){
				i=1;
			}
			
			else if(strcmp(input_store[0],"exit")==0){
				i=0;
			}

			else if(strcmp(input_store[0],"cd")==0){
				i = cd(input_store);
			}

			else{ 
				pid = fork();
				if(pid < 0){
					printf("error during fork\n");
					exit(1);
				}
				else if (pid == 0) {
					//printf("%d\n", sizeredirect);
					if(sizeredirect>0){
						if(strcmp(redirect_store[0],">")==0){
							int p = open(redirect_store[1],O_WRONLY|O_TRUNC|O_CREAT,0644);
							dup2(p,STDOUT_FILENO);
							close(p);
						}
						if(strcmp(redirect_store[0],">>")==0){
							int p = open(redirect_store[1],O_RDWR|O_APPEND|O_CREAT,0644);
							dup2(p,STDOUT_FILENO);
							close(p);
						}
						if(strcmp(redirect_store[0],"<")==0){
							int read = open(redirect_store[1],O_RDONLY);
							dup2(read,STDIN_FILENO);
							close(read);
						}
						for(int j=0;j<sizeredirect;j++){
							char dere[1000] = "";
							strcpy(dere,redirect_store[j]);
							//printf("%c\n", dere[0]);
							if(dere[0]=='1'){
								char* abc[1000] ;
								char *str = strtok(redirect_store[j],">");
								int q=0;
								while(str){
									abc[q++]=str;
									str = strtok(NULL, ">");
								}
								abc[q++]=NULL;
								int p = open(abc[1],O_WRONLY|O_CREAT|O_TRUNC,0644);
								dup2(p,STDOUT_FILENO);
								close(p);
							}
							else if(dere[0]=='2' && dere[1]=='>' && dere[2]=='&'){
								dup2(fd[1],fd[2]);
							}

							else if(dere[0]=='2'){
								//printf("%c\n", dere[0]);
								char* abc[1000] ;
								char *str = strtok(redirect_store[j],">");
								int q=0;
								while(str){
									abc[q++]=str;
									str = strtok(NULL, ">");
								}
								abc[q++]=NULL;
								int p = open(abc[1],O_WRONLY|O_CREAT|O_TRUNC,0644);
								dup2(p,STDERR_FILENO);
								close(p);
							}
							
						}
					}

					if(execvp(input_store[0],input_store)<0){
						printf("error during execution check your commands\n");
						exit(1);
					}

						
					
				}
				else{
					while(wait(&status)!=pid);
				}
			}
		}





		else{
			char *pipe_input_store[1000];
			int status;
			int pid1;
			int fd1[2];
			int ret;
			int temp = 0; 
			int l=0;
			for( ; input_store[l]!=NULL ; ){
				parsespace(input_store[l],pipe_input_store,redirect_store);

				if(pipe_input_store[0] == NULL){
					i=1;
				}
				
				else if(strcmp(pipe_input_store[0],"exit")==0){
					i=0;
				}

				else if(strcmp(pipe_input_store[0],"cd")==0){
					i = cd(pipe_input_store);
				}

				ret = pipe(fd1);
				if (ret == -1) {
					printf ("Unable to create pipe\n");
					return 0;
				}
				else{
					pid1 = fork ();
					if(pid1<0){
						printf("error during fork\n");
						exit(1);
					}
					else if(pid1==0){
						if(sizeredirect>0){
							if(strcmp(redirect_store[0],">")==0){
								int p = open(redirect_store[1],O_WRONLY|O_TRUNC|O_CREAT,0644);
								dup2(p,STDOUT_FILENO);
								close(p);
							}
							if(strcmp(redirect_store[0],">>")==0){
								int p = open(redirect_store[1],O_RDWR|O_APPEND|O_CREAT,0644);
								dup2(p,STDOUT_FILENO);
								close(p);
							}
							if(strcmp(redirect_store[0],"<")==0){
								int read = open(redirect_store[1],O_RDONLY);
								dup2(read,STDIN_FILENO);
								close(read);
							}
							for(int j=0;j<sizeredirect;j++){
								char dere[1000] = "";
								strcpy(dere,redirect_store[j]);
								//printf("%c\n", dere[0]);
								if(dere[0]=='1'){
									char* abc[1000] ;
									char *str = strtok(redirect_store[j],">");
									int q=0;
									while(str){
										abc[q++]=str;
										str = strtok(NULL, ">");
									}
									abc[q++]=NULL;
									int p = open(abc[1],O_WRONLY|O_CREAT|O_TRUNC,0644);
									dup2(p,STDOUT_FILENO);
									close(p);
								}
								else if(dere[0]=='2' && dere[1]=='>' && dere[2]=='&'){
									dup2(fd[1],fd[2]);
								}

								else if(dere[0]=='2'){
									//printf("%c\n", dere[0]);
									char* abc[1000] ;
									char *str = strtok(redirect_store[j],">");
									int q=0;
									while(str){
										abc[q++]=str;
										str = strtok(NULL, ">");
									}
									abc[q++]=NULL;
									int p = open(abc[1],O_WRONLY|O_CREAT|O_TRUNC,0644);
									dup2(p,STDERR_FILENO);
									close(p);
								}
								
							}
						}

						//close(fd1[0]);
						dup2(temp,0);



						if(l<sizepipe-1){
							dup2(fd1[1],1);
						}
						close(fd1[0]);
						if(execvp(pipe_input_store[0],pipe_input_store)<0){
							printf("error during execution check your commands\n");
							exit(1);
						}

					}
					else {
						
						close(fd1[1]);
						//wait(NULL);
						temp = fd1[0];
						l++;
						// int s =0;
						// while(s++<=100000);
					}

				}

			}
		}


		printf("\n");
	
	}while(i==1);

	return EXIT_SUCCESS;
}