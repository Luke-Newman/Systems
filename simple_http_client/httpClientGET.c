/* usage: website address */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>


#define BUFSIZE 1024

int main(int argc, char **argv)
{
        int sockfd, portno = 80, request_length = 0, read_count, flag_200 = 0;
        int buf_count = 0;
        size_t received_len = 0;
        char* body_buf;
        ssize_t nByte;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        char server_body_string[BUFSIZE];
        char request[BUFSIZE];
        char request_template[] = {
                "%s /index.html HTTP/1.1\r\nHost: %s\r\n\r\n"};
        char request_template_2[] = {
                "%s /index.html HTTP/1.1\r\nHost: %s\r\nConnection: "
                        "Close\r\n\r\n"};
        char *hostname = argv[1];

        /* Connection: Close allows me to not really on some specific end tag */

        /* check command line arguments */
        if (argc != 2)
        {
                fprintf(stderr, "usage: %s <website>\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        request_length = snprintf(request, BUFSIZE, request_template_2, "GET",
                                  hostname);

        if (request_length >= BUFSIZE)
        {
                fprintf(stderr, "Request length exceeds buffer size: %d\n",
                        request_length);
                exit(EXIT_FAILURE);
        }

        /* gethostbyname: get the server's DNS entry */
        server = gethostbyname(hostname);
        if (server == NULL)
        {
                herror("While calling gethostbyname()");
                exit(EXIT_FAILURE);
        }

        /* socket: create the socket */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0)
        {
                fprintf(stderr, "Failed to create socket: %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        /* Build the server's Internet address */
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;

        bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr,
              server->h_length);
        serv_addr.sin_port = htons(portno);

        /* connect: Create a connection with the server */
        if (connect(sockfd, (struct sockaddr *) &serv_addr,
                    sizeof(serv_addr)) < 0)
        {
                fprintf(stderr, "Failed to connect to client: %s",
                        strerror(errno));
                exit(EXIT_FAILURE);
        }
        else
        {
                printf("Connected to Server\n\n");
        }

        printf("\nGet Request:\n");
        nByte = write(sockfd, request, request_length);
        if (nByte <= 0) {
                fprintf(stderr, "Failed to send request: %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        size_t tmpBuf = BUFSIZE;
        body_buf = malloc(tmpBuf);
        while ((read_count = read(sockfd, server_body_string, BUFSIZE)) != 0)
        {
                /* was trying to make reading from buffer more robust with
                 snprintf() but didn't work properly */
                /* tried strtok and strstr to try find \r\n for body,
                 could never find it in server_body_str */
                if (read_count + buf_count > tmpBuf)
                {
                        tmpBuf *= 2;
                        char* tmp = realloc(body_buf, tmpBuf);

                        if (tmp)
                        {
                               body_buf = tmp;
                        }
                        else
                        {
                                /* Problem writing to buffer */
                                free(body_buf);
                                body_buf = 0;
                                break;
                        }
                }

                memcpy(body_buf + buf_count, server_body_string, read_count);
                buf_count += read_count;
                bzero(server_body_string, BUFSIZE);
        }


        /* Finding HTTP status code */
        received_len = strlen(body_buf);
        char* pch=strstr(body_buf, "HTTP/1.1");
        unsigned long pos=pch - body_buf; /* find pos via pointer subtraction */
        char status_string[3];
        int status_code = 0;
        int stat_pos = 0;
        pos += 9; /* get rid of "HTTP/1.1 " */
        if (pch != NULL)
        {
                while  (pos < received_len)
                {
                        if (body_buf[pos] == '\n')
                        {
                                status_code = atoi(status_string);
                                if (status_code == 200)
                                {
                                        flag_200 = 1;
                                }
                                break;
                        }

                        if (isdigit(body_buf[pos]))
                        {
                                status_string[stat_pos] = body_buf[pos];
                                stat_pos ++;
                        }
                        pos++;
                }
        }

        /* Printing body of request can print to stdout because the
         * socket stream isn't chasing anymore */
        pch=strstr(body_buf, "\r\n\r\n");

        if (pch != NULL && flag_200 == 1)
        {
                /* pch now points to new start point in buffer */
                printf("%s", pch);
        } else
        {
                printf("\nStatus Code: %d\n", status_code);
        }

        /* This is a problem but I'd rather not exit with failure
         * so I can free memory and close fd
         * */
        if (read_count == -1)
        {
                fprintf(stderr, "Failed to read anything from buffer: %s",
                        strerror(errno));
        }

        free(body_buf);
        close(sockfd);
        printf("\nClient has terminated successfully\n");
        return 0;
}
