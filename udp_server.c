/* 
 * Code to start Socket Programming in IECE/ICSI 516/416
 * Instructor: Prof. Dola Saha
 *
 * udp_server.c - A simple UDP echo server 
 *
 * Compile: gcc udp_server.c -o udp_server
 * 
 * usage: udp_server <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

/*
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char **argv) {
  int sockfd; /* socket */
  int portno; /* port to listen on */
  socklen_t clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buf[BUFSIZE]; /* message buf */
  char *hostaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */

  /* 
   * check command line arguments 
   */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);

  /* 
   * socket: create the parent socket 
   */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
  optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
	     (const void *)&optval , sizeof(int));

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portno);

  /* 
   * bind: associate the parent socket with a port 
   */
  if (bind(sockfd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0) 
    error("ERROR on binding");

  /* 
   * main loop: wait for a datagram, then echo it
   */
  clientlen = (socklen_t) sizeof(clientaddr);
  while (1) {

    /*
     * recvfrom: receive a UDP datagram from a client
     */

    //this part of code is buf recieved from client
    //-----------------------------------------------------------------------------------
    bzero(buf, BUFSIZE);
    n = recvfrom(sockfd, buf, BUFSIZE, 0,
		 (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0)
      error("ERROR in recvfrom");
    
    char clientMessage[100];      
    bzero(clientMessage, strlen(clientMessage));    //initializes clientMessage to all null
    strcpy(clientMessage, buf+13);                  //sets clientMessage to buff after 13 which is after "UDP Client 1 "
    bzero(buf, BUFSIZE);                            //sets buf to all null                   
    strcpy(buf, clientMessage);                     //sets buf to clientMessage

    //-----------------------------------------------------------------------------------

    /* 
     * gethostbyaddr: determine who sent the datagram
     */
    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
			  sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    if (hostp == NULL)
      error("ERROR on gethostbyaddr");
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
      error("ERROR on inet_ntoa\n");
    printf("server received datagram from %s (%s)\n", 
	   hostp->h_name, hostaddrp);
    printf("server received %ld/%ld bytes: %s\n", strlen(buf), strlen(buf), buf);
    for(int i = 0; i < strlen(buf); i++){           //uppercases buf
      buf[i] = toupper(buf[i]);
    }

    //---------------------------------------------------------------------------------------
    
    /* 
     * sendto: echo the input back to the client 
     */

    //this is what buf is sending to client
    //----------------------------------------------------------------------------------------

    char header[100] = "UDP SERVER ";                     //creates header to be but with message
    char tmp[100];                                         
    bzero(tmp, strlen(tmp));                              //creates tmp to all null
    strcat(tmp, header);                                  //concats tmp with header
    strcat(tmp, buf);                                     //concats tmp with buf, so tmp should be UDP SERVER message
    bzero(buf, BUFSIZE);                                  //sets buf to all null                         
    strcpy(buf, tmp);                                     //sets buf to tmp

    //------------------------------------------------------------------------------------------
    n = sendto(sockfd, buf, strlen(buf), 0, 
	       (struct sockaddr *) &clientaddr, clientlen);
    if (n < 0) 
      error("ERROR in sendto");
  }
  close(sockfd);
}


