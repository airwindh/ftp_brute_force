/*
 * DATE: 04/11/2015
 *******************************************************************************************
 * DESCRIPTION: SAMPLE PROGRAM SHELL FOR FTP INTERACTION
 * NOTES: THIS PROGRAM HAS BEEN WRITTEN TO COMPILE AND RUN USING BOTH WINSOCK AND POSIX.
 * YOU MUST USE POSIX.
 *******************************************************************************************
 *------------------------------------------------------------------------------------------
 * MODIFIED COMMENTS TO REFLECT CHANGE REQUIREMENTS
 * ADDED POSIX AND WINSOCK COMPATABILITY
 *******************************************************************************************
 */

#include <stdio.h>      											/* for printf() and fprintf() */
#ifdef _WIN32
#include <winsock2.h>                                               /* for winsock 2 library which replace socket.h and inet.h */
#include <ws2tcpip.h>                                               /* for defined constants */
                                                                    /* ADD libws2_32.a to the 'linker settings' in 'build options' for Windows Compilation */
#else
#include <sys/socket.h> 											/* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  											/* for sockaddr_in and inet_addr() */
#endif
#include <stdlib.h>       										    /* for atoi() and exit() */
#include <unistd.h>     											/* for close() */
#include "Utils.h"
#include "HandleTCPServer.h"

#define TITLE "FTP Sockets COMPLETE"
#define AUTHOR "Arvindh"
#define DATE "07/09/2015"
#define VERSION "0.0.0"

#define BUFSIZE 4096   												/* Size of receive buffer */

void header() {
    printf("============================================\n");
    printf("%s: version %s\n", TITLE, VERSION);
    printf("--------------------------------------------\n");
    printf("Author: %s (c) %s\n",AUTHOR, DATE);
    printf("============================================\n");
};

int main(int argc, char *argv[])
{
    #ifdef _WIN32
    WSADATA info;
    if (WSAStartup(MAKEWORD(1,1), &info) != 0)
    {
      fprintf(stderr,
          "Could not find suitable windows socket dll\n");
      exit(1);  // or some other suitable exit code
    }
    #endif // _WIN32WSADATA info;
    int sock;                        							    /* Socket descriptor */
    unsigned short servPort;      							        /* Echo server port */
    char *servIP;                     							    /* Server IP address (dotted quad) */

    if ((argc < 4) || (argc > 5))     							    /* Test for correct number of arguments */
    {
       fprintf(stderr, "Usage: %s <Server IP> <username-file> <password-file> [<Port>]\n",argv[0]);
       exit(1);
    }

    header();

    servIP = argv[1];                  						    /* First arg: server IP address (dotted quad) */

    if (argc == 5)
        servPort = atoi(argv[4]); 							    /* Use given port, if any */
    else
        servPort = 99;             							    /* Default to 99 */

    printf(" == Connecting to: %s on %d ==\n", servIP, servPort);
    char userdir[100],passdir[100];
    char temp[100];
    strcpy(temp,"find /home -name \"");
    strcat(temp,argv[2]);
    strcat(temp,"\" > temp.txt 2>/dev/null");
    system(temp);
    FILE *fptemp = fopen("temp.txt","r");
    memset(temp,0,strlen(temp));
    fscanf(fptemp,"%s",temp);
    strcpy(userdir,temp);
    fclose(fptemp);
    strcpy(temp,"find /home -name \"");
    strcat(temp,argv[3]);
    strcat(temp,"\" > temp.txt 2>/dev/null");
    system(temp);
    fptemp = fopen("temp.txt","r");
    memset(temp,0,strlen(temp));
    fscanf(fptemp,"%s",temp);
    strcpy(passdir,temp);
    fclose(fptemp);
    sock = createAndConnectSocket(servIP, servPort);
    HandleTCPServer(sock,servIP,servPort,userdir,passdir);


    printf(" == FTP Client End. ==\n");
    close(sock);

    return 0;
}




