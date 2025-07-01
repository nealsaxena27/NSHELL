#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define NSH_RL_BUFSIZE 1024
#define NSH_TOK_BUFSIZE 64
#define NSH_TOK_DELIM " \t\n\r\a"

char* nsh_read_line(){
    int bufsize = NSH_RL_BUFSIZE, pos = 0, c;
    char* buffer = malloc(sizeof(char) * bufsize);

    if(!buffer){
        fprintf(stderr, "nsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        // read a character
        c = getchar();

        // hit EOF or \n, then end the string with \0 and return
        if(c == EOF || c == '\n'){
            buffer[pos] = '\0';
            return buffer;
        }
        else{
            buffer[pos] = c;
        }
        pos++;

        // if buffer exceeds, reallocate
        if(pos >= bufsize){
            bufsize += NSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if(!buffer){
                fprintf(stderr, "nsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char **nsh_parse(char *line, char* delim){
    int bufsize = NSH_TOK_BUFSIZE, pos = 0;
    char **tokens = malloc(sizeof(char *) * bufsize);
    char *token;

    if(!tokens){
        fprintf(stderr, "nsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, delim);

    while(token != NULL){
        // store the token in tokens
        tokens[pos] = token;
        pos++;

        // if buffer exceeds, reallocate
        if(pos >= bufsize){
            bufsize += NSH_TOK_BUFSIZE;
            tokens = realloc(tokens, sizeof(char *) * bufsize);
            if(!tokens){
                fprintf(stderr, "nsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, delim);
    }

    tokens[pos] = NULL;
    return tokens;
}

int nsh_launch(char **args){
    pid_t pid, wpid;
    int status;

    int in_fd = -1, out_fd = -1;
    for(int i = 0; args[i] != NULL; i++){
        if(strcmp(args[i], "<") == 0){
            // redirect stdin to given input file
            in_fd = open(args[i + 1], O_RDONLY);
            if(in_fd < 0){
                perror("nsh");
                return 1;
            }
            args[i] = NULL;
        }
        else if(strcmp(args[i], ">") == 0){
            // redirect stdout to given output file (truncate file if already exists)
            out_fd = open(args[i + 1], O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
            if(out_fd < 0){
                perror("nsh");
                return 1;
            }
            args[i] = NULL;
        }
        else if(strcmp(args[i], ">>") == 0){
            // redirect stdout to given output file (append to file if already exists)
            out_fd = open(args[i + 1], O_CREAT | O_WRONLY | O_APPEND, S_IRWXU);
            if(out_fd < 0){
                perror("nsh");
                return 1;
            }
            args[i] = NULL;
        }
    }

    pid = fork(); // create a child process
    if(pid < 0){
        perror("nsh");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        // child process
        if(in_fd != -1){
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        if(out_fd != -1){
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }
        if(execvp(args[0], args) == -1){
            perror("nsh");
        }
        exit(EXIT_FAILURE);
    }
    else{
        // parent process
        // wait for child to terminate or stop by signal
        do{
            wpid = waitpid(pid, &status, WUNTRACED);
        } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

// function declarations for builtin shell commands
int nsh_cd(char **args);
int nsh_help(char **args);
int nsh_exit(char **args);

// list of builtin commands, followed by their function pointers
char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &nsh_cd,
    &nsh_help,
    &nsh_exit
};

int nsh_num_builtins(){
    return sizeof(builtin_str) / sizeof(char *);
}

// builtin function implementations
int nsh_cd(char **args){
    if(args[1] == NULL){
        fprintf(stderr, "nsh: no argument received for \"cd\"\n");
    }
    else{
        if(chdir(args[1]) != 0){
            perror("nsh");
        }
    }
    return 1;
}

int nsh_help(char **args){
    printf("==============================================================\n");
    printf("\t\t\tNSHELL HELP\n");
    printf("==============================================================\n");
    printf("Following commands are builtin\n");
    int n = nsh_num_builtins();
    for(int i = 0; i < n; i++){
        printf("%s\n", builtin_str[i]);
    }
    printf("Use the man command for information on other programs.\n");
    return 1;
}

int nsh_exit(char **args){
    return 0; // status = 0 will make nsh_loop end
}

int nsh_execute(char **args){
    if(args[0] == NULL){
        // empty command entered
        return 1;
    }

    int n = nsh_num_builtins(); 
    for(int i = 0; i < n; i++){
        if(strcmp(args[0], builtin_str[i]) == 0){
            // call the matched builitin command
            return (*builtin_func[i])(args);
        }
    }
    // call nsh_launch to fork and exec as required
    return nsh_launch(args);
}

void nsh_loop(int interactive){
    if(interactive == 1){
        printf("==============================================================\n");
        printf(".__   __.      _______. __    __   _______  __       __      \n\
|  \\ |  |     /       ||  |  |  | |   ____||  |     |  |     \n\
|   \\|  |    |   (----`|  |__|  | |  |__   |  |     |  |     \n\
|  . `  |     \\   \\    |   __   | |   __|  |  |     |  |     \n\
|  |\\   | .----)   |   |  |  |  | |  |____ |  `----.|  `----.\n\
|__| \\__| |_______/    |__|  |__| |_______||_______||_______|\n");
        printf("\t\t\tWELCOME TO NSHELL\n");
        printf("==============================================================\n");
    }
    char *line;
    char **args;
    char **commands;
    int status;

    do{
        if(interactive == 1) printf("nsh> ");
        line = nsh_read_line(); // read
        // parse and seperate the commands chained by ";"
        commands = nsh_parse(line, ";");
        for(int i = 0; commands[i] != NULL; i++){
            // parse and seperate the args delimiting by white spaces
            args = nsh_parse(commands[i], " \n\t\r\a");
            status = nsh_execute(args); // execute
            free(args);
            if(status == 0) break;
        }
        free(commands);
        free(line);
    } while(status); // status = 0 will make nsh_loop end
}

int main(int argc, char **argv){
    int interactive = 1;
    if(argv[1] != NULL){
        // batch mode - input commands taken from the batch file provided in argv[1]
        int in_fd = open(argv[1], O_RDONLY);
        if(in_fd < 0){
            perror("nsh");
            exit(EXIT_FAILURE);
        } 
        dup2(in_fd, STDIN_FILENO);
        close(in_fd);
        interactive = 0;
    }
    nsh_loop(interactive); // run command loop
    exit(EXIT_SUCCESS);
}