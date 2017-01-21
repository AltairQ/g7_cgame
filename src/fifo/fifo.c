#ifdef _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "fifo.h"

struct pipes {
    HANDLE fifo_in, fifo_out;
    bool is_host;
};

void pokazBlad(char *komunikat)
{
	puts(komunikat);
}

static HANDLE openOutPipe(char *name);
static HANDLE openInPipe(char *name);

void closePipes(PipesPtr pipes)
{
  CloseHandle(pipes->fifo_in);
  CloseHandle(pipes->fifo_out);
  free(pipes);
}

PipesPtr initPipes(bool host)
{
    PipesPtr pipes=(PipesPtr)malloc(sizeof(struct pipes));
    if (pipes == NULL) {
        fprintf(stderr,"Memory allocation error\n");
    } else {
        pipes->is_host=host;
        pipes->fifo_out = openOutPipe(pipes->is_host ? "\\\\.\\pipe\\AtoB" : "\\\\.\\pipe\\BtoA");
        pipes->fifo_in = openInPipe(pipes->is_host ? "\\\\.\\pipe\\BtoA" : "\\\\.\\pipe\\AtoB");
    }
    return pipes;
}

static HANDLE openOutPipe(char *name) {
    HANDLE pipe = CreateNamedPipe(
                   name, // name of the pipe
                   PIPE_ACCESS_OUTBOUND, // 1-way pipe -- send only
                   PIPE_TYPE_BYTE, // send data as a byte stream
                   1, // only allow 1 instance of this pipe
                   0, // no outbound buffer
                   0, // no inbound buffer
                   0, // use default wait time
                   NULL // use default security attributes
               );
    if (pipe == NULL || pipe == INVALID_HANDLE_VALUE)
    {
        printf("Error in creating output pipe\n");
        // look up error code here using GetLastError()
        exit(-1);
    }
    return pipe;
}

static HANDLE openInPipe(char *name){
    HANDLE pipe;
    do
    {
        pipe = CreateFile(
                      name,
                      GENERIC_READ, // only need read access
                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                      NULL,
                      OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL,
                      NULL
                  );
        if (pipe == INVALID_HANDLE_VALUE)
        {
            printf("Pipe is not ready to connect - waiting for the second copy of the program ...\n");
            Sleep(3000);
        }
    }
    while (pipe == INVALID_HANDLE_VALUE);
    return pipe;
}

void sendStringToPipe(PipesPtr pipes,const char *data)
{
    DWORD numBytesWritten = 0;
    int result = WriteFile(
                     pipes->fifo_out, // handle to our outbound pipe
                     data, // data to send
                     strlen(data), // length of data to send (bytes)
                     &numBytesWritten, // will store actual amount of data sent
                     NULL // not using overlapped IO
                 );
    if (!result)
    {
        pokazBlad("Failed to send data");
     }
}

bool getStringFromPipe(PipesPtr pipes,char *buffer,size_t size)
{
    DWORD numBytesRead = 0, totalBytesAvail;

    int peekResult = PeekNamedPipe(
                     pipes->fifo_in,
                     buffer, // the data from the pipe will be put here
                     size-1, // number of bytes allocated
                     NULL, // do not want to read from pipe
                     &totalBytesAvail,
                     NULL);
    if (peekResult == 0 || totalBytesAvail == 0)
        return false;
    int result = ReadFile(
                     pipes->fifo_in,
                     buffer, // the data from the pipe will be put here
                     size-1, // number of bytes allocated
                     &numBytesRead, // this will store number of bytes actually read
                     NULL // no using overlapped IO
                 );
    if (result) buffer[numBytesRead] = '\0';
    return result;
}



#endif


#ifdef __linux__

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "fifo.h"

struct pipes {
    FILE *fifo_in, *fifo_out;
    bool is_host;
} ;

int fileno(FILE *file);
void pokazBlad(char *komunikat);
static FILE *openOutPipe(char *name);
static FILE *openInPipe(char *name);

void closePipes(PipesPtr pipes) 
{ 
  fclose(pipes->fifo_in); 
  fclose(pipes->fifo_out); 
  free(pipes); 
}

PipesPtr initPipes(bool host)
{
    PipesPtr pipes=(PipesPtr)malloc(sizeof(struct pipes));
    if (pipes == NULL) {
        fprintf(stderr,"Memory allocation error\n");
    } else {
        pipes->is_host= host;
        pipes->fifo_out = openOutPipe(pipes->is_host ? "AtoB" : "BtoA");
        pipes->fifo_in = openInPipe(pipes->is_host ? "BtoA" : "AtoB");
    }
    return pipes;
}

static FILE *openOutPipe(char *name) {
    FILE *pipe = fopen(name, "w+");
    if (pipe == NULL)
    {
        pokazBlad("Error in creating output pipe");
        exit(-1);
    }
    return pipe;
}

static FILE *openInPipe(char *name){
    FILE *pipe = fopen(name, "r+");
    if (pipe == NULL)
    {
        pokazBlad("Error in creating input pipe");
        exit(-1);
    }
    int flags, fd;
    fd = fileno(pipe);
    flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return pipe;
}

void sendStringToPipe(PipesPtr pipes, const char *data)
{
    int result = fprintf(pipes->fifo_out,"%s",data);
    fflush(pipes->fifo_out);
    if (result == 0)
        pokazBlad("Failed to send data");
}

bool getStringFromPipe(PipesPtr pipes, char *buffer, size_t size)
{
    char *result = fgets(buffer,size,pipes->fifo_in);
    fflush(pipes->fifo_in);
    //if (result == NULL) pokazBlad("Failed to read data");
    return result != NULL;
}

#endif