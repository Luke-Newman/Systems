/* uasge: website address */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 1024

int main(int argc, char **argv)
{
        int sockfd, portno = 80, request_length = 0, read_count;
        int buf_count = 0;
        char* head_buf = NULL;
        ssize_t nByte;
        struct sockaddr_in serv_addr;
        struct hostent *server;
        char server_head_string[BUFSIZE];
        char request[BUFSIZE];
        char request_template[] = {
                "%s /index.html HTTP/1.1\r\nHost: %s\r\nConnection: "
                        "Close\r\n\r\n"};
        char *hostname = argv[1];

        /* Connection: Close allows me to not rely on some specific end tag */

        /* check command line arguments */
        if (argc != 2)
        {
                fprintf(stderr, "usage: %s <website>\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        request_length = snprintf(request, BUFSIZE, request_template, "HEAD",
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

        /* I've decided to store the stream in a dynamically allocated buffer
         * because trying to get everything I wanted done while streaming was
         * a complete mess
         * */

        printf("\nHead Request:\n\n");
        nByte = write(sockfd, request, request_length);
        if (nByte <= 0) {
                fprintf(stderr, "Failed to send request: %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        size_t tmpBuf = BUFSIZE;
        head_buf = malloc(tmpBuf);
        while ((read_count = read(sockfd, server_head_string, BUFSIZE)) != 0)
        {
                /* was trying to make reading from buffer more robust with
                 snprintf() but didn't work properly */
                /* tried strtok and strstr to try find \r\n for body,
                 could never find it in server_body_str */
                if (read_count + buf_count > tmpBuf)
                {
                        tmpBuf *= 2;
                        char* tmp = realloc(head_buf, tmpBuf);

                        if (tmp)
                        {
                                head_buf = tmp;
                        }
                        else
                        {
                                /* Problem writing to buffer */
                                free(head_buf);
                                head_buf = 0;
                                break;
                        }
                }

                memcpy(head_buf + buf_count, server_head_string, read_count);
                buf_count += read_count;
                bzero(server_head_string, BUFSIZE);
        }

        /* Tried to make the following a function to save repetition but it
         * wasn't working properly */

        char* pch=strstr(head_buf, "HTTP/1.1");
        unsigned long pos=pch - head_buf; /* find pos via pointer
 * subtraction */
        char tmp_string[512];
        int i = 0;

        if (pch != NULL)
        {
                while  (pos < 512)
                {
                        if (head_buf[pos] == '\n')
                        {
                                tmp_string[i+1] = '\n';
                                printf("%s\n", tmp_string);
                                memset(tmp_string, 0, 512);
                                break;
                        }
                        tmp_string[i] = head_buf[pos];
                        i++;
                        pos++;
                }
        }

        pch=strstr(head_buf, "Content-Type");
        pos=pch - head_buf;
        i = 0;

        if (pch != NULL)
        {
                while  (pos < 512)
                {
                        if (head_buf[pos] == '\n')
                        {
                                tmp_string[i+1] = '\n';
                                printf("%s\n", tmp_string);
                                memset(tmp_string, 0, 512);
                                break;
                        }
                        tmp_string[i] = head_buf[pos];
                        i++;
                        pos++;
                }
        }

        pch=strstr(head_buf, "Last-Modified");
        pos=pch - head_buf;
        i = 0;

        if (pch != NULL)
        {
                while  (pos < 512)
                {
                        if (head_buf[pos] == '\n')
                        {
                                tmp_string[i+1] = '\n';
                                printf("%s\n", tmp_string);
                                memset(tmp_string, 0, 512);
                                break;
                        }
                        tmp_string[i] = head_buf[pos];
                        i++;
                        pos++;
                }
        }

        free(head_buf);
        close(sockfd);
        printf("\n\nClient has terminated successfully\n");
        return 0;
}
