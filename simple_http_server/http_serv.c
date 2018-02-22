#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

void manage_connection(int sockfd);
char* format_response(size_t *len, bool get_req);
void process_request(const char* request, bool *valid_req, bool *get_req, bool
        *valid_dir);
void handle_sigcld(int);

int main(int argc, char **argv)
{
        int rssd = 0; /* rendezvous sd */
        int essd = 0; /* empheral sd */
        int server_count = 0;
        int server_Port;
        char* server_name;

        int status; /* stores error code */
        int client_len;
        int pid; /* process id of created child pid */
        struct sockaddr_in server,client; /* address structures for server
                                             and connected clients */
        struct sigaction cldsig; /* to handle SIGCHLD to prevent zombies */

        if (argc != 3)
        {
                /* localhost would not work at all with telnet, curl, browser */
                fprintf(stderr,"usage: %s <ip_address> <port>\n", argv[0]);
                exit(EXIT_SUCCESS);
        }

        server_Port = atoi(argv[2]);
        server_name = argv[1];

        printf("\nServer is initializing, responds to 4 requests then exits\n");

        cldsig.sa_handler=handle_sigcld;
        cldsig.sa_flags=SA_RESTART | SA_NOCLDSTOP;
        sigaction(SIGCHLD, &cldsig, NULL);
        rssd = socket(AF_INET, SOCK_STREAM, 0);

        if(rssd < 0) /* Failed to bind socket */
        {   /* using different out streams to stop processes jumbling up */
                fprintf(stderr, "Failed create rendezvous socket: %s",
                        strerror(errno));
                exit(EXIT_FAILURE);
        }

        /* Binding address details to a socket */
        memset(&server, 0, sizeof(server));
        server.sin_family=AF_INET;
        server.sin_addr.s_addr=inet_addr(server_name);
        server.sin_port=htons(server_Port);

        if((status=bind(rssd, (struct sockaddr *) &server, sizeof(server))) < 0)
        {
                fprintf(stderr, "Failed to bind socket: %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        fprintf(stdout, "S:\tSocket has been bound successfully: it now has a\n"
                "S:\tlocal address so that other sockets may connect to it\n");

        fprintf(stdout, "S:\tServer Port: %d\n", ntohs(server.sin_port));

        /* Port marked as open by process and listening */
        status=listen(rssd, 1);

        if(status < 0)
        {
                fprintf(stderr, "Failed to set message queue up: %s",
                        strerror(errno));
                exit(EXIT_FAILURE);
        }

        fprintf(stdout, "S:\tSocket has been marked as a passive and is "
                "listening\nS:\tfor incoming connection requests...\n\n");

        while(1)
        {
                if (server_count == 4)
                {
                        /* let the child finish it's business */
                        wait(NULL);
                        printf("#############################################\n"
                               "Reached max responses, server shutting down..\n"
                             "#############################################\n");

                        /* trying to handle this more gracefully */
                        if (shutdown(rssd, SHUT_RDWR) < 0)
                        {
                                if (errno != ENOTCONN && errno != EINVAL)
                                        fprintf(stderr, "Shutdown err: %s",
                                                strerror(errno));
                        }

                        if (close(rssd) < 0)
                                fprintf(stderr, "Socket close err: %s",
                                        strerror(errno));

                        exit(EXIT_SUCCESS);
                }

                client_len=sizeof(client);
                essd=accept(rssd, (struct sockaddr *) &client,
                            (socklen_t *) &client_len);

                if(essd < 0)
                {
                        fprintf(stderr, "H:\tFailed to accept connection: %s",
                                strerror(errno));

                        exit(EXIT_FAILURE);
                }

                /* get client details based off descriptor passed */

                /* Creating child process to handle client */
                if( (pid=fork()) == 0) /* if this is the child process */
                {
                        /* close renzdevous sd */
                        manage_connection(essd); /* manage client connection */

                }
                else /* This is the parent process */
                {
                        printf("S:\tPid %d will service this client\n", pid);
                        server_count ++;
                }
        }
}

void manage_connection(int sockfd)
{
        char prefix[100]; /* to show which child */
        char client_request_string[BUFSIZE];
        char* request;
        ssize_t read_count = 0;
        int buf_count = 0;

        char* twinkle;
        char* response;
        char* status_str;
        size_t stat_len;
        size_t twinkle_len = 0;
        size_t template_len = 0;
        size_t response_len = 0;
        bool get_req = false, valid_req = false, valid_dir = true;

        const char* response_template =
                {
                        "HTTP/1.0 %s\r\nContent-Type: text/plain\r\n"
                        "Content-Length: %zu\r\n\r\n%s\r\n"
                };

        sprintf(prefix,"C:\tChild with pid %d: ",getpid());
        fprintf(stdout,"\n%s: starting up\n",prefix);

        size_t tmpBuf = BUFSIZE;
        request = malloc(tmpBuf);
        while ((read_count = read(sockfd, client_request_string, BUFSIZE)) != 0)
        {
                if (read_count + buf_count > tmpBuf)
                {
                        tmpBuf += BUFSIZE; /* was *= 2 */
                        char* tmp = realloc(request, tmpBuf);

                        if (tmp)
                        {
                                request = tmp;
                        }
                        else
                        {
                                /* there's a problem writing to buffer */
                                free(request);
                                break;
                        }
                }

                memcpy(request + buf_count, client_request_string, read_count);
                buf_count += read_count;
                bzero(client_request_string, BUFSIZE);


                if (strstr(request, "\r\n\r\n") != NULL)
                {
                        break;
                }

        }

        process_request(request, &valid_req, &get_req, &valid_dir);

        if (valid_req)
        {
                if (valid_dir) {
                        status_str = "200 OK";

                        stat_len = strlen(status_str);
                        template_len = strlen(response_template);
                        twinkle = format_response(&twinkle_len, get_req);
                        response_len = template_len + twinkle_len + stat_len+ 3;
                        response = calloc(sizeof(char), response_len);

                        snprintf(response, response_len, response_template,
                                 status_str,
                                 twinkle_len, twinkle);

                        fprintf(stdout, "C:\tresponse:\n%s\n", response);
                        write(sockfd, response, response_len);

                        if (get_req)
                        {
                                free(twinkle);
                        }

                        free(response);
                }
                else
                {
                        status_str = "HTTP/1.0 404 Not Found\r\n\r\n";
                        fprintf(stdout, "C:\tresponse:\n%s\n", status_str);
                        write(sockfd, status_str, strlen(status_str));
                }
        }
        else
        {
                status_str = "HTTP/1.0 501 Not Implemented\r\n\r\n";
                fprintf(stdout, "\nC:\tresponse:\n%s\n", status_str);
                write(sockfd, status_str, strlen(status_str));
        }


        free(request);
        fprintf(stdout,"%sResponse sent, child terminating...\n", prefix);

        if (shutdown(sockfd, SHUT_RDWR) < 0)
        {
                if (errno != ENOTCONN && errno != EINVAL)
                        fprintf(stderr, "Shutdown err: %s", strerror(errno));
        }

        if (close(sockfd) < 0)
                fprintf(stderr, "Socket close err: %s", strerror(errno));

        exit(EXIT_SUCCESS);
}

void process_request(const char* request, bool *valid_req, bool *get_req, bool
*valid_dir)
{
        size_t pos = 0;
        char request_type[16]; /* should be found by the time we reach end */

        fprintf(stdout, "C:\tRequest to child:\n%s\n", request);

        /* Did this way; GET, HEAD, / etc could be elsewhere in string */
        while (pos < 8)
        {
                if (request[pos] == ' ')
                {
                        break;
                }

                request_type[pos] = request[pos];
                pos++;
        }

        if (strcmp(request_type, "GET") == 0)
        {
                *valid_req = true;
                *get_req = true;
        } else if (strcmp(request_type, "HEAD") == 0)
        {
                *valid_req = true;
                *get_req = false;
        }

        if (pos > 1 && pos < 13 && *valid_req == true)
        {
                if ((request[pos + 1] != '/') || (request[pos + 2] != ' '))
                {
                        *valid_dir = false;
                }
        }
}


char* format_response(size_t *len, bool get_req)
{
        int num_lines = 20;
        size_t concat_len = 1, pos = 0; /* room for null terminator */
        int i;
        int r;
        char* twinkle_merged;

        srand((unsigned)time(NULL));

        /* I'm not counting spaces between paragraphs as a line */
        char* twinkle[] =
                {
                        "Twinkle, twinkle, little star,\n",
                        "How I wonder what you are!\n",
                        "Up above the world so high\n",
                        "Like a diamond in the sky.\n",

                        "When the blazing sun is gone,\n",
                        "When he nothing shines upon,\n",
                        "Then you show your little light,\n",
                        "Twinkle, twinkle, all the night.\n",

                        "Then the traveller in the dark\n",
                        "Thanks you for your tiny sparks;\n",
                        "He could not see which way to go,\n",
                        "If you did not twinkle so.\n",

                        "In the dark blue sky you keep,\n",
                        "And often through my curtains peep,\n",
                        "For you never shut your eye\n",
                        "Till the sun is in the sky.\n",

                        "As your bright and tiny spark\n",
                        "Lights the traveller in the dark,\n",
                        "Though I know not what you are,\n",
                        "Twinkle, twinkle, little star.\n"
                };

        r = rand() % num_lines;

        for (i = 0; i < r; i++)
        {
                concat_len += strlen(twinkle[i]);
        }

        /* I think calloc is best here, could calloc each time in one loop */
        twinkle_merged = calloc(sizeof(char), concat_len);

        for (i = 0; i < r; i++)
        {
                strcpy(twinkle_merged+pos, twinkle[i]);
                pos += strlen(twinkle[i]);
        }

        /* TODO remove... */
        /* Because we need to remove the +1 in concat_len for nul terminator */
        if (r == 0)
                *len = 0;
        else
                *len = concat_len;

        if (get_req)
        {
                return  twinkle_merged;
        }

        return "\0";
}

/* The kernel calls this function */
void handle_sigcld(int signo)
{
        pid_t child_pid = wait(NULL); /* only because I want the child pid */

        /* This ensures signal will not be missed */
        while(0<waitpid(-1,NULL,WNOHANG));

        printf("\nS:\tChild pid %d has had its exit status accounted for\n\n",
               child_pid);
}