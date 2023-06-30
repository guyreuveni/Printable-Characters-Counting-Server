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
#include <fcntl.h>
#include <sys/stat.h>

ssize_t BUF_SIZE = 100000;

int main(int argc, char *argv[])
{
    int file_fd, sock_fd;
    ssize_t now_sent, not_written_inner, now_read;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_addr;
    struct stat fileStat;
    unsigned int N, N_network, total_sent, not_written, total_sent_inner, not_read, total_read, C_network, C;
    char *serv_ip, *serv_port, *file_name;

    if (argc != 4)
    {
        perror("Invalid amount of arguments\n");
        exit(1);
    }

    /*Opening file and getting its length*/
    file_name = argv[3];
    file_fd = open(file_name, O_RDONLY);
    if (file_fd < 0)
    {
        perror("Failed to open file\n");
        exit(1);
    }

    if (stat(file_name, &fileStat) < 0)
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
    serv_ip = argv[1];
    serv_port = argv[2];
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
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
    N_network = htonl(N);
    not_written = sizeof(unsigned int);
    total_sent = 0;
    while (not_written > 0)
    {
        now_sent = write(sock_fd, ((char *)(&N_network)) + total_sent, not_written);
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

    while (not_written > 0)
    {
        not_written_inner = read(file_fd, buf, BUF_SIZE);
        if (not_written_inner < 0)
        {
            perror("Failed reading from file\n");
            exit(1);
        }
        total_sent_inner = 0;

        while (not_written_inner > 0)
        {
            now_sent = write(sock_fd, buf + total_sent_inner, not_written_inner);
            if (now_sent < 0)
            {
                perror("Failed to send data\n");
                exit(1);
            }
            total_sent_inner += now_sent;
            not_written_inner -= now_sent;
            total_sent += now_sent;
            not_written -= now_sent;
        }
    }

    /*Closing file*/

    if (close(file_fd) < 0)
    {
        perror("Failed to close file\n");
        exit(1);
    }

    /*Getting from the server the number of printable chars*/

    not_read = sizeof(unsigned int);
    total_read = 0;

    while (not_read > 0)
    {
        now_read = read(sock_fd, ((char *)(&C_network)) + total_read, not_read);
        if (now_read < 0)
        {
            perror("Failed to read C\n");
            exit(1);
        }
        total_read += now_read;
        not_read -= now_read;
    }

    /*Closing socket*/
    if (close(sock_fd) < 0)
    {
        perror("Failed to close socket\n");
        exit(1);
    }

    C = ntohl(C_network);
    printf("# of printable characters: %u\n", C);
    exit(0);
}
