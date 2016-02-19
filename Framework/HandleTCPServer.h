#ifndef HANDLETCPSERVER_H_INCLUDED
#define HANDLETCPSERVER_H_INCLUDED

/**********************************************************************************************/
/* HUGE LIMITATION : FILE LIST MUST NOT EXCEED 4K                                             */
/* A BETTER SOLUTION WOULD BE A LINKED LIST TO STORE INTERNALLY MALLOC/FREE OR BUFFER TO DISK */
/**********************************************************************************************/
#define BUFSIZE 4096   											    /* Size of receive buffer */

char srvBuf[BUFSIZE];            							        /* Buffer for string */
char clntBuf[BUFSIZE];                 							    /* Buffer for string */
int port;                                                           /* port Number for PASV connection (DATA Channel) */
int msgSize;                                                        /* size of returned directory listing in chars */

void HandleTCPServer(int sock,char * servIP, unsigned short servPort,char * userdir,char * passdir);

#endif // HANDLETCPSERVER_H_INCLUDED
