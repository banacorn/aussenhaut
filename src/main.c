#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <strings.h>
#include "ds.h"

#define LINEBUFSIZE 16384
#define CMDBUFSIZE 512
const char * welcome_msg =
    "****************************************\n"
    "** Welcome to the information server. **\n"
    "****************************************\n";

// typedef struct Line {
//     List *  commands;
//     int     pipeTo;
// } Line;
//
// Line * alloc_line(List * commands, int pipeTo)
// {
//     Line * line = malloc(sizeof(Line));
//     line -> commands = copy_list(commands);
//     line -> pipeTo = pipeTo;
//     return line;
// }
//
// void free_line(Line * line)
// {
//     free_list(line -> commands);
//     free(line);
// }




char * read_message(int socket)
{
    char buffer[LINEBUFSIZE];
    char * result;
    int i = 0;
    ssize_t recv_result = recv(socket, buffer, LINEBUFSIZE, 0);
    if (recv_result < 0) {
        perror("recv error");
        result = (char *)malloc(1);
        result[0] = 0;
    } else {
        // remove /r/n and append /0
        buffer[recv_result - 2] = 0;
        result = (char *)malloc(recv_result - 1);
        strcpy(result, buffer);
    }
    return result;
}

void send_message(int socket, char * message)
{
    int send_result = send(socket, message, strlen(message), 0);
    if (send_result < 0) {
        perror("sending message");
    }
}

void child(int socket)
{
    send_message(socket, (char *)welcome_msg);

    while (1) {
        send_message(socket, "% ");
        char * message = read_message(socket);
        if (strcmp("exit", message) == 0) {
            free(message);
            break;
        } else {
            printf("%s\n", message);
        }
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
    // while (1) {

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
    // }

    close(socket_fd);
}



int main(int argc, char *argv[])
{
    // create_server(7000, child);
    // Command * c = parse_command(string("haha gasd hehe yo"));
    // print_command(c);
    // free_command(c);
    //
    // String * s = string("aa bb cc");
    // String * sub = substring(s, 0, 8);
    // print_string(sub);
    // free_string(s);
    // free_string(sub);


    Line * c = parse_line(string("haha gasd hehe yo | yes |2 3"));
    print_line(c);
    free_line(c);

    // ListStr * ls = nil_str();
    // ListStr * ls = cons_str(string("hey"), cons_str(string("haha"), nil_str()));
    // ListStr * ms = reverse_str(ls);
    // // ListStr * ms = cons_str(string("hey"), cons_str(string("haha"), nil_str()));
    // ListStr * hey = append_str(ls, ms);
    // print_list_str(ms);
    // free_list_str(ms);
    return 0;
}
