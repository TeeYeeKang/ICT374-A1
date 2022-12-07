#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int idx;
    int check = 1; // check number of argument 
    int success = 1; // check if command completed successfully
    pid_t pidArray[argc]; // declare child process array

    // check if any command line argument entered
    if (argc == 1)
    {
        check = 0; // represent false
    }
    else
    {
        // if there are command line arguments
        for (idx = 1; idx < argc; idx++)
        {
            //create child process
            pidArray[idx] = vfork();  
            //pidArray[idx] = fork();  

            if (pidArray[idx] > 0) // parent process
            {
                // once child terminate
                wait( (int *)0);     // let parent process wait for child to terminate

                //display complete or incomplete message
                if (success == 1)
                {
                    printf("\nCommand %s has completed successfully\n\n", argv[idx]);
                }
                else
                {
                    printf("\nCommand %s has not completed successfully\n\n", argv[idx]);
                }
            }
            else if (pidArray[idx] == 0) // child process
            {
                success = 1;
                execl(argv[idx], argv[idx], (char *)0); // locate program file

                // if unsuccessful - program will execute the following code
                perror("execv fail");
                success = 0; 
                exit(EXIT_FAILURE);
            }
            else
            {
                perror("No child. Fork return error code");
                exit(EXIT_FAILURE); // terminate program if fail to fork
            }
        }
    }

    // display final message
    if (check == 1)
        printf("All done, bye-bye !\n\n");
    else
        printf("No command line argument entered\n\n");

    exit(0);
}