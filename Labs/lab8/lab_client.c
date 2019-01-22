//Adam Granieri
//COE 0449 Systems Software
//Fall 2018

//Copied libraries and Block size from serv.c for consitency
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//Added libraries
#include <stdbool.h>
#include <netdb.h>
#include <errno.h>

// Structs for repeated operations
typedef struct download download;
typedef struct downloadFactory downloadFactory;
struct downloadFactory {
    char* ip;
    int port;
    int socketD;
    int validSocket;
    int blockSize;
    int index;
    void* buff;
    download* downloadInstance;
};
struct download {
    int threadCount;
    int buffSize;
    void* buff;
    FILE* output;
    downloadFactory* downloaders;
};

//Definitions
#define BLOCK_SIZE 1024

//function prototypes
download* newDownloader(int, char**);
void* startDownload(void*);
int connectSocket(int, char*);
void backgroundDownload(downloadFactory*);
ssize_t requestBlock(int, downloadFactory*);
void writeBlockToFile(int, ssize_t, downloadFactory*);
void runDownload(download*);
void destroyDownload(download*);

//Functions
//main to actually run
/*
We check input args, instantiate download
ans download threads, then execute and then
finally free and destroy mutexes
*/
int main(int argc, char* argv[]) {
    download* newDownload = newDownloader(argc, argv);
    startDownload(newDownload);
    runDownload(newDownload);
    destroyDownload(newDownload);
    return 0;
}

//Function for initializing a new downlaod
/*
Here we instatiate a download object and give it
thread counts for each of the avalible ports.
the output file for writing is opened and the
buffer is set up for blocks from the server.
We zero out the buffer then init its mutexes.
Finally now we're iterating through the other
threads and locking each of them and connecting
them through the factory.
*/
download* newDownloader(int argc, char* argv[]) {
    int downloadThreadCount = 1;
    download* returnDown = (download*)malloc(sizeof(download));

    //Assign download properties
    returnDown->threadCount = downloadThreadCount;
    returnDown->downloaders = (downloadFactory*)malloc(sizeof(downloadFactory)*downloadThreadCount);
    returnDown->output = fopen(argv[2],"w");
    returnDown->buffSize = BLOCK_SIZE*downloadThreadCount;
    returnDown->buff = malloc(returnDown->buffSize);    

    //Check if file open worked before threading
    if (!returnDown->output) {
        printf("Failed to open file\n");
    }

    //zero out buffer
    memset(returnDown->buff, 0, BLOCK_SIZE*downloadThreadCount);

    //initialize download threads
    int arg = 1;
    int i;
    for (i = 0; i <= downloadThreadCount; i++) {
        //Load in ip address
        int length = strlen(argv[arg]);
        char* ipStr = malloc(10*sizeof(char));
        ipStr = strcpy(ipStr, "127.0.0.1");
        ipStr[length] = '\0';
        memcpy(ipStr, argv[arg], length);
        arg++;

        //initalize download struct
        downloadFactory d;
        d.ip = ipStr;
        d.index = i;
        d.validSocket = 1;
        d.port = atoi(argv[arg++]);
        d.blockSize = BLOCK_SIZE;
        d.buff = malloc(BLOCK_SIZE);

        //zero out buffer
        memset(d.buff, 0, BLOCK_SIZE);

        //finish download initialization
        d.downloadInstance = returnDown;
        returnDown->downloaders[i] = d;
    }
    return returnDown;
}

//function to call for intializing thread downloads
//This just calls other functions for the factory
void* startDownload(void* d) {
    downloadFactory* factory = (downloadFactory*)d;
    //create and connect to given ip at port
    factory->socketD = connectSocket(factory->port, factory->ip);
    backgroundDownload(factory);
    return NULL;
}

//function for connect sockets for individual download threads
/*
This function gets the host addressm connects to
the socket and returns the id for the download 
thread to request data from.
*/
int connectSocket(int p, char* addr) {
    int socketD, val;
    struct sockaddr_in serverAddr;
    struct hostent* hostAddr;

    //set up server socket
    socketD = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(p);

    //set up host
    hostAddr = gethostbyname(addr);
    memcpy(&serverAddr.sin_addr, hostAddr->h_addr, hostAddr->h_length);

    //finally connect to socket
    val = connect(socketD, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    return socketD;
}

//function for executing background download of other threads
//this function actually requests the block then writes to 
//the file
void backgroundDownload(downloadFactory* f) {
    //initialize values
    int tempIndex = f->index;
    int reqBlock = tempIndex;
    int factoryCount = f->downloadInstance->threadCount;
    int blockCount = 0;

    //request blcok loop
    while (f->validSocket) {
        ssize_t receiveCount = requestBlock(reqBlock, f);
        if (receiveCount <= 0) { //check if receive worked
            break;
        }

        writeBlockToFile(reqBlock, receiveCount, f);
        reqBlock += factoryCount;
        blockCount++;
    }
 
}

//block for requesting block from server
/*
This function allocates a string buffer and sends
a request to the socket for the next block into
that buffer array and then call connectSocket again
to begin getting the next block. Lastly it returns
the amount of bytes received on this request.
*/
ssize_t requestBlock(int b, downloadFactory* f) {
    char stringBuff[128];
    int length = sprintf(stringBuff, "%d", b);
    int sendRequest = send(f->socketD, stringBuff, 128, 0);

    ssize_t receivedBytes = recv(f->socketD, f->buff, BLOCK_SIZE, 0);
    close(f->socketD);

    //Check received byte count if it worked
    if (receivedBytes > 0) {
        f->socketD = connectSocket(f->port, f->ip);
    } else { //invalid socket
        f->validSocket = 0;
    }
    return receivedBytes;
}

//Function for taking a given block and outputting to a buffer
/*
This function simply takes a block and outputs it to 
the respective threads buffer output.  After locking
the thread it moves the file pointer to where it needs
to write to then continually dumps it into the file.
it then flushes the buffer and unlocks the thread.
*/
void writeBlockToFile(int bIndex, ssize_t writeBytes, downloadFactory* f) {
    //need an offset in file to write to
    long int fileOffset = (bIndex)*BLOCK_SIZE;
    
    //Calcultae offset from file
    FILE* fp = f->downloadInstance->output;
    fseek(fp, 0, SEEK_END);
    long int byteOffset = fileOffset - ftell(fp);
    fseek(fp, 0, SEEK_SET);

    //loop for writing values
    while (byteOffset > 0) {
        int writeFileBytes = byteOffset < f->downloadInstance->buffSize ? byteOffset : f->downloadInstance->buffSize;
        byteOffset -= fwrite(f->downloadInstance->buff, 1, writeFileBytes, fp);
    }

    //Finish pointer adjustments before closing file
    fseek(fp, fileOffset, SEEK_SET);
    fwrite(f->buff, 1, writeBytes, fp);
    fflush(fp);
}

//function to run a download thread
/*
This funtion takes a download object and
starts the download threads waiting for 
each other to complete blocks coming down.
Once the file write is complete it reports
the total amount written to the file.
*/
void runDownload(download* d) {
    printf("Running download\n");
    //write to file
    FILE* fp = d->output;
    fseek(fp, 0, SEEK_END);
    long int location = ftell(fp);
    printf("Write %d bytes to file\n", location);
}

//function to end threads and free allocated memory
/*
This function goes through each download in the
download factory and frees the allocated buffer
memory and destroys the mutexes and cond.
*/
void destroyDownload(download* d) {
    //iterate through threads
    int i;
    for (i = 0; i < d->threadCount; i++) {
        downloadFactory* tempFactory = &(d->downloaders[i]);
        //free memory
        free(tempFactory->ip);
        free(tempFactory->buff);
    }

    //Now free download data
    free(d->downloaders);
    free(d->buff);
    free(d);
}

