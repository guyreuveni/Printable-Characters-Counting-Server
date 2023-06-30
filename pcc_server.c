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

int is_proccesing_client = 0;
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
    if (is_proccesing_client == 0)
    {
        terminate();
    }
    its_time_to_terminate = 1;
}

int main(int argc, char const *argv[])
{
    struct sigaction sa_sigint;
    int listening_sock_fd;
    struct sockaddr_in serv_addr;
    char *serv_port;
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
        perror("Failed to listen\n");
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

    while (its_time_to_terminate == 0)
    {
        /* code */
    }

    terminate();
}
