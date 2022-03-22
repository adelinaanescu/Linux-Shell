#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>
#include<readline/readline.h>
#include<readline/history.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

void strip(char* str) {
    int after = 0;
    for (int i = strlen(str) - 1; i >= 0; i--)
        if (str[i] == ' ')
            after++;
        else
            break;
    str[strlen(str) - after] = '\0';

    after = 0;
    for (int i = 0; i < strlen(str); i++)
        if (str[i] == ' ')
            after++;
        else
            break;

    char *str1 = strdup(str);
    strcpy(str, str1 + after);


}

int  readCommand(char command[100]){
    char path[1024];
    if(getcwd(path, sizeof(path))<0){
        return -1;
    }
    printf("\n");
    printf("%s ", path);
    char* buffer;
    buffer=readline(">");
    if(strlen(buffer)>0){
        strcpy(command,buffer);
        add_history(command);
        return 1;
    }
    return -1;
}

int  parseCommand(char command[100],char** parsedcommand){
    char* p;
    int i=0;
    p=strtok(command," ");
    while(p){
        strip(p);
        parsedcommand[i]=p;
        p=strtok(NULL," ");
        i++;
    }
   parsedcommand[i]=NULL;
    return i;

}

int  help_command(){
    FILE *fptr;
    fptr = fopen("help.txt", "r");
    if (fptr == NULL){
        return 0;
    }
    char content = fgetc(fptr);
    while (content != EOF){
        printf ("%c", content);
        content= fgetc(fptr);
    }
    fclose(fptr);
    return 1;
}

int wc_command(char** argv, int argc){
    char** files=malloc(100 * sizeof(char *));
    int number_of_files=0;
    for(int i=1;i<argc;i++)
    {
        if(argv[i][0]!='-') {
            files[number_of_files] = argv[i];
            number_of_files++;
        }

    }
   // printf("Number of files: %d\n",number_of_files);
    int totallines = 0, totalwords = 0, totalbytes = 0, totalmaxLineLength = 0;
    for(int i=0;i<number_of_files;i++) {
        int lines = 0, words = 1, bytes = 1, maxLineLength = 0, lineLength = 0;
        FILE *fptr;
        fptr = fopen(files[i], "r");
        if (fptr == NULL) {
            return -1;
        }

        char content1 = fgetc(fptr);
        char content = content;
        content = fgetc(fptr);
        while (content != EOF) {
            bytes++;
            if (content != '\n')
                lineLength++;
            if (maxLineLength < lineLength)
                maxLineLength = lineLength;
            if (content == '\n') {
                lines++;
                lineLength = 0;
            }
            if ((content != '\n' && content != ' ' && content != '\t') &&
                (content1 == '\n' || content1 == ' ' || content1 == '\t'))
                words++;

            content1 = content;
            content = fgetc(fptr);
        }

        fclose(fptr);
       // printf("Test output: %d %d %d %s\n", lines, words, bytes, files[i]);
        char **param = malloc(100 * sizeof(char *) + 1);
        int number_of_param = 0;
        for (int j = 1; j < argc; j++) {
            if (argv[j][0] == '-') {
                param[number_of_param] = argv[j];
                number_of_param++;
            }
        }

        if (number_of_param == 0)
            printf("%d %d %d %s\n", lines, words, bytes, files[i]);
        else{
          //  printf("We have parameters\n");
            for (int j = 0; j < number_of_param; j++) {
                if (param[j][1] == 'c')
                    printf("%d ", bytes);
                if (param[j][1] == 'w')
                    printf("%d ", words);
                if (param[j][1] == 'l')
                    printf("%d ", lines);
                if (param[j][1] == 'L')
                    printf("%d ", maxLineLength);
                if(strchr("cwlL",param[i][1])==0)
                {
                    return -1;
                }
            }
            printf("%s\n",files[i]);
        }

        totalbytes=totalbytes+bytes;
        totallines=lines+totallines;
        totalwords=totalwords+ words;
        if(totalmaxLineLength<maxLineLength)
            totalmaxLineLength=maxLineLength;
        free(param);
    }

    if(number_of_files>1){
        char **param = malloc(100 * sizeof(char *) + 1);
        int number_of_param = 0;
        for (int j = 1; j < argc; j++) {
            if (argv[j][0] == '-') {
                param[number_of_param] = argv[j];
                number_of_param++;
            }
        }
        if (number_of_param == 0)
            printf("%d %d %d total\n", totallines, totalwords, totalbytes);
        else{
            for (int j = 0; j < number_of_param; j++) {
             //   printf("Params[1]: %c \n", param[j][1]);
                if (param[j][1] == 'c')
                    printf("%d ", totalbytes);
                if (param[j][1] == 'w')
                    printf("%d ", totalwords);
                if (param[j][1] == 'l')
                    printf("%d ", totallines);
                if (param[j][1] == 'L')
                    printf("%d ", totalmaxLineLength);
            }
                printf("total\n");
        }
        free(param);
    }
    free(files);

    return 1;
}

int expand_command(char** argv, int argc) {
    char **files = malloc(100 * sizeof(char *));
    int number_of_files = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            files[number_of_files] = argv[i];
            number_of_files++;
        }

    }
   // printf("Number of files: %d\n", number_of_files);
    for (int i = 0; i < number_of_files; i++) {
        FILE *fptr;
        fptr = fopen(files[i], "r");
        if (fptr == NULL) {
            return 0;
        }
        char content = fgetc(fptr);
        char content1 = content;

        char **param = malloc(10 * sizeof(char *) + 1);
        int number_of_param = 0;
        for (int j = 1; j < argc; j++) {
            if (argv[j][0] == '-') {
                param[number_of_param] = argv[j];
                number_of_param++;
            }
        }
          //  printf("Number of params: %d\n", number_of_param);

            if (number_of_param == 0) //simple case /t -> 8 spaces
            {
                while (content != EOF) {
                    if (content != '\t')
                        printf("%c", content);
                    else {
                        printf("        ");
                    }
                    content = fgetc(fptr);
                }
            } else if (number_of_param == 1) {
                    if (param[0][1] == 'i')
                    {
                        while (content != EOF) {
                            if(content !='\t')
                                printf("%c", content);
                            else if (content == '\t' && (content1 != ' ' && content1 != '\n'))
                                printf("%c", content);
                            else {
                                printf("        ");
                            }
                            content1 = content;
                            content = fgetc(fptr);
                        }
                    } else if (param[0][1] == 't') {
                        int N = ((int) (param[0][2])) - ((int) ('0'));
                        while (content != EOF) {
                            if (content != '\t')
                                printf("%c", content);
                            else {
                                for (int j = 0; j < N; j++)
                                    printf(" ");
                            }
                            content = fgetc(fptr);
                        }

                    } else {
                        printf("Invalid Parameter\n");
                        return 0;
                    }
                } else if (number_of_param == 2) {
                    int N = ((int) (param[0][2]))-((int)('0'));//maybe -tN is the second one, does not work like that
                    while (content != EOF) {
                        if(content != '\t')
                            printf("%c", content);
                        else if (content == '\t' && (content1 != ' ' && content1 != '\n'))
                            printf("%c", content);
                        else {
                            for (int j = 0; j < N; j++)
                                printf(" ");
                        }
                        content1 = content;
                        content = fgetc(fptr);
                    }

                } else { return -1; }

        fclose(fptr);

    }
    return 0;
}

int env_command(char** argv, int argc, char* envp[]) {

    if(argc%2==0){
        printf("Detrimental usage, try: env [OPTION]... [-] [NAME=VALUE]... [COMMAND [ARG]...]\n");
        return -1;
    }

    if(argc==1)
    {
    for (int i = 0; envp[i] != NULL; i++)
        printf("\n%s", envp[i]);
    getchar();
    }
    else if(argc==3){

        int cnt=1;
        for (int i = 0; envp[i] != NULL; i++)
            if (strstr(envp[i], argv[2]) != 0)
                cnt=0;

        if(cnt==1){
            printf("Env variable: %s does not exist\n",argv[2]);
            return 0;
        }

        strcat(argv[2],"=");
        for (int i = 0; envp[i] != NULL; i++) {
            if (strstr(envp[i], argv[2]) == 0) {
                printf("\n%s", envp[i]);

            }
        }
        getchar();



    } else if(argc == 5) {


        strcat(argv[2],"=");
        strcat(argv[4],"=");
        for (int i = 0; envp[i] != NULL; i++) {
            if (strstr(envp[i], argv[2]) == 0 && strstr(envp[i], argv[4]) == 0) {
                printf("\n%s", envp[i]);

            }

        }
        getchar();



    }

    return 1;

}

int  executeBuildinCommand(char **argv,  int argc,char* envp[])
{
        if (strcmp(argv[0], "exit") == 0) {
            exit(0);
        }
        if (strcmp(argv[0], "help") == 0) {
            help_command();
            return 1;
        }
        if (strcmp(argv[0], "cd") == 0) { //how do you go back after cd?
            chdir(argv[1]);
            return 1;
        }

        if (strcmp(argv[0], "wc") == 0) {
          //  printf("wc_command\n");
            wc_command(argv, argc);
            return 1;
        }
        if (strcmp(argv[0], "expand") == 0) {
         //   printf("expand_command\n");
            expand_command(argv, argc);
            return 1;
        }
        if (strcmp(argv[0], "env") == 0) {
          //  printf("env_command\n");
            env_command(argv, argc, envp);
            return 1;

        }


    return 0;
}

int executeCommand(char** parsedcommand){

    //printf("exec_command\n");
    pid_t pid = fork();
    if (pid==-1) {
        return -1;
    } else if (pid == 0) {
        int err= execvp(parsedcommand[0], parsedcommand);
        if(err==-1)
            return -1;
        return 0;
    } else {
        wait(NULL);
        return 1;
    }

}

void inChildPipeCommand(char* command){

    char *temp,*p;
    temp=strdup(command);
    p=strtok(temp," ");
    int i=0;
    while(p){
        i++;
        p=strtok(NULL," ");
    }
    char** argv=malloc(sizeof (char*)*i+1);
    parseCommand(command,argv);
    pid_t pid=fork();
    if(pid==0){
        int error=execvp(argv[0],argv);
        if(error==-1)
            exit(0);
    }else{
        while(wait(NULL)!=pid);
    }
}

int executeMultiPipeCommand(char** parsedpipecommand){

    int fd[2];
    int i=0;
    int fd3=0;

    while(parsedpipecommand[i] != NULL)
    {

        if(pipe(fd)==-1){
            return -1;
        }
        pid_t pid=fork();
        if(pid<0){
            return -1;
        }
        if(pid==0){

            dup2(fd3,0);
            if(parsedpipecommand[i+1]){
                dup2(fd[1],1);
            }
            close(fd[0]);
            inChildPipeCommand(parsedpipecommand[i]);
            exit(0);

        }else{
            waitpid(pid,NULL,0);
            close(fd[1]);
            fd3=fd[0];
            i=i+1;
        }


    }

    return 1;

}

int parsePipeCommand(char* command, char** pipecommand)
{
    char* command1=strdup(command);
    char* p =strtok(command1,"|");
    int i=0;
    while(p){
        strip(p);
        pipecommand[i]=strdup(p);
        p=strtok(NULL,"|");
        i++;

    }
    pipecommand[i]=NULL;

//    for(int j=0;pipecommand[j]!=NULL;j++){
//        printf("%s \n",pipecommand[j]);
//    }

    if (pipecommand[1] == NULL)
        return 0;
    else {
        return 1;
    }
}

int run_command( char command[100],char* envp[]){


    char **pipecommand=malloc(strlen(command) * sizeof(char *)+1);
    char **parsedcommand = malloc(strlen(command) * sizeof(char *)+1);


    if (parsePipeCommand(command, pipecommand) == 1) {
            if(executeMultiPipeCommand(pipecommand)==-1){
                printf("Error at executing pipe command");
                return -1;
            }
    }

    else {

    int command_argc=parseCommand(command, parsedcommand);

    if (executeBuildinCommand(parsedcommand, command_argc,envp) == 0) {

            if (executeCommand(parsedcommand) == -1){
                     printf("Error at executing command");
                     return -1;
            }
    }

    }

    free(parsedcommand);
    free(pipecommand);

    return 1;

}

int parseRedirectCommand(char* initialcommand, char** command)
{

    for(int i=0;i<2;i++){
        command[i]= strsep(&initialcommand,">");
    }

    if (command[1] == NULL)
        return 0;
    else {
        return 1;
    }

}

void run_shell(char* envp[]) {

    char initialcommand[100];
    readCommand(initialcommand);
    char *parsedredirectcommand[2];

    if (parseRedirectCommand(initialcommand, parsedredirectcommand) == 1) {

        //redirect output to parsedredirectcommand[1] -> filename
        int initial_fd=dup(STDOUT_FILENO);
        int fd1 = open(parsedredirectcommand[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd1 < 0)
            exit(0);
        dup2(fd1,1);

        //code that will be printed in filename
        run_command(parsedredirectcommand[0],envp);

        //redirect output back to terminal -> standard output descriptor
        close(fd1);
        dup2(initial_fd,1);
    }
    else{

        //without redirect
        run_command(parsedredirectcommand[0],envp);
    }

}

int main(int argc, char *argv[], char * envp[]) {

    for(;;){
    run_shell(envp);
    }

}


