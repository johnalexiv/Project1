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

void getInputFromUser(char **firstCommand, char **secondCommand);
int * createPipe();
void getCommands(char *buffer, char **firstCommand, char **secondCommand);
bool bufferIsValid(char *buffer);
void childProcess(int *fileDescriptor, char **firstCommand);
void parentProcess(int pid, int *fileDescriptor, char **secondCommand);
void executeCommand(char **command);
void waitForChild(int pid);

int main()
{    
    char *firstCommand[100];
    char *secondCommand[100];

    getInputFromUser(firstCommand, secondCommand);
    
    int *fileDescriptor = createPipe();
    int pid = fork();
    if ( pid == 0 )
        childProcess(fileDescriptor, firstCommand);
    else if ( pid > 0 )
        parentProcess(pid, fileDescriptor, secondCommand);
    else
        perror(NULL);

    return 0;
}

void getInputFromUser(char **firstCommand, char **secondCommand)
{
    printf("Enter command: ");
    char *buffer = (char *)calloc(256, sizeof(char));

    if ( bufferIsValid(buffer) )
    {
        printf("No command was entered. \n");
        exit(1);
    }
    else 
    {
        getCommands(buffer, firstCommand, secondCommand);
    }
}

bool bufferIsValid(char *buffer)
{
    fgets(buffer, 256, stdin);
    buffer[strlen(buffer) - 1] = '\0';
    if ( buffer[0] == '\n' )
        return true;
    else 
        return false;
}

void getCommands(char *buffer, char **firstCommand, char **secondCommand)
{
    bool twoCommands = false;
    int size = 0;
    int tempSize = 0;

    char *token = strtok(buffer, " ");
    while( token != NULL )
    {
        if ( *token == '|' )
            twoCommands = true;
        else if ( !twoCommands )
            firstCommand[size++] = token;      
        else
            secondCommand[tempSize++] = token;

        token = strtok(NULL, " ");
    }
    firstCommand[size] = NULL;
    secondCommand[tempSize] = NULL;
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





