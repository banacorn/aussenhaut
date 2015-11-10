#include "net.h"
#include "parser.h"
#include "type.h"

#define LINEBUFSIZE 16384
#define CMDBUFSIZE 512

String_ * read_message(int socket)
{
    char buffer[LINEBUFSIZE];
    ssize_t recv_result = recv(socket, buffer, LINEBUFSIZE, 0);
    if (recv_result < 0) {
        perror("recv error");
        return string("");
    } else {
        buffer[recv_result - 2] = 0;
        return string(buffer);
    }
}

void send_message(int socket, String_ * message)
{
    int send_result = send(socket, message -> content, string_length(message), 0);
    free_string(message);
    if (send_result < 0) {
        perror("sending message");
    }
}

void create_server(int port_number, void (*callback)(int))
{
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    // initialize server address
    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_number);

    // opening socket
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("opening stream socket");
    }

    // configure socket to SO_REUSEADDR
    int true = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int));


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

    // non-stop accepting
    while (1) {
        socklen_t client_length = sizeof(client_address);
        int client_socket_fd = accept(socket_fd, (struct sockaddr *) &client_address, &client_length);
        if (client_socket_fd < 0) {
            perror("accepting socket");
        }


        // forking child processes
        pid_t child_pid = fork();
        if (child_pid < 0) {
            perror("fork error");
        } else if (child_pid == 0) {    //  child process
            close(socket_fd);
            callback(client_socket_fd);
            exit(EXIT_SUCCESS);
        } else {                        //  parent process
            close(client_socket_fd);
        }
    }

    close(socket_fd);
}
