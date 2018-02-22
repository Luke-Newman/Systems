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
#include <ctype.h>
#include <stdbool.h>

#define SERVER_TCP_PORT 7858
#define BUF_LEN 512

void manage_connection(int, int);
int server_processing(char *instr, char *outstr);
void handle_sigcld(int);

int main()
{
    int rssd; /* rendezvous sd */
    int essd; /* empheral sd */
    
    int ec; /* stores error code */
    int client_len;
    int pid; /* process id of created child pid */
    struct sockaddr_in server,client; /* address structures for server and
                                                        connected clients */
    struct hostent *client_details; /* hostent structure for client name */
    struct sigaction cldsig; /* to handle SIGCHLD to prevent zombies
                              from totally taking over the system */
    
    bool listening;
    
    printf("\nH: Server is initializing...\n");
    
    cldsig.sa_handler=handle_sigcld;
    
    cldsig.sa_flags=SA_RESTART | SA_NOCLDSTOP;
    
    sigaction(SIGCHLD, &cldsig, NULL);
    
    rssd = socket(PF_INET, SOCK_STREAM, 0);
    
    
    if(rssd < 0) /* Failed to bind socket */
    {   /* using different out streams to stop processes jumbling up */
        fprintf(stderr, "Failed create rendezvous socket: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    printf("H:\tRenzdevous socket descriptor = %d\n\n", rssd);
    
    /** Binding address details to a socket **/
    memset(&server, 0, sizeof(server));
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=htonl(INADDR_ANY); /* describe this */
    server.sin_port=htons(SERVER_TCP_PORT); /* this too */
    
    if((ec=bind(rssd, (struct sockaddr *) &server, sizeof(server))) < 0)
    {
        fprintf(stderr, "Failed to bind socket: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    fprintf(stderr, "H:\tSocket has been bound successfully: it now has a\n"
           "H:\tlocal address so that other sockets may connect to it\n\n");
    
    /** Listening connections to socket to Server socket **/
    ec=listen(rssd, 5);
    
    if(ec < 0)
    {
        fprintf(stderr, "Failed to set message queue up: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    fprintf(stderr, "H:\tSocket has been marked as a passive and is listening\n"
           "H:\tfor incoming connection requests...\n\n");
    
    listening = true;
    
    while(listening)
    {
        client_len=sizeof(client);
        essd=accept(rssd, (struct sockaddr *) &client,
                    (socklen_t *) &client_len);
        
        if(essd < 0)
        {
            fprintf(stderr, "H:\tFailed to accept connection: %s",
                    strerror(errno));
            
            exit(EXIT_FAILURE);
        }
        
        /** get client details based off descriptor passed **/
        /* gethostbyaddr superseded by getaddrinfo? */
        client_details=gethostbyaddr((void *) &client.sin_addr.s_addr,
                                     4, AF_INET);
        
        if(client_details == NULL)
        {
            herror("H: While resolving client's address");
            exit(EXIT_FAILURE);
        }
        
        printf("H:\tAccepted a connection from %s on port %d with empheral sd:"
               "%d\n\n", client_details->h_name, ntohs(client.sin_port), essd);
        
        /** Creating child process to handle client **/
        if( (pid=fork()) == 0) /* if this is the child process */
        {
            close(rssd); /* close renzdevous sd */
            manage_connection(essd, essd); /* manage client connection */
        }
        else /* This is the parent process */
        {
            close(essd);
            printf("H:\tProcess with pid %d will service this client\n", pid);
        }
        
    }
    
    close(rssd); /* currently unreachable code */
    return 0;
}

void manage_connection(int in, int out)
{
    int rc,bc; /* read count and buffer count */
    char inbuf[BUF_LEN], outbuf[BUF_LEN],
    hostname[40]; /* buffers for vairous things */
    char prefix[100]; /* bit at start of output messages
                       to show which child */
    char end_of_data='&';
    int i;
    int revcnt;
    char revbuf[BUF_LEN];
    bool connected = true;
    /* get name of machine running the server */
    gethostname(hostname,40);
    sprintf(prefix,"\tChild with pid %d: ",getpid());
    fprintf(stderr,"\n%sstarting up\n",prefix);
    /* construct the announcment message and send it off */
    sprintf(outbuf,"\nConnected to Prac Server on host %s\n"\
            "Enter X as the first character to exit. \nOtherwise enter the "\
            "string to be case toggled.\n",hostname);
    write(out,outbuf,strlen(outbuf));
    
    while(connected)
    {
        bc = 0;
        
        while(1)
        {
            rc=read(in, inbuf+bc, BUF_LEN); // was inbuf+bc
            
            if(rc > 0)
            { /** TODO change to only use one buffer **/
               if( (bc + rc) > BUF_LEN)
               {
                   fprintf(stderr, "%s Error, received buffer size exceeded"\
                           "limit\n", prefix);
                   close(in);
                   exit(EXIT_SUCCESS);
               }
                
                fprintf(stderr, "%sHave read in:\n", prefix);
                
                /* displaying what was read */
                for(i = bc; i < bc + rc; i++) // i < bc + rc
                    printf("%s%d\t%c\n", prefix, inbuf[i], inbuf[i]);
                
                //memcpy(&inbuf[bc], inbuf, rc);
                bc=bc + rc;
                
                /* The index before the \r\n telnet ends with */
                if(inbuf[bc-2] == end_of_data) /* was -3 for telnet */
                    break;

            }
            else if (rc == 0)
            {
                fprintf(stderr, "\n%s Client has closed the connection\n\n"\
                       "H:\tClosing in-socket descriptor.\n", prefix);
                close(in);
                exit(EXIT_FAILURE);
            }
            else
            {
                sprintf(prefix,"Error While reading from connection");
                perror(prefix);
                close(in);
                exit(EXIT_FAILURE);
            }
        }
        
        inbuf[bc-1]='\0'; /* removing the &\r\n from the end of telnet */
        
        if(inbuf[bc-rc] == 'X' && inbuf[bc-rc+1] == end_of_data)
            break;
        
        revcnt=server_processing(inbuf, revbuf);
        /* send it back with a message and next prompt */
        sprintf(outbuf,"The server received %lu characters, which when the "\
                "case is toggled are:\n%s\n\nEnter next string: ",
                strlen(revbuf),revbuf);
        
        write(out,outbuf,strlen(outbuf));
    }
    
    fprintf(stderr,"\n%sClient has ended session, terminating...\n", prefix);
    close(in);
}


int server_processing(char *instr, char *outstr)
{
    int i, len;
    char c;
    len=strlen(instr);
    for(i=0;i<len;i++)
    {
        c=tolower(instr[i]);
        if(c==instr[i])
            outstr[i]=toupper(instr[i]);
        else
            outstr[i]=c;
    }
    outstr[len]='\0';
    return len;
}

/** The kernel calls this function **/
void handle_sigcld(int signo)
{
    pid_t child_pid = wait(NULL); /* only because I want the child pid */
    
    /* This ensures signal will not be missed */
    while(0<waitpid(-1,NULL,WNOHANG));
   
    printf("\nH:\tChild pid %d has had its exit status accounted for\n\n",
           child_pid);
}
























