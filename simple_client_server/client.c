/*
 * usage: tcpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>

#define BUFSIZE 512

int main(int argc, char **argv)
{
    int sockfd, portno, n, string_size;
    struct sockaddr_in serveraddr;
    struct hostent *server_host;
    char *hostname;
    char buf[BUFSIZE];
    char client_send_str[BUFSIZE];
    char server_string[BUFSIZE];
    bool connected = true;
    char end_of_data = '&';
    
    /* check command line arguments */
    if (argc != 3)
    {
        fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);
    
    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0)
    {
        fprintf(stderr, "Failed to create socket: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    /* gethostbyname: get the server's DNS entry */
    server_host = gethostbyname(hostname);
    
    if (server_host == NULL)
    {
        herror("While calling gethostbyname()");
        exit(EXIT_FAILURE);
    }
    
    /* Build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server_host->h_addr,
          (char *)&serveraddr.sin_addr.s_addr, server_host->h_length);
    serveraddr.sin_port = htons(portno);
    
    /* connect: Create a connection with the server */
    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        fprintf(stderr, "Failed to connect to client: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    
    n = read(sockfd, server_string, BUFSIZE);
    fprintf(stderr,"<server msg>%s<end server msg>\n",
            server_string);
    
    while(connected)
    {
        /* get message from the user */
        fgets(client_send_str, BUFSIZE, stdin);
        string_size = strlen(client_send_str); /* was +1 */
        
        /* send the message line to the server */
        n = write(sockfd, client_send_str, string_size);
        
        if(n<0)
        {
            fprintf(stderr, "Failed to write to socket: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* If termination character entered, print the server's reply */
        if (client_send_str[string_size-2] == end_of_data)
        {
            n = read(sockfd, server_string, BUFSIZE);
            
            if(n<0) // if error flag, report and exit
            {
                fprintf(stderr, "Failed to read from server sd buffer: %s",
                        strerror(errno));
                exit(EXIT_FAILURE);
            }

            /* This would sever connection; program is now useless */
            if (client_send_str[0] == 'X' && client_send_str[1] == end_of_data)
                break;
            
            fprintf(stderr,"<server msg>\n%s\n<end server msg>\n",
                    server_string);
            
        }
        memset(client_send_str, 0, sizeof(client_send_str));
        memset(server_string, 0, sizeof(server_string));
    }
    
    close(sockfd);
    printf("Client has terminated successfully\n");
    return 0;
}

