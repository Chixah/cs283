#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
void reverse_string(char *, int, int);
void word_print(char *, int, int);
int replace_string(char *, int, char *, char *);

// Helper function to find length of a string
int word_len (char *);


// char *buff - Internal buffer 'buff'
// char *user_str - user supplied string
// int len - size of buff
// Return - length of user supplied string after removing duplicated white spaces
int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    
    char space_char = ' ';
    int string_len = 0;
    int white_space_char_limit = 1;
    int space_count = 0;
    int tab_count = 0;
    int dots_spaces;


    for(int i=0; user_str[i] != '\0'; i++) {
        
        if(user_str[i] == space_char) {
            space_count++;
        } else {
            space_count = 0;
        }

        // '\t' is the notation for tab
        if(user_str[i] == '\t') {
            tab_count++;
        } else {
            tab_count = 0;
        }

        // If consecutive white space is found, ignore it
        if(space_count > white_space_char_limit) {
            string_len--;
            space_count--;
        } 

        if(tab_count > white_space_char_limit) {
            string_len--;
            space_count--;
        }

        // Copying over every non-white space character
        buff[string_len] = user_str[i];
        string_len++;
    }
    
    // Removing the first space if no word preceed it.
    if (buff[0] == space_char) {
        for(int i=0; i< string_len; i++) {
            buff[i] = buff[i+1];
        }
        buff[string_len] = '\0';
        string_len--;
    }
    
    // Removing the last space if no word is after it.
    if (buff[string_len -1] == space_char) {
        
        buff[string_len - 1] = '\0';
        string_len--;
    }

    // if user provided string exceeds BUFFER_SZ (50)
    if (string_len > BUFFER_SZ) {
        return -1;
    } 

    // Calculate filler space
    dots_spaces = BUFFER_SZ - string_len;

    memset((buff+string_len), '.', dots_spaces * sizeof(char));
    //  printf("%d\n", user_str_len);

    return string_len;
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    //YOU MUST IMPLEMENT

    int wc;

    // Please implement

    // Count the number of spaces between words
    for (int i = 0; i < str_len; i++) {
        if (buff [i] == ' ') {
            wc ++;
        }
    }
    
    return wc;
}

void reverse_string(char *buff, int len, int str_len){
    // Suggested local variables
    int end_idx;        //should be length of string - 1
    int start_idx;
    char tmp_char;

    // Please implement
    end_idx = str_len - 1;
    start_idx = 0;
    while (end_idx > start_idx) {
        tmp_char = buff[start_idx];
        buff[start_idx] = buff[end_idx];
        buff[end_idx] = tmp_char;
        start_idx++;
        end_idx--;

    }

    return;
}

void word_print(char *buff, int len, int str_len){
    char space_char = ' ';
    // word count
    int wc = 0 ;
    // word length
    int wlen = 0;
    char current_character;

    
    for (int i = 0; i < str_len; i++) {
        current_character = buff[i];
        
        if(current_character != space_char) {
            wlen++;
            if( wc == 0 && i ==0) {
                wc++;
                printf("%d. ", wc);
            }

            printf("%c", current_character);
        } 

        // In the readme.md example there is a space between the word and the length
        // But in the test.sh example there is not.
        if(current_character == space_char) {
            printf(" (%d)\n", wlen);
            wlen = 0;
            wc++;
            printf("%d. ", wc);
        }

    }

    printf(" (%d)\n\n", wlen);

    // Model after test case in test.sh
    printf("Number of words returned: %d\n", wc);
    return ;
}

// Calculating the length of a string
int word_len(char *str) {
    int str_len = 0;

    for(int i=0; str[i] != '\0'; i++) {
        str_len++;
    }

    return str_len;
}


int replace_string(char *buff, int str_len, char *target_string, char *replacement_string) {
    char current_character;
    int matching_char = 0;
    int target_length = word_len(target_string);
    int replacement_string_length = word_len(replacement_string);
    int difference;

    if(target_length < replacement_string_length) {
        difference = replacement_string_length - target_length;
    } else {
        difference = target_length - replacement_string_length;
    }

    for (int i = 0; i < str_len; i++) {
        current_character = buff[i];

        if(current_character == target_string[matching_char]) {
            matching_char++;
        } else {
            matching_char = 0;
        }


        if( matching_char == target_length){
            
            if((buff[i+1] == ' ') || (buff[i+1] == '\0')) {
                for(int j = str_len + difference; j > i; j--) {
                    buff[j] = buff[j - difference];
                }

                for(int k = 0; k < replacement_string_length; k++) {
                    buff[k + i + 1 - target_length] = replacement_string[k];
                }

                return 99;
            }
        }

    }
    return -1 ;
}
//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING

    // This is safe because:
    // If there is not an argv[1], it means the user did not choose an option
    // Which will tell the compiler to use argv[0] as a safe option
    // then exits showing that there is a comand line problem
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING

    // if the argument counter is less than 3 do the following:
    // usage is a function with the parameter of a pointer to the first element
    // similar to an array accessing the first element.
    // exit(1) signifies there is a command line problem
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    //https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/ refresher
    buff = (char*)malloc(BUFFER_SZ * sizeof(char));

    if (buff == NULL) {
     //   printf("Memory not allocated.\n");
        return 99;
    } else {
    //  printf("Memory successfully allocated using malloc.\n");
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    //printf("%d\n", user_str_len);

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
        

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options

        case 'r':
            reverse_string(buff, BUFFER_SZ, user_str_len);
            break;

        case 'w':
            printf("Word Print\n----------\n");

            word_print(buff, BUFFER_SZ, user_str_len);
            break;

        case 'x':
            if ((argc < 3) || (*argv[1] != '-')){
                usage(argv[0]);
                exit(1);
            } else if (argc != 5) {
                printf("Error: Need 5 Arguments\n");
                exit(3);
            }

            int result = replace_string(buff, user_str_len, argv[3], argv[4]);
            if(result == -1) {
                printf("Not Implemented!\n");
                exit(3);
            }
            
            break;


        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);

    // Modeled after test.sh
    if(opt == 'x') {
        printf("Not Implemented!\n");
    }
    free(buff);// be free
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE