#include <stdio.h>                                                  /* for perror() */
#include <stdlib.h>                                                 /* for exit() */
#ifdef _WIN32
#include <winsock2.h>                                               /* for winsock 2 library which replace socket.h and inet.h */
#include <ws2tcpip.h>                                               /* for defined constants */
                                                                    /* ADD ws2_32.a to the 'linker settings' in 'build options' for Windows Compilation */
#else
#include <sys/socket.h> 											/* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  											/* for sockaddr_in and inet_addr() */
#endif
#include <stdlib.h>     											/* for atoi() and exit() */
#include <string.h>     											/* for memset() */
#include <unistd.h>     											/* for close() */
#include "Utils.h"

/* Should have a procedure to allow buffered file storage rather than depend on a 4K Buffer */
typedef struct 
{
        char filp[11];
        int l;
        char userna[50];
        char userty[10];
        long size;
        char dat[50];
        char filena[50];
}filda;
int createAndConnectSocket(char * servIP, unsigned short servPort) {
    /* Create a reliable, stream socket using TCP */
    int sock = 0;
    struct sockaddr_in servAddr;  							        /* server address */

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        errorMessage("socket() failed");
    }

    /* Construct the server address structure */
    memset(&servAddr, 0, sizeof(servAddr));                         /* Zero out structure */
    servAddr.sin_family      = AF_INET;                             /* Internet address family */
    servAddr.sin_addr.s_addr = inet_addr(servIP);                   /* Server IP address */
    servAddr.sin_port        = htons(servPort);                     /* Server port */

    /* Establish the connection to the server */
    if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        errorMessage("connect() failed");
    }
    return sock;
};

/* Receive Message over command stream */
int receiveMessage(int sock, char *buf, int maxLen)
{
       int received = 0;
       int rv = 0;
       memset(buf,0,maxLen);

       rv = recv(sock, buf+received, 1, 0);
       while ((received < maxLen) && (rv > 0) && *(buf+received) != '\n')
       {
	      received += rv;
	      rv = recv(sock, buf+received, 1, 0);
       }
       if (rv < 0)
       {
	      errorMessage("recv() failed");
       }
       return received;
};

/* Send message over command stream */
void sendMessage(int sock, char *buf, int msgLen)
{
    if (send(sock, buf, msgLen, 0) != msgLen) {
        errorMessage("sendMessage Error");
    }
};

/* process PASV return string to get IP Address and Port number for Data Channel */
void processPASV(char * buf, char *addr, int * port) {
        int i,comcount=0,l=0,d;
        char temp[50];
        for(i=0;buf[i]!='(';i++);   //move pointer to first brace
        for(i=i+1;comcount<=3;i++)
        {
            if(buf[i]==',')         //replace comma with .
            {
                temp[l++]='.';
                comcount++;
                continue;
            }
            temp[l++] = buf[i];
        }
        temp[l-1]='\0';
        strcpy(addr,temp);          //addr now contains ip address of the server
        memset(temp,0,strlen(temp));  //empty temp
        l=0;
        for(;buf[i]!=')';i++)               //read the rest of the data to get port
        {
            if(buf[i]==',')
            {
               d = atoi(temp);
               memset(temp,0,strlen(temp));
               l=0;
               continue;
            }
            temp[l++]=buf[i];
        }
        d = (d*256)+ atoi(temp);
        *port = d;
};


/* Receive data from data stream */
int receiveData(int dataSocket, char * buffer, int maxSize) {
        int byteCount = 0;
        FILE *fpr = fopen("received.txt","w+");
        while(strlen(buffer)>0)
        {
             memset(buffer,0,maxSize);
             receiveMessage(dataSocket,buffer,sizeof(buffer));
             fprintf(fpr,"%s",buffer);
             byteCount+=sizeof(buffer);
        }
        rewind(fpr);
        memset(buffer,0,strlen(buffer));
            /* Get Data from the server, theres a 4K buffer there for this but you could use a temp file or dynamic memory */
    return byteCount;
};

/* Extract Return Code (First three characters) */
int returnCode(char * buf) {
    int i;
    char temp[4];
    for(i=0;i<3;i++)            //extract the first 3 characters of the message.
            temp[i]=buf[i];
    temp[i]='\0';
    int d = atoi(temp);
    return d; /* dummy value for compile!!!!!!!! */
};

void errorMessage(char * message) {
    fprintf(stderr, "ERROR: %s\n", message);
    exit(1);
};
