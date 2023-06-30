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
#include <sys/stat.h>

int BUF_SIZE = 100000;

int main(int argc, char *argv[])
{
    int file_fd, sock_fd;
    ssize_t now_sent;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_addr;
    struct stat fileStat;
    unsigned int N, total_sent, not_written;
    char *serv_ip, *serv_port, *N_str;

    if (argc != 4)
    {
        perror("Invalid amount of arguments\n");
        exit(1);
    }

    /*Opening file and getting its length*/
    fd = open(argv[3], O_RDONLY);
    if (fd < 0)
    {
        perror("Failed to open file\n");
        exit(1);
    }

    if (stat(filename, &fileStat) == -1)
    {
        perror("Error getting file status");
        exit(1);
    }

    N = fileStat.st_size;
    /*TODO: can we assume N > 0 ? */

    /*Creating socket*/
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("Could not create socket \n");
        exit(1);
    }

    /*Handlig server adress*/
    serv_port = argv[2];
    serv_ip = argv[3];
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(serv_port));
    if (inet_pton(AF_INET, serv_ip, &(serv_addr.sin_addr.s_addr)) < 1)
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

    /*Writing to the server*/

    /*Sending N - the file length*/
    N_str = (char *)(&htonl(N));
    not_written = 4;
    total_sent = 0;
    while (not_written > 0)
    {
        now_sent = write(sock_fd, N_str + total_sent, not_written);
        if (now_sent <= 0)
        {
            perror("Failed to send N\n");
            exit(1);
        }
        total_sent += now_sent;
        not_written -= now_sent;
    }

    /*Sending the file's data*/

    not_written = N;
    total_sent = 0;

    return 0;
}
