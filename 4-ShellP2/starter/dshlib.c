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
    //printf("Successfully cleared buff\n");

    if (strlen(cmd_line) == 0 || is_empty(cmd_line) == 0) {
        rc = WARN_NO_CMDS;
    } 

    
    /*printf("Printing User Input Before Trim\n");
    for (int i = 0; cmd_line[i] != '\0'; i++) {
        printf("%c[%d] ", cmd_line[i], cmd_line[i]);
    } */

    //printf("\n");

    // trim ALL leading and trailing spaces
    int leading_space = 0;
    char temp[256];

    //printf("String Length of cmd_line is %ld\n\n", strlen(cmd_line));
    for(int i = 0; cmd_line[i] != '\0'; i++) {
        char current_character = cmd_line[i];
        if (current_character == SPACE_CHAR && leading_space == i) {
            leading_space++;
        } 
    }

    int count = 0;
    for(int i = leading_space; cmd_line[i] != '\0'; i++) {
        char current_character = cmd_line[i];
        temp[count] = current_character;
        count++;
    }
    temp[count] = '\0';

    int ending_space = strlen(temp);

    /*printf("Temp before ending trim + Length %ld\n\n", strlen(temp));
    for (int i = 0; temp[i] != '\0'; i++) {
        printf("%c[%d] ", temp[i], temp[i]);
    } */

    //printf("\n");

    for(int i = strlen(temp) - 1; i != -1; i--) {
        char current_character = temp[i];
        //printf("%c[%d] ", current_character, current_character);
        if(current_character == SPACE_CHAR && ending_space-1 == i) {
            ending_space--;
            //printf("  Space Detected  ");
        }
    }

    //printf("\n %d\n\n", ending_space);
    //printf("Temp After ending trim + Length %ld\n\n", strlen(temp));
    char new_temp[256];

    for(int i = 0; i < ending_space; i++) {
        new_temp[i] = temp[i];
    }
    new_temp[ending_space] = '\0';



    /* printf("Printing User Input After Trim\n");
    for (int i = 0; new_temp[i] != '\0'; i++) {
        printf("%c[%d] ", new_temp[i], new_temp[i]);
    } */
    //printf("String Length of temp is %ld\n\n", strlen(new_temp));

    //printf("\n");
    

    strncpy(cmd_buff->_cmd_buffer, new_temp, strlen(new_temp)+1);
    /* printf("String Length of cmd_buffer is %ld\n\n", strlen(cmd_buff->_cmd_buffer));
    for (int i = 0; cmd_buff->_cmd_buffer[i] != '\0'; i++) {
        printf("%c[%d] ", cmd_buff->_cmd_buffer[i], cmd_buff->_cmd_buffer[i]);
    } */

    // Break Into Arguments
    for(int i = 0; i < 256; i++) {
        temp[i] = '\0';
    }
    int within_quotes = 0;
    count = 0;
    int arguments = 0;
    //printf("\n\n");
    for(int i = 0; new_temp[i] != '\0'; i++) {
        char current_character = new_temp[i];
        //printf("Character position[%d]: %c[%d]\n", i, current_character, current_character);
        if(current_character == '"'){
            
            if(within_quotes == 1) {
                //printf("Argument: %d\nCount: %d\n", arguments, count);
                temp[count] = '\0';
                cmd_buff->argv[arguments] = (char *) malloc(ARG_MAX * sizeof(char));
                strncpy(cmd_buff->argv[arguments], temp, count+1);
                for(int i=0; i<count+1; i++) {
                    temp[count] = '\0';
                }
                count = 0;
                arguments++;
                within_quotes = 0;
            } else {
                char previous_character = new_temp[i-1];

                //printf("Argument: %d\nCount: %d\n", arguments, count);
                within_quotes = 1;
                if (previous_character == SPACE_CHAR) {
                    count = 0;
                    for(int i=0; i< 256; i++) {
                        temp[count] = '\0';
                    }
                } else {
                    temp[count] = '\0';
                    cmd_buff->argv[arguments] = (char *) malloc(ARG_MAX * sizeof(char));
                    strncpy(cmd_buff->argv[arguments], temp, count+1);
                    for(int i=0; i<count+1; i++) {
                        temp[count] = '\0';
                    }
                    count = 0;
                    arguments++;
                    
                }
            }
        } else {
            char previous_character = new_temp[i-1];
            if(current_character == SPACE_CHAR && within_quotes == 0 && previous_character != SPACE_CHAR) {
                temp[count] = '\0';
                cmd_buff->argv[arguments] = (char *) malloc (ARG_MAX * sizeof(char));
                strncpy(cmd_buff->argv[arguments], temp, count+1);
                for(int i=0; i<count+1; i++) {
                    temp[count] = '\0';
                }
                count = 0;
                arguments++;
                
            } else {
                temp[count] = current_character;
                count++;
            }
            
        }

    }

    //printf("Current arguments: %d\n\n", arguments);
    //printf("Value of within_quotes: %d\n\n", within_quotes);
    if (new_temp[0] != SPACE_CHAR && strlen(new_temp) != 0) {
        temp[count] = '\0';
        cmd_buff->argv[arguments] = (char *) malloc (ARG_MAX * sizeof(char));
        strncpy(cmd_buff->argv[arguments], temp, count+1);
        arguments++;
    }

    

    /* for(int i = 0; i < arguments; i++) {
        printf("Argument[%d]: \n\n", i);
        for(int j = 0; cmd_buff->argv[i][j] != '\0'; j++){
            printf("%c",  cmd_buff->argv[i][j]);
        }
        printf("Argument length %ld\n", strlen(cmd_buff->argv[i]));
        printf("\n");
    } */
    cmd_buff->argc = arguments;

    if (arguments > CMD_MAX) {
        rc = ERR_CMD_ARGS_BAD;
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


int exec_cmd(cmd_buff_t *cmd) {
    int rc = OK;
    Built_In_Cmds command = match_command(cmd->argv[0]);
    //printf("Command is %d\n\n", command);
    if (command == BI_CMD_EXIT) {
        rc = OK_EXIT;
        return rc;
    } else if (command == BI_CMD_DRAGON) {
        print_dragon();
    } else if (command == BI_CMD_CD) {
        if (cmd->argv[1] != NULL) {
            if (chdir(cmd->argv[1]) == 0) {
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
            if(execvp(cmd->argv[0], cmd->argv) == -1) {
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
    
   
    return rc;
}


int exec_local_cmd_loop()
{
    char cmd_buff[SH_CMD_MAX];
    int rc = 0;
    cmd_buff_t cmd;

    //printf("At the start\n");

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
