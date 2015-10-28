#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <strings.h>

#include "util.h"
#include "type.h"

#define LINEBUFSIZE 16384
#define CMDBUFSIZE 512
const char * welcome_msg =
    "****************************************\n"
    "** Welcome to the information server. **\n"
    "****************************************\n";

String * read_message(int socket)
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

void send_message(int socket, String * message)
{
    int send_result = send(socket, message -> content, string_length(message), 0);
    free_string(message);
    if (send_result < 0) {
        perror("sending message");
    }
}

void child(int socket)
{
    send_message(socket, string((char *)welcome_msg));

    while (1) {
        send_message(socket, string("% "));
        // Line * line = parse_line(read_message(socket));
        // print_line(line);
        // if (null_cmd(line -> cmds)) {   // empty command
        //     free_line(line);
        // } else {
        //     if (strcmp("exit", line -> cmds -> data -> name -> content) == 0) {
        //         free_line(line);
        //         break;
        //     } else if (strcmp("printenv", line -> cmds -> data -> name -> content) == 0) {
        //         // printf("%s\n", );
        //         free_line(line);
        //     }
        // }
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


int main(int argc, char *argv[])
{
    // String * s = string("a | b | d | c |3 d");
    // String * t = string(" | ");
    // List * l = tokenize(s, t);
    // print_list(l);
    //
    // String * a = head(l));
    // print_string(a);
    // free_string(a);


    // free_list(l);


    // Pair * l = rsplit(s, t);
    // print_pair(l);
    // free_pair(l);

    // String * s = string("a");
    // Command * c = parse_command(s);
    // print_command(c);
    // free_command(c);


    String * s = string("a b | c d | sdae |2");
    Line * c = parse_line(s);

    printf("=== parsed line ===\n");
    print_line(c);
    printf("\n");
    free_line(c);

    // String * t = string(" | ");
    // Box * l = last(tokenize(s, t));
    // print(l);
    // destruct(l);
    //
    // List * l = tokenize(s, t);
    // print_list(l);
    // free_list(l);

    // create_server(7000, child);

    // List * l = cons(box_chars("Apple"), cons(box_chars("Orange"), nil()));
    // List * g = copy_list(l);
    // print_list(l);
    // print_list(g);
    // free_list(l);
    // free_list(g);

    // Box * s = box_chars("hey");
    // print(s);
    // Box * t = copy(s);
    // print(t);
    // destruct(s);
    // destruct(t);

    // Line * l = parse_line(string(""));
    // print_line(l);
    // free_line(l);
    return 0;
}
