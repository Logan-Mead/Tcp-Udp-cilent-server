/* 
 * Code to start Socket Programming in IECE/ICSI 416
 * Instructor: Prof. Dola Saha
 *
 * tcp_server.c - A simple TCP echo server 
 *
 * Compile: gcc tcp_server.c -o tcp_server
 * 
 * usage: tcp_server <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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
  int parentfd; /* parent socket */
  int childfd; /* child socket */
  int portno; /* port to listen on */
  socklen_t clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buf[BUFSIZE]; /* message buffer */
  char specialChar[1] = "&";  //creates special character to be attached to end of file
  char *hostaddrp; /* dotted decimal host addr string */
  char *inputFile;  //file with text on it
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */

  /* 
   * check command line arguments 
   */
  if (argc != 3) {
    fprintf(stderr, "usage: %s <port> <inputFile>\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);
  inputFile = argv[2];            //grabs 3rd input argument

  /* 
   * socket: create the parent socket 
   */
  parentfd = socket(AF_INET, SOCK_STREAM, 0);
  if (parentfd < 0) 
    error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
  optval = 1;
  setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, 
	     (const void *)&optval , sizeof(int));

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));

  /* this is an Internet address */
  serveraddr.sin_family = AF_INET;

  /* let the system figure out our IP address */
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

  /* this is the port we will listen on */
  serveraddr.sin_port = htons((unsigned short)portno);

  /* 
   * bind: associate the parent socket with a port 
   */
  if (bind(parentfd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0) 
    error("ERROR on binding");

  /*
   * Needed for TCP
   * listen: make this socket ready to accept connection requests 
   */
  if (listen(parentfd, 5) < 0) /* allow 5 requests to queue up */ 
    error("ERROR on listen");

  /* 
   * main loop: wait for a connection request, echo input line, 
   * then close connection.
   */
  clientlen = sizeof(clientaddr);
  while (1) {

    /* 
     * accept: wait for a connection request 
     */
    childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
    if (childfd < 0) 
      error("ERROR on accept");
    
    /* 
     * gethostbyaddr: determine who sent the message 
     */
    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
			  sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    if (hostp == NULL)
      error("ERROR on gethostbyaddr");
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
      error("ERROR on inet_ntoa\n");
    printf("server established connection with %s (%s)\n", 
	   hostp->h_name, hostaddrp);

//---------------------------------------------------------------------------------------------
    
    /* 
     * read: read input string from the client
     */
    bzero(buf, BUFSIZE);
    n = read(childfd, buf, BUFSIZE);
    if (n < 0) 
      error("ERROR reading from socket");
    //printf("server received %d bytes: %s", n, buf);
    
    if(strcmp(buf, inputFile) != 0){
      error("file naming not the same");
    }

    FILE* serverFile = fopen(inputFile, "r");           //open file as read only and set to serverFile
    if(serverFile == NULL){                             //throw error if file is null/empty
      error("file is null");
    }

    while(fgets(buf, BUFSIZE, serverFile) != 0){      //runs loop until end of file
        printf("%s", buf);                            //prints line of file
        n = write(childfd, buf, BUFSIZE);             //writes line to buf
        if (n < 0){                                  
          error("ERROR writing to socket");
        }  
    }

//---------------------------------------------------------------------------------------------    

//---------------------------------------------------------------------------------------------    
   
    /* 
     * write: echo the input string back to the client 
     */

//---------------------------------------------------------------------------------------------
    fclose(serverFile);                                     //close serverFile
    n = write(childfd, specialChar, strlen(specialChar));   //writes special charachter, &, at end of file
    if (n < 0) 
      error("ERROR writing to socket");
    close(childfd);
  }
  close(parentfd);
}
