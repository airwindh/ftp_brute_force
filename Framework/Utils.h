#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

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

int createAndConnectSocket(char * servIP, unsigned short servPort);
int receiveMessage(int s, char *buf, int maxLen);
int receiveData(int dataSocket, char * buffer, int maxSize);
int returnCode(char * buf);
void sendMessage(int sock, char *buf, int msgLen);
void processPASV(char * buf, char * addr, int * port);
void errorMessage(char * message);

#endif // UTILS_H_INCLUDED
