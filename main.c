#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

#define SH_LINE_SIZE 1024
char *sh_read_line(){
    int buffer_size = SH_LINE_SIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char)*buffer_size);
    if(!buffer){
        fprintf(stderr,"Shell: [-] can not allocate memory\n");
        exit(EXIT_FAILURE);
    }
    int c;


    while (true)
    {
        c = getchar();

        if(c==EOF || c=='\0'){
            buffer[position] = '\0';
            return buffer;
        }
        else{
            buffer[position] = c;
        }
        position++;
        if(position>=buffer_size){
            buffer_size += SH_LINE_SIZE;
            buffer = realloc(buffer,buffer_size);
            if(!buffer){
                fprintf(stderr, "Shell: [-] can not allocate memmory\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    
}


#define SH_TOKEN_SIZE 64
#define SH_TOKEN_DELIMITER "\t\r\n\a"
char **sh_pars_line(char *line){
    int buffer_size = SH_TOKEN_SIZE;
    int position = 0;
    char **tokens = malloc(buffer_size * sizeof(char*));
    if (!tokens) {
        fprintf(stderr, "Shell: [-] can not allocate memmory\n");
        exit(EXIT_FAILURE);
        }
    char **token;
    token = strtok(line,SH_TOKEN_DELIMITER);
    while(token!=NULL){
        tokens[position] = token;
        position++;
        if(position>=buffer_size){
            buffer_size+=SH_TOKEN_SIZE;
            tokens = realloc(tokens,buffer_size*sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "Shell: [-] can not allocate memmory\n");
                exit(EXIT_FAILURE);
            }
        }
        token=strtok(NULL,SH_TOKEN_DELIMITER);
    }
    tokens[position] = NULL;
    return tokens;
}

int sh_launch(char **args){
    pid_t pid,wpid;
    int status;

    pid = fork();
    if(pid==0){
        // child process
        if(execvp(arg[0],args)==-1){ // v: vector arguments, p: program name 
            perror("sh");
        }
        exit(EXIT_FAILURE);
    }
    else if(pid<0){
        perror("sh");
    }
    else{
        do{
            wpid = waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status)&&!WIFSIGNALED(status));
    }
    return 1;
}



void sh_loop(){
    char *line;
    char **args;
    int status;

    do
    {
        printf(">");
        line = sh_read_line();
        args = sh_pars_line(line);
        status = sh_execute(args);

        free(line);
        free(args);
                
    } while (status);
    
}

/*
  Builtin function
*/
char *builtin_str[] = {"cd","help","exit"};
int sh_cd(char **args);
int sh_help(char **args);
int sh_exit(char **args);
int (*builtin_func[]) (char **) = {
  &sh_cd,
  &sh_help,
  &sh_exit
};
int sh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}
int sh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "Shell: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("sh");
    }
  }
  return 1;
}
int sh_help(char **args)
{
  int i;
  printf("rhjr's Shell\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}
int sh_exit(char **args)
{
  return 0;
}


int sh_execute(char **args){
    int i;

    if(args[0]==NULL){
        return 1;
    }

    for(i=0;i<sh_num_builtins;i++){
        if(strcmp(args[0],builtin_str[i])==0){
            return (*builtin_func[i])(args);
        }
    }

    return sh_launch(args);
}


int main(int argc, char const *argv[])
{

    sh_loop();



    return EXIT_SUCCESS;
}
