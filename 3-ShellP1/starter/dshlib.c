#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
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
                strncpy(clist->commands[command].exe, temp, count+1);
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
                    strncpy(clist->commands[command].args, temp, count);
                } else {
                    temp[count] = '\0';
                /* for(int j = 0; temp[j] != '\0'; j++) {
                    printf("%c[%d] ", temp[j], temp[j]);
                }
                printf("Count: %d ", count); */
                    strncpy(clist->commands[command].args, temp, count+1);
                //printf("\n");
                }
                
            } 
            if(command_header == 1){
                temp[count] = '\0';
                /* for(int j = 0; temp[j] != '\0'; j++) {
                    printf("%c[%d] ", temp[j], temp[j]);
                }
                printf("Count: %d ", count); */
                strncpy(clist->commands[command].exe, temp, count+1);
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
        strncpy(clist->commands[command].exe, temp, count+1);
        clist->commands[command].args[0] = '\0';
    } else if (command_header == 0){
        strncpy(clist->commands[command].args, temp, count+1);
    }
    command++;
    

    if (command > 8) {
        rc = ERR_TOO_MANY_COMMANDS;
    } else {
        clist->num = command;
    }

    for (int i = 0; i < clist->num; i++) {
        if(strlen(clist->commands->args) > 256) {
            rc = ERR_CMD_OR_ARGS_TOO_BIG;
        } else if (strlen(clist->commands->exe) > 64) {
            rc = ERR_CMD_OR_ARGS_TOO_BIG;
        }
    }
    
    
    //printf("\n");
    //printf("Number of Commands: %d\n", clist->num);
    return rc;
 }

 