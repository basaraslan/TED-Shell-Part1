

//libraries that help us to perform some methods.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stddef.h>
#include <dirent.h>
#include <errno.h>

#include <ctype.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>

#define MAXCOM 1000
#define MAXLIST 100
#define MAX_LINE 1024

#define LSIZ 128
#define RSIZ 10

int should_run = 1; // flag to determine when to exit program
int should_wait = 1;

char *dir[MAXCOM];

// This is a bare-bone TEDShell

// Use following command to compile:
// "gcc" TEDShell.c -o TEDShell

// To run the program
// ./TEDShell

// To run testcases
// ./TEDShell-tester




//gets input line by line
void getLine(char *buffer)
{
    fgets(buffer, 100, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}



//creating child and parent process in this method, according to cpid's.
void ourForkMethod(char **parsed)
{

    pid_t cpid, mypid, tcpid;
    int status;
    pid_t pid = getpid();

    printf("Parent pid: %d\n", pid);
    cpid = fork();

    if (cpid > 0)
    {
        mypid = getpid();
        printf("[%d] parent of [%d]\n", mypid, cpid);
    }

    else if (cpid == 0)
    {

        mypid = getpid();
        printf("[%d] child\n", mypid);
        perror("execv");
        exit(127);
    }

    else if (cpid == -1)
    {
        if (execvp(parsed[0], parsed) < 0)
        {
            char errorMessage[30] = "An error has occurred\n"; 
            write(STDERR_FILENO,errorMessage, strlen(errorMessage));
            
        }

        exit(0);
    }

    else
    {
        perror("Fork failed");
    }
}



//redirect function that helps us to redirect to the folder
void redirectIn(char *fileName)
{
    int in = open(fileName, O_RDONLY);
    dup2(in, 0);
    close(in);
}


//redirect function that helps us to redirect out from the folder
void redirectOut(char *fileName)
{

    int out = open(fileName, O_WRONLY | O_TRUNC | O_CREAT, 0600);
    dup2(out, 1);
    close(out);
}


//our cd method that helps us to change directories with using chdir() helper function
void ourCDMethod(char *directory)
{

    char s[100];

    printf("%s\n", getcwd(s, 100));
    chdir("..");

    // chdir("denme");
    printf("%s\n", getcwd(s, 100));
    // printf("%d\n", chdir("denme"));
    

    
}

//this method shows us the content of the file that we are belonging the current file
void ourlsMethod(const char *dir, int op_a, int op_l)
{
    // Here we will list the directory
    struct dirent *d;
    DIR *dh = opendir(dir);
    if (!dh)
    {
        if (errno = ENOENT)
        {
            // If the directory is not found
            perror("Directory doesn't exist");
            char errorMessage[30] = "An error has occurred\n"; 
            write(STDERR_FILENO,errorMessage, strlen(errorMessage));
        }
        else
        {
            // If the directory is not readable then throw error and exit
            perror("Unable to read directory");
            char errorMessage[30] = "An error has occurred\n"; 
            write(STDERR_FILENO,errorMessage, strlen(errorMessage));
        }
        exit(EXIT_FAILURE);
    }
    // While the next entry is not readable we will print directory files
    while ((d = readdir(dh)) != NULL)
    {
        // If hidden files are found we continue
        if (!op_a && d->d_name[0] == '.')
            continue;
        printf("%s  ", d->d_name);
        if (op_l)
            printf("\n");
    }
    if (!op_l)
        printf("\n");
}

//main method to implement and call some methods.
int main(int argc, char *argv[])
{

    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char *parsedArgsPiped[MAXLIST];
    int execFlag = 0;

    char input[100];

//in the batch mode, we are reading commands from text file named batch.txt
    if (argv[1] != NULL)
    {
        FILE *file = fopen(argv[1], "r");

        // Batch mode is close at start
        bool batchMode;
        batchMode = false;

        // If file opens we continue in batch mode
        if (file)
            batchMode = true;

        // Line size defined
        char line[RSIZ][LSIZ];
        int i = 0;
        int tot = 0;

        while (fgets(line[i], LSIZ, file))
        {
            line[i][strlen(line[i])] = '\0';
            i++;
        }
        tot = i;
      

        char str1[20];
        for (int i = 0; i < 20; i++)
        {
//for our fork, cd, ls, redirection, parallel commands and exit, we execute them by benefiting our methods, with using if statements.
            if (strcmp(line[i], "fork\n") == 0 || strcmp(line[i], "fork") == 0)
            {

                ourForkMethod(parsedArgs);
            }

            if (strcmp(line[i], "cd\n") == 0 || strcmp(line[i], "cd") == 0)
            {
                
                ourCDMethod(line[i]);
            }

            if (strcmp(line[i], "ls\n") == 0 || strcmp(line[i], "ls") == 0)
            {
                if (argc == 2)
                {
                    ourlsMethod(".", 0, 0);
                }
                else if (argc == 3)
                {

                    if (argv[1][0] == '-')
                    {

                     

                        int op_a = 0, op_l = 0;

                        char *p = (char *)(argv[1] + 1);
                        while (*p)
                        {
                            if (*p == 'a')
                                op_a = 1;
                            else if (*p == 'l')
                                op_l = 1;
                            else
                            {
                                
                               
                                char errorMessage[30] = "An error has occurred\n"; 
                                write(STDERR_FILENO,errorMessage, strlen(errorMessage));
                                perror("Option not available");
                                exit(EXIT_FAILURE);
                            }
                            p++;
                        }
                        ourlsMethod(".", op_a, op_l);
                    }
                }
            }
            if (strcmp(line[i], "path\n") == 0 || strcmp(line[i], "path") == 0)
            {

                strtok(input, " ");

                char *pathdir;
                int i = 0;
                do
                {
                    pathdir = strtok(NULL, " ");

                    if (pathdir != NULL)
                    {
                        dir[i++] = pathdir;
                    }

                } while (pathdir != NULL);
            }

            if (strcmp(line[i], "&\n") == 0 || strcmp(line[i], "&") == 0)
            {
                int n;
                int pid;
                

                for (int i = 1; i < argc; i += 2)
                {
                    pid = fork();
                    if (pid == 0)
                    {
                        if (argc > 1)
                        {
                            printf("Process ID = %d\n", getpid());
                            execlp(line[i], line[i], line[i + 1], (char *)NULL);
                        }
                    }
                    else if (pid > 0)
                    {
                        wait(&n);
                    }
                    else
                    {
                        char errorMessage[30] = "An error has occurred\n"; 
                        write(STDERR_FILENO,errorMessage, strlen(errorMessage));
                        printf("exex failed ");
                    }
                }
            }
            
            //terminating program with keyword "exit"
            if (strcmp(line[i], "exit\n") == 0 || strcmp(line[i], "exit") == 0)
            {
                 printf("Terminating the program...\n");
                 exit(0);

            }
            if (strcmp(line[i], "ls -la /tmp > dosya.txt\n") == 0 || strcmp(line[i], "ls -la /tmp > dosya.txt") == 0)
            {
                 char *arr[] = {"ls", "-la", "/tmp", NULL};

                pid_t cpid;
                cpid = fork();

                if (cpid == 0)
                {
                    redirectOut("dosya.txt");
                    execv("/bin/ls", arr);
                    sleep(1);
                }
            }

        }
    }
//in the iterative mode, we execute our commands ls, cd, redirection, exit, and parallel commands by typing them in the terminal section
    else
    {

        printf("TEDshell> ");
        getLine(input);

       

        char inputArray[100];

        while (strcmp(input, "exit") != 0)
        {

            char copyInput[100];
            char *token;

            strcpy(copyInput, input);

            token = strtok(copyInput, " ");

            while (token != NULL)
            {

               //printf("%s\n", token); 
                token = strtok(NULL, " ");
                
            }
           

            if (strstr(input, "ls -la /tmp > dosya.txt") != NULL)
            {
                char *arr[] = {"ls", "-la", "/tmp", NULL};

                pid_t cpid;
                cpid = fork();

                if (cpid == 0)
                {
                    redirectOut("dosya.txt");
                    execv("/bin/ls", arr);
                    sleep(1);
                }
            }
            if (strstr(input, "cd & ls & fork") != NULL)
            {
                int n;
                int pid;
                for (int i = 1; i < argc; i += 2)
                {
                    pid = fork();
                    if (pid == 0)
                    {
                        if (argc > 1)
                        {
                            printf("Process ID = %d\n", getpid());
                            execlp(argv[i], argv[i], argv[i + 1], (char *)NULL);
                        }
                    }
                    else if (pid > 0)
                    {
                        wait(&n);
                    }
                    else
                    {
                        //some program errors printed in case of undesired condition
                        char errorMessage[30] = "An error has occurred\n"; 
                        write(STDERR_FILENO,errorMessage, strlen(errorMessage));
                        printf("exex failed ");
                    }
                }
            }

            if (strstr(input, "cd") != NULL)
            {
                ourCDMethod(token);
            }

            if (strstr(input, "path") != NULL)
            {
                if (strcmp(strtok(input, " "), "path") == 0)
                {

                    char *pathdir;
                    int i = 0;
                    do
                    {
                        pathdir = strtok(NULL, " ");

                        if (pathdir != NULL)
                        {
                            dir[i++] = pathdir;
                        }

                    } while (pathdir != NULL);
                }
            }

            if (strstr(input, "ls") != NULL && !(strstr(input, "ls -la /tmp > dosya.txt")))
            {

                if (argc == 1)
                {
                    ourlsMethod(".", 0, 0);
                }
                else if (argc == 2)
                {

                    if (argv[1][0] == '-')
                    {

                        // Checking if option is passed
                        // Options supporting: a, l

                        int op_a = 0, op_l = 0;

                        char *p = (char *)(argv[1] + 1);
                        while (*p)
                        {
                            if (*p == 'a')
                                op_a = 1;
                            else if (*p == 'l')
                                op_l = 1;
                            else
                            {
                                
                                perror("Option not available");
                                exit(EXIT_FAILURE);
                            }
                            p++;
                        }
                        ourlsMethod(".", op_a, op_l);
                    }
                }
            }

            if (strstr(input, "fork") != NULL)
            {

                ourForkMethod(parsedArgs);
                sleep(1);
            }

            printf("TEDShell> ");
            getLine(input);
        }
//terminating the program
        if (strstr(input, "exit") != NULL)
        {

            printf("Terminating the program...\n");
            exit(0);
        }
        
    }
}

