#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <strings.h>

#define  BUFFERSIZE 65536
#define  PORTNO     7000


void
create_server(int port_number, void (*callback)(void))
{

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    // initialize server address
    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORTNO);

    // opening socket
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("opening stream socket");
    }

    // binding socket
    int bind_result = bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (bind_result < 0) {
        perror("binding socket");
    }

    // listening socket
    int listen_result = listen(socket_fd, 0);
    if (listen_result < 0) {
        perror("listening socket");
    }

    // accepting
    while (1) {

        socklen_t client_length = sizeof(client_address);
        int client_socket_fd = accept(socket_fd, (struct sockaddr *) &client_address, &client_length);
        if (client_socket_fd < 0) {
            perror("accepting socket");
        }

        // forking child processes
        int child_pid = fork();
        if (child_pid < 0) {
            perror("fork error");
        } else if (child_pid == 0) {    //  child process
            close(socket_fd);
            callback();
            exit(EXIT_SUCCESS);
        } else {                        //  parent process
            // prevent deamon from accessing imcoming sockets
            close(client_socket_fd);
        }

    }
}

void
child()
{
    printf("haha\n");
}

int
main(int argc, char *argv[])
{
    create_server(7000, child);
    // struct sockaddr_in server_address;
    // struct sockaddr_in client_address;
    //
    // // initialize server address
    // bzero((char *) &server_address, sizeof(server_address));
    // server_address.sin_family = AF_INET;
    // server_address.sin_addr.s_addr = INADDR_ANY;
    // server_address.sin_port = htons(PORTNO);
    //
    // // opening socket
    // int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    // if (socket_fd < 0) {
    //     perror("opening stream socket");
    // } else {
    //     printf("socket fd: %d\n", socket_fd);
    // }
    //
    // // binding socket
    // int bind_result = bind(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    // if (bind_result < 0) {
    //     perror("binding socket");
    // }
    //
    // // listening socket
    // int listen_result = listen(socket_fd, 0);
    // if (listen_result < 0) {
    //     perror("listening socket");
    // }
    //
    // // accepting
    // socklen_t client_length;
    //
    // while (1) {
    //     client_length = sizeof(client_address);
    //     int new_socket_fd = accept(socket_fd, (struct sockaddr *) &client_address, &client_length);
    //     if (new_socket_fd < 0) {
    //         perror("accepting socket");
    //     } else {
    //         printf("new socket fd: %d\n", new_socket_fd);
    //     }
    //
    //     int child_pid = fork();
    //     char buffer[BUFFERSIZE];
    //     ssize_t recv_result;
    //     if (child_pid < 0) {
    //         perror("fork error");
    //     } else if (child_pid == 0) {    //  child process
    //         printf("incoming %d\n", child_pid);
    //
    //         // while (1) {
    //         //     recv_result = recv(new_socket_fd, buffer, sizeof(buffer), 0);
    //         //     printf("recv_result %zd\n", recv_result);
    //         //     if (recv_result == 0) {
    //         //         printf("closing new socket\n");
    //         //         close(new_socket_fd);
    //         //     } else if (recv_result < 0) {
    //         //         perror("recv error");
    //         //     } else {
    //         //
    //         //         // print to STDOUT
    //         //         // getline
    //         //         // int i = 0;
    //         //         // while (i != recv_result) {
    //         //         //     putchar(buffer[i]);
    //         //         //     i++;
    //         //         // }
    //         //
    //         //         // send back
    //         //         send(new_socket_fd, buffer, recv_result, 0);
    //         //     }
    //         // }
    //
    //         close(socket_fd);
    //
    //         exit(1);
    //     }
    //
    //     close(new_socket_fd);
    //     printf("out %d\n", child_pid);
    //
    // }



    return 0;
}
