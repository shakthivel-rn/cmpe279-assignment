// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    pid_t pid;
    struct passwd* pwd;
    char *user = "nobody";

    if (argc == 1) 
    {
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
	    &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( PORT );

        if (bind(server_fd, (struct sockaddr *)&address,
	    sizeof(address)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }

        pid = fork();

        if (pid > 0)
        {
            printf("In Parent Process Waiting For Child Process \n");
            wait(&pid);
            exit(0);
        }
        else if (pid == 0) 
        {
            printf("In Forked Child Process \n");
            if((pwd = getpwnam(user)) == NULL) {
                perror("UID for nobody not found");
                exit(EXIT_FAILURE);
            }
            setuid(pwd->pw_uid);

            char *fname = "./server";
            char *arg[3];
            char targ[10];

            sprintf(targ, "%d", server_fd);

            arg[0] = fname;
            arg[1] = targ;
            arg[2] = NULL;

            execvp(fname, arg);
        }
        
    }

    printf("New Execed Child Running \n");
    server_fd = atoi(argv[1]);

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    valread = read(new_socket, buffer, 1024);
    printf("Read %d bytes: %s\n", valread, buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    return 0;
}
