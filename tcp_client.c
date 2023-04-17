 /* 
 * Code to start Socket Programming in IECE/ICSI 416
 * Instructor: Prof. Dola Saha
 *
 * tcp_client.c - A simple TCP client 
 *
 * Compile: gcc tcp_client.c -o tcp_client
 * 
 * usage: tcp_client <host> <port>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>

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
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char *inputFile;                    //inializes inputfile as a char
    char buf[BUFSIZE];
	struct timespec start, end;
	
    /* check command line arguments */
    if (argc != 4) {
       fprintf(stderr,"usage: %s <hostname> <port> <inputFile>\n", argv[0]);
       exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);
    inputFile = argv[3];                    //sets inputFile to 4th input argument

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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

    /* connect: create a connection with the server */
    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) 
      error("ERROR connecting");
      
//---------------------------------------------------------------------------------------------

    bzero(buf, BUFSIZE);
    /* send the message line to the server */
    n = write(sockfd, inputFile, strlen(inputFile));
    if (n < 0) 
      error("ERROR writing to socket");
    FILE* outputFile = fopen("clientOutput.txt", "w");          //opens clientOutput.txt as writable and sets to outputFile
    if(outputFile == NULL){
        error("file is null");
    }
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
    
    /* print the server's reply */
    while((n = read(sockfd, buf, BUFSIZE)) > 0){
        if (n < 0) 
      error("ERROR writing to socket");

        if(strcmp(buf,"&") == 0){                   //if buf is special character close socket and file and close progam
            fclose(outputFile);
            close(sockfd);
            return 0;
        }

        else{
            fprintf(outputFile, "%s", buf);         //writes to outputfile what buf is from server
        } 

    }
    
    
//---------------------------------------------------------------------------------------------
    bzero(buf, BUFSIZE);
    fclose(outputFile);                          //close outputFile
    close(sockfd);
    return 0;
}
