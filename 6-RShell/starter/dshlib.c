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

    

    cmd_buff->argc = argc;
    for(int i = 0; i < argc - 1; i++) {
        int length = strlen(cmd_buff->argv[i]);
        cmd_buff->argv[i][length-1] = '\0';
    }

    /*for(int i = 0; i < argc; i++) {
        printf("Argument[%d]: \n\n", i);
        for(int j = 0; cmd_buff->argv[i][j] != '\0'; j++){
            printf("%c",  cmd_buff->argv[i][j]);
        }
        printf("Argument length %ld\n", strlen(cmd_buff->argv[i]));
        printf("\n");
    } */

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
            rc = OK;
        }
    }
    return rc;
}

int build_cmd_list(cmd_buff_t *cmd_buff, command_list_t *clist) {
    if (!cmd_buff || !clist) {
        return -1; // Invalid input
    }

    clist->num = cmd_buff->argc;
    //clist->commands[0].argv[0] = (char *)malloc(ARG_MAX);
    //strcpy(clist->commands[0].argv[0], cmd_buff->argv[0]);

    for (int i = 0; i < cmd_buff->argc; i++) {

        char *token = strtok(cmd_buff->argv[i], " ");
        int j = 0;
    
        while (token != NULL) {
            // Allocate memory for each argument
            clist->commands[i].argv[j] = (char *)malloc(strlen(token) + 1);
            //printf("Token[%s]\n", token);
    
            strcpy(clist->commands[i].argv[j], token);
    
            j++;
    
            token = strtok(NULL, " ");
        }
        clist->commands[i].argv[j] = NULL;
        clist->commands[i].argc = j;
        
    }
    
    //printf("Command[0]Arg[0]: %s", clist->commands[0].argv[0]);
    //printf("Command[1]Arg[0]: %s", clist->commands[1].argv[0]);
    //printf("Command[1]Arg[1]: %s", clist->commands[1].argv[1]);

    /*for (int i = 0; i < clist->num; i++) {
        printf("Command[%d]: ", i);
        for (int j = 0; j < clist->commands[i].argc; j++) {
            printf("%s\n", clist->commands[i].argv[j]); 
        }
        printf("\n"); 
    } */

    for (int i = 0; i < clist->num; i++) {
        if (strcmp(clist->commands[i].argv[0], "grep") == 0) {
            for (int j = 1; clist->commands[i].argv[j] != NULL; j++) {
                char *arg = clist->commands[i].argv[j];
                size_t len = strlen(arg);
    
                if (arg[0] == '"' && arg[len - 1] == '"') {
                    for (size_t k = 0; k < len - 2; k++) {
                        arg[k] = arg[k + 1];
                    }
                    arg[len - 2] = '\0'; 
                }
            }
        }
    }

    //printf("Clist number is %d, Cmd_buff Number is %d \n", clist->num, cmd_buff->argc);

    

    return 0; // Success
}

int free_cmd_list(command_list_t *clist) {
    
    return 0; // Success
}

int execute_pipeline(command_list_t *clist) {
    int num_commands = clist->num;
    //printf("Number of commands %d\n", num_commands);
    
    int pipes[num_commands - 1][2];
    pid_t pids[num_commands];

    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_commands; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) {  // Child process
            // Set up input pipe for all except first process
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }

            // Set up output pipe for all except last process
            if (i < num_commands - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Close all pipe ends in child
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Execute command
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            //perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    // Parent process: close all pipe ends
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all children
    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], NULL, 0);
    }
    return OK;
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
        rc = build_cmd_list(&cmd, &clist);
        
        if (rc < 0) {
            break;
        } 
        
        if (clist.num == 1) {
            Built_In_Cmds command = match_command(clist.commands[0].argv[0]);
            //printf("%s\n",clist->commands[0].argv[0] );
            if (command == BI_CMD_EXIT) {
                rc = OK_EXIT;
                //printf("Execute");
                return rc;
                //printf("Execute After");
            } else if (command == BI_CMD_DRAGON) {
                print_dragon();
                return rc;
            } else if (command == BI_CMD_CD) {
                if (clist.commands[0].argv[1] != NULL) {
                    if (chdir(clist.commands[0].argv[1]) == 0) {
                        //printf("Successfully changed directory: %s\n", cmd->argv[1]);
                    } else {
                        rc = ERR_EXEC_CMD;
                        return rc;
                    }
                }
            } 
        } 

        pid_t supervisor = fork();
        if (supervisor == -1) {
            perror("fork supervisor");
            exit(EXIT_FAILURE);
        }

        if (supervisor == 0) {
            //printf("Make it here");
            rc = execute_pipeline(&clist);
            exit(EXIT_SUCCESS);
        }
        
        waitpid(supervisor, NULL, 0);

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
    free_cmd_list(&clist);
    //printf("Broke After free\n");
    
    return rc;
}

