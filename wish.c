#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>

int PATHCHANGED=0;
char *path;
int REDIRECTION=0;

void printError(){
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
}

int checkBuiltIn(char *arg){
   int flag=0;
   if(strcmp("exit",arg)==0)
        flag=1;
   else if(strcmp("cd",arg)==0)
        flag=2; 
   else if(strcmp("path",arg)==0)
        flag=3;
   return flag; 
        
}

int checkRedirection(char *arg){
        char *position= strchr(arg,'>');
        if(position==NULL)
                return 0;
        else 
                return 1;
}



//TODO: IMPLEMENT REDIRECTION
//TODO: IMPLEMENT PARALLEL COMMANDS 



int newProcess(int myargc,char *myargv[]){ 
        int rc=fork();
        if(rc<0){ //Fork Error
                printError();
                exit(1); 
        }
        else if(rc==0){ //Child process
                if(PATHCHANGED==0)
                        path=strdup("/bin/");
                        
                if(REDIRECTION==1){
                        
                        int i=0;
                        char* myargs[sizeof(myargv[0])];
                        myargs[0]= strtok(myargv[0]," \n\t");
                        while(myargs[i]!=NULL){
                                i++;
                                myargs[i]=strtok(NULL," \n\t"); 
                        }
                        int x=0;
                        char* file[sizeof(myargv[1])];
                        file[0]= strtok(myargv[1]," \n\t");
                        while(file[x]!=NULL){
                                x++;
                                file[x]=strtok(NULL," \n\t"); 
                        }
                        int out=open(file[0],O_WRONLY|O_CREAT|O_TRUNC,0666);
                        int error=open(file[0],O_WRONLY|O_CREAT|O_TRUNC,0666);
                        dup2(out,STDOUT_FILENO);
                        dup2(out,STDERR_FILENO);
                        close(out);
                        if(out==-1 || error==-1)
                                printError();
                        if(myargc>2)
                                printError();
                       
                        if(x!=1){
                                printError();
                        }
                        myargs[i+1]=NULL;
                        file[x+1]=NULL;
                        
                        path=strcat(path,myargs[0]);
                        
                        
                        if(access(path,X_OK)!=0){//successfully accessed binary or not?
                                if(PATHCHANGED==0)
                                path="/usr/bin/";
                                path=strcat(path,myargs[0]);
                                if(access(path,X_OK)!=0){
                                        printError();
                                }               
                        }
                       
                        if(execv(path,myargs)==-1){//successfuly executed binary or not? 
                                        printError();
                                        exit(1);
                        }
                        file[0]='\0';
                        myargs[0]='\0';
                }
                else{
                        path=strcat(path,myargv[0]);
                        if(access(path,X_OK)!=0){//successfully accessed binary or not?
                                if(PATHCHANGED==0)
                                        path="/usr/bin/";
                                path=strcat(path,myargv[0]);
                                if(access(path,X_OK)!=0){
                                        printError();
                                }               
                        }
                        if(execv(path,myargv)==-1){//successfuly executed binary or not? 
                                 printError();
                                 exit(1);                        
                        }              
                        return 1; 
                }
        }
        else{
                rc=wait(NULL);
            }         
        return 0;
}       


void loop_wish(){
        char *line;
        size_t len=0;
        path=(char*)malloc(512);
        char* delim= (char*)malloc(10);
        do{
                delim=strdup(" \n\t");
                int i=0;
                REDIRECTION=0;
                printf("wish > ");
                getline(&line,&len,stdin); //reads the entire STDIN command
                char *myargs[sizeof(line)]; //Can't be more than the line's size.
                if(checkRedirection(line)==1){
                        delim= strdup(">\n\t");
                        REDIRECTION=1;
                }
                myargs[0]= strtok(line,delim); // first call returns pointer to first part of user_input
                                             // separated by delim
                while(myargs[i]!=NULL){
                        i++;
                        myargs[i]=strtok(NULL,delim); // every call with NULL uses saved user_input 
                                                   // value and returns next substring
                }
                if(i==0)
                        continue;
                myargs[i+1]=NULL;
                int builtin=checkBuiltIn(myargs[0]);
                if(builtin!=0 && REDIRECTION!=1){ //Built-In Execution.
                        if(builtin==1){//exit
                                if(i==1){
                                        exit(0);
                                }
                                else
                                        printError();
                        }
                        else if(builtin==2){//cd
                                if(i==2){
                                        if(chdir(myargs[1])!=0)
                                                printError();
                                }
                                else
                                        printError();
                        }
                        else if(builtin==3){
                                PATHCHANGED=1;
                                char *temp=(char*) malloc(512);
                                temp[0]=0;
                                for(int x=1;x<i;x++){
                                        temp=strcat(temp,myargs[x]);
                                }
                                path=temp;
                                free(temp);
                                myargs[0]='\0';
                        }
                 
                }
                else{
                        newProcess(i,myargs);                
                }
                //for(int m=0;m<i;m++){ //DEBUGGING PURPOSES. 
                       // printf("%s\n",myargs[m]);
                //}
                line[0]='\0';    
        }while(1); //\n is used for correct functioning of strcasecmp.
}



int main(int argc, char* argv[]){
        loop_wish();
        return 0;
}






