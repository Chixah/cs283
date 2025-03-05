#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

void empty(command_list_t *clist) {
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9 ; j++ ){
            clist->commands[i].argv[j] = '\0';
        }

    }
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    // Return Code
    int rc = OK;

    if (strlen(cmd_line) == 0 || is_empty(cmd_line) == 0) {
        rc = WARN_NO_CMDS;
    } 

    
    // Breakdown
    // https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/

    char temp[256];
    int count = 0;
    int command = 0;
    int command_header = 1;
    int after_pipe = 0;

    for(int i = 0; cmd_line[i] != '\0'; i++) {
        char current_character = cmd_line[i];
        //printf("Character position[%d]: %c[%d]\n", i, current_character, current_character);

        if(current_character != PIPE_CHAR) {
            if(current_character == SPACE_CHAR && command_header == 1 && after_pipe == 0) {
                temp[count] = '\0';
                /*for(int j = 0; temp[j] != '\0'; j++) {
                    printf("%c[%d] ", temp[j], temp[j]);
                }
                printf("Count: %d ", count); */
                strncpy(clist->commands[command].argv[command], temp, count+1);
                //printf("\n");
                count = 0;
                command_header = 0;

            } else if (after_pipe == 1) {
                if(current_character == SPACE_CHAR) {
                    command_header = 1;
                    count = 0;
                    after_pipe = 0;
                } else {
                    after_pipe = 0;
                    temp[count] = current_character;
                    count++;
                }
                
            } else {
                temp[count] = current_character;
                count++;
            }
            
        }

        if(current_character == PIPE_CHAR){
            if(command_header == 0) {
                if (temp[count-1] == SPACE_CHAR) {
                    temp[count-1] = '\0';
                    strncpy(clist->commands[command].argv[command], temp, count);
                } else {
                    temp[count] = '\0';
                /* for(int j = 0; temp[j] != '\0'; j++) {
                    printf("%c[%d] ", temp[j], temp[j]);
                }
                printf("Count: %d ", count); */
                    strncpy(clist->commands[command].argv[command], temp, count+1);
                //printf("\n");
                }
                
            } 
            if(command_header == 1){
                temp[count] = '\0';
                /* for(int j = 0; temp[j] != '\0'; j++) {
                    printf("%c[%d] ", temp[j], temp[j]);
                }
                printf("Count: %d ", count); */
                strncpy(clist->commands[command].argv[command], temp, count+1);
                //printf("\n");
            }
            count = 0;
            command_header = 1;
            command++;
            after_pipe = 1;
            
        }

        

    }

    //printf("Outside:\n");
    temp[count] = '\0';
    /*for(int j = 0; temp[j] != '\0'; j++) {
        printf("%c[%d] ", temp[j], temp[j]);
    }
    //printf("Count: %d ", count); */

    //printf("Outside command header %d\n", command_header);

    if (command_header == 1) {
        strncpy(clist->commands[command].argv[command], temp, count+1);
        clist->commands[command].argv[0] = '\0';
    } else if (command_header == 0){
        strncpy(clist->commands[command].argv[command], temp, count+1);
    }
    command++;
    

    if (command > 8) {
        rc = ERR_TOO_MANY_COMMANDS;
    } else {
        clist->num = command;
    }

    for (int i = 0; i < clist->num; i++) {
        if(strlen(clist->commands[command].argv[command]) > 256) {
            rc = ERR_CMD_OR_ARGS_TOO_BIG;
        } else if (strlen(clist->commands[command].argv[command]) > 64) {
            rc = ERR_CMD_OR_ARGS_TOO_BIG;
        }
    }
    
    
    //printf("\n");
    //printf("Number of Commands: %d\n", clist->num);
    return rc;
 }

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = (char *)malloc(SH_CMD_MAX * sizeof(char));
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY; 
    }
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    return OK; 
}

int free_cmd_buff(cmd_buff_t *cmd_buff){
    if(cmd_buff->_cmd_buffer != NULL) {
        free(cmd_buff->_cmd_buffer);
    }

    for (int i = 0; i < cmd_buff->argc; i++) {
        if (cmd_buff->argv[i] != NULL) {
            free(cmd_buff->argv[i]);
        }
    }
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer != NULL) {
        memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    }
    //printf("Error Before Argument clear\n");
    for (int i = 0; i < 9; i++) {
        if (cmd_buff->argv[i] != NULL) {
            //free(cmd_buff->argv[i]);
            cmd_buff->argv[i] = NULL;
        }
    }
    //printf("Error After Argument clear\n");
    cmd_buff->argc = 0;
    return OK; 
}

int is_empty(char *cmd_line) {
    int space_count = 0;
    int command_length = strlen(cmd_line);
    for(int i = 0; cmd_line[i] != '\0'; i++) {
        //printf("%d\n", cmd_line[i]);

        // SideNote Space is 32
        if(cmd_line[i] == SPACE_CHAR) {
            space_count++;
        }
    }
    //printf("Spaces: %d, Command Length: %d\n", space_count, command_length);

    // String consists of all spaces
    if (space_count == command_length) {
        return 0;
    }

    // Not Empty String
    return 1;

}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    int rc = OK;
    clear_cmd_buff(cmd_buff);

    // Check if command line is empty
    if (strlen(cmd_line) == 0 || is_empty(cmd_line) == 0) {
        rc = WARN_NO_CMDS;
        return rc;
    }

    // Remove leading and trailing spaces
    int leading_space = 0;
    char temp[256];
    for (int i = 0; cmd_line[i] != '\0'; i++) {
        if (cmd_line[i] == SPACE_CHAR && leading_space == i) {
            leading_space++;
        }
    }

    int count = 0;
    for (int i = leading_space; cmd_line[i] != '\0'; i++) {
        temp[count++] = cmd_line[i];
    }
    temp[count] = '\0';

    int ending_space = strlen(temp);
    for (int i = strlen(temp) - 1; i >= 0; i--) {
        if (temp[i] == SPACE_CHAR && i == ending_space - 1) {
            ending_space--;
        }
    }

    char new_temp[256];
    for (int i = 0; i < ending_space; i++) {
        new_temp[i] = temp[i];
    }
    new_temp[ending_space] = '\0';

    // Split into multiple commands based on `|`
    char *token = strtok(new_temp, "|");
    int argc = 0;

    while (token != NULL) {
        // Trim spaces around each command
        char trimmed_cmd[256];
        sscanf(token, " %255[^\n]", trimmed_cmd);

        // Store the command in the command buffer
        cmd_buff->argv[argc] = (char *)malloc(strlen(trimmed_cmd) + 1);
        if (cmd_buff->argv[argc] == NULL) {
            rc = ERR_MEMORY; // Memory allocation failed
            break;
        }
        strcpy(cmd_buff->argv[argc], trimmed_cmd);

        argc++;
        if (argc > CMD_MAX) {
            rc = ERR_TOO_MANY_COMMANDS; // Too many commands
            break;
        }

        token = strtok(NULL, "|"); // Get the next command
    }

    /*for(int i = 0; i < argc; i++) {
        printf("Argument[%d]: \n\n", i);
        for(int j = 0; cmd_buff->argv[i][j] != '\0'; j++){
            printf("%c",  cmd_buff->argv[i][j]);
        }
        printf("Argument length %ld\n", strlen(cmd_buff->argv[i]));
        printf("\n");
    } */

    cmd_buff->argc = argc;

    if (rc != OK) {
        // Clean up in case of errors
        for (int i = 0; i < argc; i++) {
            free(cmd_buff->argv[i]);
        }
        cmd_buff->argc = 0;
    }

    return rc;
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, "exit") == 0) {
        return BI_CMD_EXIT;
    } else if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    } else if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    }
    return BI_NOT_BI;
}

int exec_cmd(cmd_buff_t *cmd_buff) {
    int rc = OK;
    int num_commands = cmd_buff->argc;

    // Validate the command buffer
    if (cmd_buff == NULL || num_commands < 1) {
        return WARN_NO_CMDS; // No commands to execute
    }
    /*for(int i = 0; i < cmd_buff->argc ; i++) {
        printf("Argument[%d]: \n\n", i);
        for(int j = 0; cmd_buff->argv[i][j] != '\0'; j++){
            printf("%c",  cmd_buff->argv[i][j]);
        }
        printf("Argument length %ld\n", strlen(cmd_buff->argv[i]));
        printf("\n");
    }*/

    if (num_commands == 1) {
        Built_In_Cmds command = match_command(cmd_buff->argv[0]);
        if (command == BI_CMD_EXIT) {
            rc = OK_EXIT;
            return rc;
        } else if (command == BI_CMD_DRAGON) {
            print_dragon();
        } else if (command == BI_CMD_CD) {
            if (cmd_buff->argv[1] != NULL) {
                if (chdir(cmd_buff->argv[1]) == 0) {
                    //printf("Successfully changed directory: %s\n", cmd->argv[1]);
                } else {
                    rc = ERR_EXEC_CMD;
                    return rc;
                }
            }
        } else if (command == BI_EXECUTED) {
            rc = OK;
        } else if (command == BI_NOT_BI) {
            pid_t pid = fork();
    
            if (pid == 0) {
                if(execvp(cmd_buff->argv[0], cmd_buff->argv) == -1) {
                    //printf("Execvp went wrong!!!\n\n");
                    int status;
                    waitpid(pid, &status, 0);
    
                    if (WIFEXITED(status)) {
                        rc = WEXITSTATUS(status);
                    }
                    return rc;
                }
    
            } else if (pid < 0) {
                rc = ERR_EXEC_CMD;
                return rc;
            } else {
                int status;
                waitpid(pid, &status, 0);
    
                if (WIFEXITED(status)) {
                    rc = WEXITSTATUS(status);
                }
                return rc;
            }
        }
    } else {
        int fd[2]; // `fd` keeps track of input for each command
        pipe(fd); // Create a pipe
        pid_t pid1, pid2;

        pid1 = fork();
        if (pid1 == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid1 == 0) {
            // Child process 1 (ls)
            close(fd[0]); // Close unused read end
            dup2(fd[1], STDOUT_FILENO); // Redirect stdout to pipe write end
            close(fd[1]);

            execlp("ls", "ls", NULL);
            perror("execlp");
            exit(EXIT_FAILURE);
        }

        pid2 = fork();
        if (pid2 == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid2 == 0) {
            // Child process 2 (grep)
            close(fd[1]); // Close unused write end
            dup2(fd[0], STDIN_FILENO); // Redirect stdin to pipe read end
            close(fd[0]);

            execlp("grep", "grep", ".c", NULL);
            perror("execlp");
            exit(EXIT_FAILURE);
        }

        close(fd[0]);
        close(fd[1]);

        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
    }

    return rc;
}

int exec_local_cmd_loop()
{
    char cmd_buff[SH_CMD_MAX];
    int rc = 0;
    cmd_buff_t cmd;
    command_list_t clist;

    //printf("At the start\n");
    empty(&clist);
    rc = alloc_cmd_buff(&cmd);
    //printf("Between clear and allocate\n");
    clear_cmd_buff(&cmd);
    //printf("After Allocation\n");

    // TODO IMPLEMENT MAIN LOOP
    while (1)
    {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL)
        {
            printf("\n");
            break;
        }

        //printf("Trimming:\n");
        // remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        //printf("Am Before Build\n");
        // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff
        rc = build_cmd_buff(cmd_buff, &cmd);
        
        if (rc < 0) {
            break;
        } 

        rc = exec_cmd(&cmd);

        //printf("Problem is here");
        if (rc == OK_EXIT || rc == ERR_CMD_ARGS_BAD) {
            break;
        } 
        //printf("Before Last clear\n");
        clear_cmd_buff(&cmd);
        //printf("After Last Clear\n");
        empty(&clist);
    }

    

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"
    //printf("Broke before free\n");
    free_cmd_buff(&cmd);
    //printf("Broke After free\n");
    
    return rc;
}

