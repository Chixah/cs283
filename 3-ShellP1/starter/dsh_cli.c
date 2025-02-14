#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dshlib.h"

/*
 * Implement your main function by building a loop that prompts the
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.  Since we want fgets to also handle
 * end of file so we can run this headless for testing we need to check
 * the return code of fgets.  I have provided an example below of how
 * to do this assuming you are storing user input inside of the cmd_buff
 * variable.
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
 *
 *   Expected output:
 *
 *      CMD_OK_HEADER      if the command parses properly. You will
 *                         follow this by the command details
 *
 *      CMD_WARN_NO_CMD    if the user entered a blank command
 *      CMD_ERR_PIPE_LIMIT if the user entered too many commands using
 *                         the pipe feature, e.g., cmd1 | cmd2 | ... |
 *
 *  See the provided test cases for output expectations.
 */
void empty(command_list_t *clist) {
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9 ; j++ ){
            clist->commands[i].exe[j] = '\0';
            clist->commands[i].args[j] = '\0';
        }

    }
}

void print_dragon(FILE *fptr){
    char line[256];

    while(fgets(line, ARG_MAX, fptr) != NULL) {
        printf("%s", line);
    }
}

int main()
{
    char cmd_buff[SH_CMD_MAX];
    int rc = 0;
    command_list_t clist;
    //printf("Outside While loop:\n");

    empty(&clist);

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

        
        // IMPLEMENT THE REST OF THE REQUIREMENTS
        rc = build_cmd_list(cmd_buff, &clist);

        //printf("My Rc: %d\n", rc);
        //printf("Before Exit Command:\n");
        // exit
        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            rc = OK;
            break;
        }


        if (rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
        } else if (rc == OK) {
            printf(CMD_OK_HEADER, clist.num);
            int dragon_check = 0;
            char dragon[] = "dragon";
            for(int i = 0; i < clist.num; i++){
                printf("<%d> ", i+1);
                for(int j = 0; clist.commands[i].exe[j] != '\0'; j++) {
                    //printf("%c[%d] ", clist.commands[i].exe[j], clist.commands[i].exe[j]);
                    printf("%c", clist.commands[i].exe[j]);
                    char current_character = clist.commands[i].exe[j];
                    if(current_character == dragon[j]) {
                        dragon_check++;
                    } else {
                        dragon_check = 0;
                    }
                }
                if (dragon_check == 6) {
                    FILE *fptr = NULL;

                    fptr = fopen("dragon.txt", "r");
                    printf("\n");
                    print_dragon(fptr);
                    fclose(fptr);
                }
                //printf("Exe length: %ld ", (strlen(clist.commands[i].exe)));
                if(clist.commands[i].args[0] != '\0') {
                    printf(" [");
                    for(int j = 0; clist.commands[i].args[j] != '\0'; j++) {
                        printf("%c", clist.commands[i].args[j]);
                        //printf("%c[%d] ", clist.commands[i].args[j], clist.commands[i].args[j]);
                    }
                    printf("]");
                }
                
                //printf("Argument Length: %ld ", (strlen(clist.commands[i].args)));
                printf("\n");
            } 
        } else if (rc == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, 8);
        }

        //printf("End Check:\n");

        
        empty(&clist);
    }

    
    exit(rc);
}