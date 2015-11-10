#include "Cello.h"
#include "parser.h"
// #include "type.h"
#include "network.h"
// #include "env.h"
// #include <sys/wait.h>
// #include <fcntl.h>


char * welcome_msg =
    "****************************************\n"
    "** Welcome to the information server. **\n"
    "****************************************\n";


//
// // Env -> Cmd -> Nullable String_
// String_ * search_exec(Env * env, Command * cmd)
// {
//     List_ * prefixes = get_path(env);
//
//     // scan through all of the paths, return the first one that is executable
//     List_ * cursor = prefixes;
//     String_ * executable = NULL;
//     while (cursor -> Nil == FALSE) {
//         String_ * prefix = append_string(head(cursor), copy_string(cmd -> name));
//         if (access( prefix -> content, X_OK ) != -1) {
//             executable = copy_string(prefix);
//             free_string(prefix);
//             break;
//         } else {
//             free_string(prefix);
//             cursor = cursor -> Cons;
//         }
//     }
//     free_list(prefixes);
//     return executable;
// }
//
// typedef struct Socket {
//     int in_socket;
//     int out_socket;
// } Socket;
//
// Socket std = {0, 1};
//
// Socket create_pipe()
// {
//     int fds[2];
//     if (pipe(fds) == -1) {
//         perror("pipe");
//         exit(EXIT_FAILURE);
//     } else {
//         Socket s = {fds[0], fds[1]};
//         return s;
//     }
// }
//
// void replace_socket(Socket sckt)
// {
//     if (sckt.in_socket != 0) {
//         close(0);
//         dup(sckt.in_socket);
//         close(sckt.in_socket);
//     }
//     if (sckt.out_socket != 1) {
//         close(1);
//         dup(sckt.out_socket);
//         close(sckt.out_socket);
//     }
// }
//
// void close_socket(Socket sckt, Socket reserve)
// {
//     if (sckt.in_socket != reserve.in_socket && sckt.in_socket != 0) {
//         close(sckt.in_socket);
//     }
//     if (sckt.out_socket != reserve.out_socket && sckt.in_socket != 1) {
//         close(sckt.out_socket);
//     }
// }
//
// int exec_command(Env * env, Command * cmd, Socket sckt, Socket stream)
// {
//     String_ * executable = search_exec(env, cmd);
//     int status;
//     // execute if found any
//     if (executable) {
//         pid_t pid = fork();
//         if (pid == -1) {
//             perror("exec fork error");
//             return -1;
//         } else if (pid == 0) {  // child process
//             replace_socket(sckt);
//             char ** arg_array = clone_char_array(cmd, copy_string(executable));
//             int result = execv(executable -> content, arg_array);
//             // not reachable anyway
//             perror("execv error\n");
//             free_command_char_array(cmd, arg_array);
//             free_string(executable);
//             return 0;
//         } else {    // parent process
//             //waiting for child to terminate
//             int pid = wait(&status);
//
//             close_socket(sckt, stream);
//             free_string(executable);
//             return 0;
//         }
//     } else {
//         return -1;
//     }
// }
//
// // returns a String_ if there's an unknown command, else NULL
// void exec_line(Env * env, Line * line, Socket stream)
// {
//     fprintf(stderr, "streaming [%d, %d]\n", stream.in_socket, stream.out_socket);
//
//     if (line -> redirect) {
//         stream.out_socket = open(line -> target -> content, O_WRONLY | O_CREAT, 0777);
//     }
//
//     // start bridging pipes;
//     List_ * cursor = line -> cmds;
//     Socket last_pipe = {stream.in_socket, 1};
//     Socket next_pipe = {0, 1};
//     Socket sckt;
//     while (cursor -> Nil == FALSE) {
//         Command * cmd = head(cursor);
//         if (length(cursor) == 1) {           // last
//             sckt.in_socket = last_pipe.in_socket;
//             sckt.out_socket = stream.out_socket;          // out
//         } else {
//             next_pipe = create_pipe();
//             sckt.in_socket = last_pipe.in_socket;
//             sckt.out_socket = next_pipe.out_socket;
//             last_pipe = next_pipe;
//         }
//
//         // returns -1 if cannot find command
//         int exec_result = exec_command(env, cmd, sckt, stream);
//         if (exec_result == -1) {
//             String_ * unknown = copy_string(cmd -> name);
//             free_command(cmd);
//             if (unknown) {
//                 dprintf(stream.out_socket, "Unknown command: [%s].\n", unknown -> content);
//                 break;
//             }
//         } else {
//             free_command(cmd);
//             cursor = cursor -> Cons;
//         }
//     }
// }
//
//
// void child(int socket)
// {
//     Env * env = insert(nil_env(), string("PATH"), string("bin:."));
//     send_message(socket, string((char *)welcome_msg));
//
//     while (1) {
//         send_message(socket, string("% "));
//         Line * line = parse_line(read_message(socket));
//         if (null(line -> cmds)) {   // empty command
//             free_line(line);
//         } else {
//             Command * first_command = head(line -> cmds);
//             String_ * command_name = first_command -> name;
//             if (compare_string(string("exit"), command_name)) {
//                 free_command(first_command);
//                 free_env(env);
//                 free_line(line);
//
//                 break;
//             } else if (compare_string(string("printenv"), command_name)) {
//                 if (arg_length(first_command) == 0) {
//                     String_ * message = show_all_env(env);
//                     send_message(socket, message);
//                 } else {
//                     String_ * key = head(first_command -> args);
//                     String_ * val = search(env, copy_string(key));
//                     if (val) {
//                         String_ * message = append_string(val, string("\n"));
//                         free_string(key);
//                         send_message(socket, message);
//                     } else {
//                         String_ * message = append_string(key, string("=\n"));
//                         send_message(socket, message);
//                     }
//                 }
//             } else if (compare_string(string("remove"), command_name)) {
//                 if (arg_length(first_command) == 1) {
//                     String_ * key = head(first_command -> args);
//                     env = remove_env(env, key);
//                 } else {
//                     String_ * message = string("wrong number of arguments for remove\n");
//                     send_message(socket, message);
//                 }
//             } else if (compare_string(string("setenv"), command_name)) {
//                 if (arg_length(first_command) == 2) {
//                     String_ * key = trim(head(first_command -> args));
//                     String_ * val = trim(last(first_command -> args));
//                     env = insert(env, key, val);
//                 } else {
//                     send_message(socket, string("ERROR: wrong number of arguments for \"setenv\"\n"));
//                 }
//             } else {
//                 Socket stream = {0, socket};
//                 exec_line(env, line, stream);
//             }
//             free_command(first_command);
//             free_line(line);
//
//         }
//     }
// }


var replace_socket(var self)
{
    struct Socket* socket = self;

    // replace STDIN 0
    if (socket->sin != 0) {
        println("directing STDIN 0 => %$", $I(socket->sin));
        close(0);
        if (dup(socket->sin) < 0)
            perror("error stdin dup");
        close(socket->sin);
    }

    // replace STDOUT 1
    if (socket->sout != 1) {
        println("directing STDOUT 1 => %$", $I(socket->sout));
        close(1);
        if (dup(socket->sout) < 0)
            perror("error stdout dup");
        close(socket->sout);
    }

    // replace STDERR 2
    if (socket->serr != 2) {
        println("directing STDERR 2 => %$", $I(socket->serr));
        close(2);
        if (dup(socket->serr) < 0)
            perror("error stderr dup");
        close(socket->serr);
    }
    return NULL;
}


var child_process(var args)
{
    // get sockets plugged
    struct Socket* socket = get(args, $I(0));
    replace_socket(socket);

    // initialize env
    var env = new(Table, String, List);
    set(env, $S("PATH"), new(List, String, $S("bin"), $S(".")));

    send_message($S(welcome_msg));

    while(1) {
        send_message($S("% "));
        var message = read_message();
        if (eq(message, $S("exit"))) {
            break;
        } else if (eq(message, $S("printenv"))) {
            print_to($(File, stderr), 0, "%$\n", env);
        } else {
            var list = tokenize(message);
            print_to($(File, stderr), 0, "%$\n", list);
            // print_to($(File, stderr), 0, "> %$\n", message);
        }
    }

    return NULL;
}



int main(int argc, char *argv[])
{
    create_server($I(4444), $(Function, child_process));

    return 0;
}
