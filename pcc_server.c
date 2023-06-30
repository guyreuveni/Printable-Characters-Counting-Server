#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

ssize_t BUF_SIZE = 100000;
int client_is_being_proccesed = 0;
int its_time_to_terminate = 0;
int printable_counter_is_initialized = 0;
unsigned int printable_counter[127];

void init_printable_counter()
{
    int i;
    for (i = 0; i <= 126; i++)
    {
        printable_counter[i] = 0;
    }
    printable_counter_is_initialized = 1;
}

void terminate()
{
    int i;

    if (printable_counter_is_initialized == 1)
    {
        for (i = 32; i <= 126; i++)
        {
            printf("char '%c' : %u times\n", i, printable_counter[i]);
        }
    }
    exit(0);
}

void handle_sigint(int sig)
{
    if (client_is_being_proccesed == 0)
    {
        terminate();
    }
    its_time_to_terminate = 1;
}

int main(int argc, char const *argv[])
{
    struct sigaction sa_sigint;
    int listening_sock_fd, con_fd, i;
    struct sockaddr_in serv_addr, peer_addr;
    char *serv_port;
    unsigned int pcc_counter, N, N_network, not_read, total_read;
    unsigned int curr_client_printable_counter[127];
    ssize_t now_read;
    char buf[BUF_SIZE];

    socklen_t addrsize = sizeof(struct sockaddr_in);

    if (argc != 2)
    {
        perror("Invalid amount of arguments\n");
        exit(1);
    }

    /*setting the signal handler for sigint*/
    sa_sigint.sa_handler = &handle_sigint;
    sigemptyset(&sa_sigint.sa_mask);
    sa_sigint.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &sa_sigint, 0) < 0)
    {
        perror("Failed to set signal handler");
        exit(1);
    }

    /*Initializing printable counter*/
    init_printable_counter();

    /*Creating listening socket*/
    listening_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_sock_fd < 0)
    {
        perror("Failed to create the listening socket\n");
        exit(1);
    }

    /*Setting up the server adress*/
    serv_port = argc[1];
    memset(&serv_addr, 0, addrsize);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(serv_port));

    /*Binding the listening socekt to the port*/
    if (bind(listening_sock_fd, (struct sockaddr *)&serv_addr, addrsize) != 0)
    {
        perror("Failed to bind\n");
        exit(1);
    }

    /*Creating the listening queue for the listening socket*/
    if (listen(listening_sock_fd, 10) != 0)
    {
        perror("Failed to listen\n");
        exit(1);
    }

    /*Handling clients*/
    while (its_time_to_terminate == 0)
    {
        /*Accepting a new client*/
        con_fd = accept(listenfd, (struct sockaddr *)&peer_addr, &addrsize);
        if (con_fd < 0)
        {
            perror("Accept failed\n");
            exit(1);
        }
        client_is_being_proccesed = 1;
        pcc_counter = 0;
        for (i = 0; i <= 126; i++)
        {
            curr_client_printable_counter[i] = 0;
        }

        /*Reading N*/

        not_read = sizeof(unsigned int);
        total_read = 0;

        while (not_read > 0)
        {
            now_read = read(con_fd, ((char *)(&N_network)) + total_read, not_read);
            if (now_read < 0)
            {
                perror("Failed to read N\n");
                exit(1);
            }
            total_read += now_read;
            not_read -= now_read;
        }

        N = ntohl(N_network);

        /*Reading data from client*/
        total_read = 0;
        
        while (total_read < N){
            now_read = read(con_fd, buf, BUF_SIZE);
            if (now_read < 0)
            {
                perror("Failed to read N\n");
                exit(1);
            }
            total_read += now_read;
        }
    }

    terminate();
}
