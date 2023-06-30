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
#include <errno.h>
#include <fcntl.h>

int BUF_SIZE = 1000000;

int main(int argc, char *argv[])
{
    int file_fd, sock_fd;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_addr;

    if (argc != 4)
    {
        perror("Invalid amount of arguments\n");
        exit(1);
    }

    fd = open(argv[3], O_RDONLY);
    if (fd < 0)
    {
        perror("Failed to open file\n");
        exit(1);
    }

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("Could not create socket \n");
        exit(1);
    }

    /*Handlig server adress*/
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &(serv_addr.sin_addr.s_addr)) < 1)
    {
        perror("Error translating IP adress \n");
        exit(1);
    }

    /*Creating connection to server*/

    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Failed to connect\n");
        exit(1);
    }

    return 0;
}
