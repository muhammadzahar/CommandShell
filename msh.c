/* Name: Muhammad Zaharudin
 * ID:1001835444
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports 10 arguments

#define MAX_PIDS_COMMAND 15     //maximum PIDS commands

#define MAX_HISTORY_COMMAND 15  //maximum history commands

int pidCommand[MAX_PIDS_COMMAND];  //array to store the process id of the child processes



//function to print the process id. num is the number of commands used in the child process
void pidsFunction(int num)
{
    int n;
    for (n=0; n < num; n++)
    {
        printf("%d: %d \n", n+1, pidCommand[n] );
    }
}



int main()
{

    char * command_string = (char*) malloc( MAX_COMMAND_SIZE );  //variable to store the input provided by user
    char *argv[MAX_NUM_ARGUMENTS];   //array to store the number of arguments
    char historyCommand[MAX_HISTORY_COMMAND][MAX_COMMAND_SIZE];  //array to store the commands and print the history
    int history=0;
    int pids=0;
    char *historyCmd[MAX_NUM_ARGUMENTS];    //

    while( 1 )
    {
        // Print out the msh prompt
        printf ("msh> ");
        // Read the command from the commandline.  The
        // maximum command that will be read is MAX_COMMAND_SIZE
        // This while command will wait here until the user
        // inputs something since fgets returns NULL when there
        // is no input
        while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

        /* Parse input */
        char *token[MAX_NUM_ARGUMENTS];

        int   token_count = 0;                                 

        // Pointer to point to the token
        // parsed by strsep
        char *argument_ptr;                                         

        char *working_string  = strdup( command_string );                

        // we are going to move the working_string pointer so
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *head_ptr = working_string;

        // Tokenize the input strings with whitespace used as the delimiter
        while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
                  (token_count<MAX_NUM_ARGUMENTS))
        {
            token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
            if( strlen( token[token_count] ) == 0 )
            {
                token[token_count] = NULL;
            }
            token_count++;
        }

    // \TODO shell functionality
		/*
		store the commands in history array. then it'll increment the array position by 1. 
		first entry would be 0 and increment history by 1 making history 1 after sorting
		*/
        strcpy(historyCommand[history],command_string);
        history++;

        
        //when no command is entered, msh will just prompt again in another line 
        if (token[0] == NULL)
        {
            continue;
        }
        //exits shell when exit or quit is entered 
        if ((strcmp(token[0], "exit") == 0) || (strcmp(token[0], "quit")) == 0)
        {
            exit(0);
        }

        //executing !n where n is the history command from 0-14
        //when an ! is detected execute the following instructions
        if(command_string[0] == '!')
        {
            //converting the history command to integer so we can use as n
            int nthNum = atoi(&command_string[1]);
            //when the nth number is not in the range of the history (0-14) or in the history, execute prompt
            if (nthNum > history)
            {
              printf("Command not in history.\n");
            }
            else
            {
				//copy the command in the nth position to the original command string 
				//to execute the command
                strcpy(command_string, historyCommand[nthNum - 1]);
              
           //reusing the code from above because we need to parse the input from history
                token_count = 0;                                 

                working_string  = strdup( command_string );                

               // we are going to move the working_string pointer so
               // keep track of its original value so we can deallocate
               // the correct amount at the end
                head_ptr = working_string;

               // Tokenize the input strings with whitespace used as the delimiter
                while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
                      (token_count<MAX_NUM_ARGUMENTS))
                {
                    token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
                    if( strlen( token[token_count] ) == 0 )
                    {
                        token[token_count] = NULL;
                    }
                        token_count++;
                }
            }
        }
       
        //when showpids is entered, the pids function will be called showing the recently used pids in the child
        if ((strcmp(token[0], "showpids")) == 0)
        {
            //printf("showpids is %d \n", pids);
            pidsFunction(pids);
        }
        //execute the cd command. 
        if(strcmp(token[0],"cd") == 0)
        {
            //cd follow a directory or .. if its blank execute the prompt
            if (token[1] == NULL)
            {
                printf("%s: Directory not found.\n", token[1]);
            }
            else
            {
                chdir(token[1]);
            }

        }

    
        
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)     //child process
        {
            /* search the following PATH at minimum:
             * Current working directory
             * /usr/local/bin
             * /usr/bin
             *  /bin
             */

             //when history is entered, the history function will be called to show the history of the commands
            if ((strcmp(token[0], "history")) == 0)
            {
                int n;
                for (n=0; n < history; n++)
                {
                    printf("%d: %s\n", n+1, historyCommand[n] );
                }
            }


            int ret = execvp(token[0], token);
            if( ret == -1 )
            {
                printf("%s: command not found\n ",token[0]);
            }
            exit(EXIT_SUCCESS);  //exit before parent process

        }
        else       //parent process
        {
            int status;
            //wait and terminate after child process terminates
            waitpid(pid,&status, 0);
            wait(&status);
            pidCommand[pids] = pid;
            pids++;


        }

    }
    return 0;
  
}
