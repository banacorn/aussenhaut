// #include "type.h"
#include "Cello.h"
#include "network.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

static int Socket_Show(var self, var output, int pos) {
    struct Socket* socket = self;
    return print_to(output, pos,
        "[ in %d, out %d, err %d ]",
        $I(socket -> sin), $I(socket -> sout), $I(socket -> serr));
}

var Socket = Cello(Socket,
    Instance(Show, Socket_Show, NULL));

var read_message()
{
    char buffer[LINEBUFSIZE];
    ssize_t recv_result;
    int offset = 0;
    while (1) {
        ssize_t recv_result = recv(0, buffer + offset, LINEBUFSIZE, 0);
        if (recv_result < 0) {
            perror("recv error");
            return $S("");
        } else {
            if (buffer[offset + recv_result - 1] == '\n') { // newline, end of command
                buffer[offset + recv_result] = 0;
                return $S(buffer);
            } else {
                offset += recv_result;
            }
        }
    }
}

// void send_message(struct Socket* socket, var message_)
void send_message(var message_)
{
    struct String* message = cast(message_, String);
    int send_result = send(1, c_str(message), len(message), 0);
    // int send_result = send(socket->sout, c_str(message), len(message), 0);
    if (send_result < 0) {
        perror("sending message");
    }
}


void create_server(struct Int* port_number, struct Function* child_process)
{
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    // initialize server address
    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(c_int(port_number));

    // opening socket
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("opening stream socket");
    }

    // configure socket to SO_REUSEADDR
    int true_ = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &true_, sizeof(int));


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
        int client_socket_fd_in = accept(socket_fd, (struct sockaddr *) &client_address, &client_length);
        int client_socket_fd_out = dup(client_socket_fd_in);
        if (client_socket_fd_in < 0) {
            perror("accepting socket");
        }
        if (client_socket_fd_out < 0) {
            perror("duping socket");
        }

        // forking child processes
        pid_t child_pid = fork();
        if (child_pid < 0) {
            perror("fork error");
        } else if (child_pid == 0) {    //  child process
            close(socket_fd);
            call(child_process, $(Socket, client_socket_fd_in, client_socket_fd_out, 2));
            exit(EXIT_SUCCESS);
        } else {                        //  parent process
            close(client_socket_fd_in);
            close(client_socket_fd_out);
        }
    }

    close(socket_fd);
}
