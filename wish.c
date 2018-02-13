#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include <fcntl.h>
void printError(){
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
}

int checkBuiltIn(char *arg){
   printf("builtIn called\n");
   int flag=0;
   if(strcmp("exit",arg)==0 || strcmp("cd",arg)==0 || strcmp("path",arg)==0)
        flag=1;
   return flag; 
        
}



int newProcess(int myargc,char *myargv[]){
        int rc=fork();
        if(rc<0){ //Fork Error
                printError();
                exit(1); 
        }
        else if(rc==0){ //Child process
                char *path=(char*)malloc(128);
                path=strdup("/bin/");
                path=strcat(path,myargv[0]);
                if(access(path,X_OK)==0){
                        if(execv(path,myargv)==-1){
                                printError();
                                exit(1);
                        }     
                }
                else{
                        path="/usr/bin/";
                        path=strcat(path,myargv[0]);
                        if(access(path,X_OK)==0){
                                printf("\n\naccessed bin2/\n");
                                if(execv(path,myargv)==-1){
                                        printError();
                                        exit(1);
                                }     
                        }
                        else{
                              printError();
                              exit(1);   
                        }
                }
                return 1; 
        }
        else{
                
                rc=wait(NULL);
                //printf("STUCK IN PARENT of %d (wc:%d) (pid:%d) \n",rc,wc,(int) getpid());
        }
        return 0;
}

void loop_wish(){
        
        char *line;
        size_t len=0;
        do{
                int i=0;
                printf("wish> ");
                getline(&line,&len,stdin); //reads the entire STDIN command.
                char *myargs[sizeof(line)]; //Can't be more than the line's size.
               
                myargs[0]= strtok(line," "); // first call returns pointer to first part of user_input
                                             // separated by delim
                while(myargs[i]!=NULL){
                        i++;
                        myargs[i]=strtok(NULL," "); // every call with NULL uses saved user_input 
                                                   // value and returns next substring
                }
                myargs[0][strlen(myargs[0])-1]=0;//remove the last \n character
                myargs[i+1]=NULL;
                if(checkBuiltIn(myargs[0])==1){
                        
                        //executeBuiltIn(myargs); TODO: IMPLEMENT IT
                }
                else{
                        newProcess(i,myargs);
                     }
               
                //for(int m=0;m<i;m++){ //DEBUGGING PURPOSES. 
                       // printf("%s\n",myargs[m]);
                //}    
        }while(strcasecmp(line,"exit\n")!=0); //\n is used for correct functioning of strcasecmp.
}



int main(int argc, char* argv[]){
        loop_wish();
        return 0;
}
