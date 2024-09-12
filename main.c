#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 256

char *path[20] = {"/bin", NULL}; // Initial path containing /bin

void errorMessage() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
    fflush(stdout);
}

void parse_command_to_args(char *line, char **args) {
    char *temp;
    int i = 0;
    while ((temp = strsep(&line, " \t\n")) != NULL) { // seperate at element
        if (*temp != '\0') {
            args[i] = temp;
            i++;
        }
    }
    args[i] = NULL; // Null terminate
}

int execute_command(char **args) {
    pid_t pid;
    int i = 0;
    int j = 0;
    int track = 0;
    int redirection = 0;
    char command_path[MAX_LINE];
    char *output_file = NULL;
    int output_fd;

    while (args[j] != NULL) {
        if (strcmp(args[j], ">") == 0) {
            if (redirection == 0) {
                if (args[j + 1] == NULL){
                    errorMessage();
                    return 0;
                }
                output_file = args[j + 1];
                args[j + 1] == NULL; // Remove output file from arguments
                args[j] = NULL;      // Remove > from arguments
                redirection++;
                if (args[j + 2] != NULL) { // Argument after redirection
                    errorMessage();
                    return 0;
                }
            } 
            /*else { // Redirection overloaded
                errorMessage();
                return 0;
            }*/
        }
        j++;
    }

    while (path[i] != NULL) { // check each path
        strcpy(command_path, path[i]);
        strcat(command_path, "/");
        strcat(command_path, args[0]);

        if (redirection == 0) { // no redirection
            if (access(command_path, X_OK) == 0) { // command exists
                track++;
                pid = fork();
                if (pid == -1) {
                    errorMessage();
                    return 0;
                } 
                else if (pid == 0) { // Child process
                    execv(command_path, args);
                }
            }
        } 
        else if (redirection == 1) { // redirection
            if (access(command_path, X_OK) == 0) { // command exists
                track++;
                pid = fork();
                if (pid == -1) {
                    errorMessage();
                    return 0;
                } 
                else if (pid == 0) { // Child
                    int fd = open(output_file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                    dup2(fd, 1); // opens file to write to
                    close(fd);

                    execv(command_path, args);
                }
            }
        } 
        else { // redirection overloaded
            errorMessage();
            return 0;
        }
        i++;
    }

    if (track == 0)
        errorMessage(); // command doesnt exist
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        errorMessage();
        exit(0);
    }

    char *line = NULL;
    size_t buf_size = 256;
    char *args[20];

    while (1) {
        printf("rush> ");
        fflush(stdout);

        getline(&line, &buf_size, stdin);

        line[strcspn(line, "\n")] = '\0'; // Removes newline character

        char *commands[20];
        char *input_element;
        int i = 0;

        // seperates input by &, so each element in commands will store 1 command with arguments
        while ((input_element = strsep(&line, "&")) != NULL) {
            if (*input_element != '\0') {
                commands[i] = input_element;
                i++;
            }
        }
        commands[i] = NULL; // Null terminate

        for (int j = 0; commands[j] != NULL; j++) { // for every command
            parse_command_to_args(commands[j], args); // parses arguments and saves to args

            if (args[0] == NULL) { // no input
                continue;
            }

            if (strcmp(args[0], "exit") == 0) { // exit
                if (args[1] != NULL) { // arguments were provided
                    errorMessage();
                    continue;
                }
                exit(0);
            }

            if (strcmp(args[0], "path") == 0) { // path
                for (int k = 0; path[k] != NULL; k++) { // Clear path
                    path[k] = NULL;
                }

                int k;
                for (k = 1; args[k] != NULL; k++) { // args[0] stores "path", so skip and add arguments to path
                    path[k - 1] = args[k];
                    k++;
                }
                path[k] = NULL; // Null terminate
                continue;
            }

            if (strcmp(args[0], "cd") == 0) { // cd
                if (args[0] == NULL) { // 0 arguments
                    errorMessage();
                } 
                else if (args[2] != NULL) { // >1 arguments
                    errorMessage();
                } 
                else {
                    if (chdir(args[1]) != 0)
                        errorMessage();
                }
                continue;
            }

            execute_command(args); // not a built in command
        }
        for (int j = 0; commands[j] != NULL; j++) {
            wait(NULL);
        }
        fflush(stdout);
    }

    free(line);
    return 0;
}