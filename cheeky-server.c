#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "cheeky-server.h"

int main()
{
    int sockfd, new_fd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    unsigned int sin_size, addrlen;
    int numbytes;
    char *last_newline;
    char *last_line;
    int yes = 1;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 8);

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    while (1) {
        sin_size = sizeof(struct sockaddr_in);

        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
            perror("accept");
            continue;
        }

        if (!fork()) {
            if (send(new_fd, "ok\n", 14, 0) == -1) {
                perror("send");
            }

            char buff[BUFF_SIZE] = "\0";

            addrlen  = sizeof(struct sockaddr);
            numbytes = recvfrom(new_fd, buff, BUFF_SIZE, 0, (struct sockaddr *)&their_addr, &addrlen);

            if (strstr(buff, "POST / HTTP/1") != NULL) {
                last_line = strstr(buff, "\r\n\r\n") + 4;
                printf("write:%s\n", last_line);

                set_message(last_line);
            }

            close(new_fd);
            exit(0);
        }

        close(new_fd);

        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
}

void set_message(char *msg)
{
    key_t key;
    int shmid;
    char *data;

    key   = ftok(SHM_KEY, 'R');
    shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT);
    data  = shmat(shmid, (void *)0, 0);

    strncpy(data, msg, SHM_SIZE);
}
