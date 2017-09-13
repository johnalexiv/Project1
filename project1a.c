// Project 1
// John Alexander
// CS 370
// Sept. 7, 2017

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h> 
#include <stdbool.h>

bool getInputFromUser(char **, char **);
int * createPipe();
bool getCommands(char *, char **, char **);
bool bufferIsValid(char *);
void childProcess(int *, char **);
void parentProcess(int, int *, char **);
void executeCommand(char **);
void waitForChild(int);

int main()
{    
    char *firstCommand[100];
    char *secondCommand[100];

    bool twoCommands = getInputFromUser(firstCommand, secondCommand);
    
    int *fileDescriptor = createPipe();
    int pid = fork();
    if ( pid == 0 )
        childProcess(fileDescriptor, firstCommand);
    else if ( pid > 0 && twoCommands)
        parentProcess(pid, fileDescriptor, secondCommand);
    else
        perror(NULL);

    return 0;
}

bool getInputFromUser(char **firstCommand, char **secondCommand)
{
    printf("Enter command: ");

    bool twoCommands;
    char *buffer = (char *)calloc(256, sizeof(char));
    if ( bufferIsValid(buffer) )
    {
        printf("No command was entered. \n");
        exit(1);
    }
    else 
    {
        if ( strlen(buffer) > 0 )
            buffer[strlen(buffer) - 1] = '\0';
        twoCommands = getCommands(buffer, firstCommand, secondCommand);
    }

    return twoCommands;
}

bool bufferIsValid(char *buffer)
{
    fgets(buffer, 256, stdin);
    if ( buffer[0] == '\n' )
        return true;
    else 
        return false;
}

bool getCommands(char *buffer, char **firstCommand, char **secondCommand)
{
    bool twoCommands = false;
    int firstSize = 0;
    int secondSize = 0;

    char *token = strtok(buffer, " ");
    while( token != NULL )
    {
        if ( *token == '|' )
            twoCommands = true;
        else if ( !twoCommands )
            firstCommand[firstSize++] = token;      
        else
            secondCommand[secondSize++] = token;

        token = strtok(NULL, " ");
    }
    firstCommand[firstSize] = NULL;
    secondCommand[secondSize] = NULL;

    return twoCommands;
}

int * createPipe()
{
    int *fileDescriptor = (int *)malloc(2 * sizeof(int *));
    if ( pipe(fileDescriptor) < 0)
    {
        perror(NULL);
        exit(1);
    }
    return fileDescriptor;
}

void childProcess(int *fileDescriptor, char **firstCommand)
{
    close(1);
    dup(fileDescriptor[1]);
    close(fileDescriptor[0]);
    executeCommand(firstCommand);
    exit(0);
}

void parentProcess(int pid, int *fileDescriptor, char **secondCommand)
{
    waitForChild(pid);
    close(0);
    dup(fileDescriptor[0]);
    close(fileDescriptor[1]);
    executeCommand(secondCommand);
}

void executeCommand(char **command)
{
    if ( execvp(command[0], command) < 0 )
    {
        perror(NULL);
        exit(1);
    }
}

void waitForChild(int pid)
{
    int status;
    if ( waitpid(pid, &status, WUNTRACED) < 0)
    {
        perror(NULL);
        exit(1);
    }
}





