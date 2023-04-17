/* 
 * udp_client.c - A simple UDP client
 * usage: udp_client <host> <port>
 */
 /* 
 * Code to start Socket Programming in IECE/ICSI 516/416
 * Instructor: Prof. Dola Saha
 *
 * udp_client.c - A simple UDP client 
 *
 * Compile: gcc udp_client.c -o udp_client
 * 
 * usage: udp_client <host> <port>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFSIZE 1024

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char **argv) {
    int sockfd, portno, n;
    socklen_t serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];

    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");


    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }
    
    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* get a message from the user */
    bzero(buf, BUFSIZE);
    printf("Please enter msg: ");
    fgets(buf, BUFSIZE, stdin);

//-------------------------------------------------------------------------------------------------------

    char header[100] = "UDP Client 1 ";             //creates header
    char tmp[100];
    bzero(tmp, strlen(tmp));                        //initializes tmp to all null                      
    strcat(tmp, header);                            //concats tmp and header
    strcat(tmp, buf);                               //concats tmp and buf so tmp should be UDP Client 1 message
    bzero(buf, BUFSIZE);                            //sets buff to all null                  
    strcpy(buf, tmp);                               //sets buf to tmp

    //sends to server UDP Client 1 message

    /* send the message to the server */
    serverlen = (socklen_t) sizeof(serveraddr);
    n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&serveraddr, serverlen);
    if (n < 0) 
      error("ERROR in sendto");

//---------------------------------------------------------------------------------------------------------

    /* print the server's reply */
    n = recvfrom(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&serveraddr, &serverlen);
    char clientMessage[100];
    bzero(clientMessage, strlen(clientMessage));                //initializes clientMessage and sets all to null                         
    for(int i = 0; buf[i+11] && buf[i+11] != '\n'; i++){        //removes anything after a new line in client message from buf, 11 is length of "UDP SERVER "
        clientMessage[i] = buf[i+11];
    }
    clientMessage[strlen(clientMessage)] = '\n';                //adds new line at end of message
    //strcpy(clientMessage, buf+11);                            //11 is length of "UDP SERVER "
    bzero(buf, BUFSIZE);                                        //sets buf to null                             
    strcpy(buf, clientMessage);                                 //sets buf to clientMessage from server

//-----------------------------------------------------------------------------------------------------------   

    if (n < 0) 
      error("ERROR in recvfrom");
    printf("Echo from server: %s", buf);
    
    close(sockfd);
    
    return 0;
}
