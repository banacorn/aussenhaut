#include "parser.h"
#include "type.h"
#include "net.h"
#include "env.h"
#include <sys/wait.h>
#include <fcntl.h>


const char * welcome_msg =
    "****************************************\n"
    "** Welcome to the information server. **\n"
    "****************************************\n";

// Env -> Cmd -> Nullable String
String * search_exec(Env * env, Command * cmd)
{
    List * prefixes = get_path(env);

    // scan through all of the paths, return the first one that is executable
    List * cursor = prefixes;
    String * executable = NULL;
    while (cursor -> Nil == FALSE) {
        String * prefix = append_string(head(cursor), copy_string(cmd -> name));
        if (access( prefix -> content, X_OK ) != -1) {
            executable = copy_string(prefix);
            free_string(prefix);
            break;
        } else {
            free_string(prefix);
            cursor = cursor -> Cons;
        }
    }
    free_list(prefixes);
    return executable;
}

typedef struct Socket {
    int in;
    int out;
} Socket;

Socket std = {0, 1};

Socket create_pipe()
{
    int fds[2];
    if (pipe(fds) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    } else {
        Socket s = {fds[0], fds[1]};
        return s;
    }
}

void replace_socket(Socket sckt)
{
    if (sckt.in != 0) {
        close(0);
        dup(sckt.in);
        close(sckt.in);
    }
    if (sckt.out != 1) {
        close(1);
        dup(sckt.out);
        close(sckt.out);
    }
}

void close_socket(Socket sckt, Socket reserve)
{
    if (sckt.in != reserve.in && sckt.in != 0) {
        close(sckt.in);
    }
    if (sckt.out != reserve.out && sckt.in != 1) {
        close(sckt.out);
    }
}

int exec_command(Env * env, Command * cmd, Socket sckt, Socket stream)
{
    String * executable = search_exec(env, cmd);
    int status;
    // execute if found any
    if (executable) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("exec fork error");
            return -1;
        } else if (pid == 0) {  // child process
            replace_socket(sckt);
            char ** arg_array = clone_char_array(cmd, copy_string(executable));
            int result = execv(executable -> content, arg_array);
            // not reachable anyway
            perror("execv error\n");
            free_command_char_array(cmd, arg_array);
            free_string(executable);
            return 0;
        } else {    // parent process
            //waiting for child to terminate
            int pid = wait(&status);

            close_socket(sckt, stream);
            free_string(executable);
            return 0;
        }
    } else {
        return -1;
    }
}

// returns a String if there's an unknown command, else NULL
void exec_line(Env * env, Line * line, Socket stream)
{
    fprintf(stderr, "streaming [%d, %d]\n", stream.in, stream.out);

    if (line -> redirect) {
        stream.out = open(line -> target -> content, O_WRONLY | O_CREAT, 0777);
    }

    // start bridging pipes;
    List * cursor = line -> cmds;
    Socket last_pipe = {stream.in, 1};
    Socket next_pipe = {0, 1};
    Socket sckt;
    while (cursor -> Nil == FALSE) {
        Command * cmd = head(cursor);
        if (length(cursor) == 1) {           // last
            sckt.in = last_pipe.in;
            sckt.out = stream.out;          // out
        } else {
            next_pipe = create_pipe();
            sckt.in = last_pipe.in;
            sckt.out = next_pipe.out;
            last_pipe = next_pipe;
        }

        // returns -1 if cannot find command
        int exec_result = exec_command(env, cmd, sckt, stream);
        if (exec_result == -1) {
            String * unknown = copy_string(cmd -> name);
            free_command(cmd);
            if (unknown) {
                dprintf(stream.out, "Unknown command: [%s].\n", unknown -> content);
                break;
            }
        } else {
            free_command(cmd);
            cursor = cursor -> Cons;
        }
    }
}


void child(int socket)
{
    Env * env = insert(nil_env(), string("PATH"), string("bin:."));
    send_message(socket, string((char *)welcome_msg));

    while (1) {
        send_message(socket, string("% "));
        Line * line = parse_line(read_message(socket));
        if (null(line -> cmds)) {   // empty command
            free_line(line);
        } else {
            Command * first_command = head(line -> cmds);
            String * command_name = first_command -> name;
            if (compare_string(string("exit"), command_name)) {
                free_command(first_command);
                free_env(env);
                free_line(line);

                break;
            } else if (compare_string(string("printenv"), command_name)) {
                if (arg_length(first_command) == 0) {
                    String * message = show_all_env(env);
                    send_message(socket, message);
                } else {
                    String * key = head(first_command -> args);
                    String * val = search(env, copy_string(key));
                    if (val) {
                        String * message = append_string(val, string("\n"));
                        free_string(key);
                        send_message(socket, message);
                    } else {
                        String * message = append_string(key, string("=\n"));
                        send_message(socket, message);
                    }
                }
            } else if (compare_string(string("remove"), command_name)) {
                if (arg_length(first_command) == 1) {
                    String * key = head(first_command -> args);
                    env = remove_env(env, key);
                } else {
                    String * message = string("wrong number of arguments for remove\n");
                    send_message(socket, message);
                }
            } else if (compare_string(string("setenv"), command_name)) {
                if (arg_length(first_command) == 2) {
                    String * key = trim(head(first_command -> args));
                    String * val = trim(last(first_command -> args));
                    env = insert(env, key, val);
                } else {
                    send_message(socket, string("ERROR: wrong number of arguments for \"setenv\"\n"));
                }
            } else {
                Socket stream = {0, socket};
                exec_line(env, line, stream);
            }
            free_command(first_command);
            free_line(line);

        }
    }
}


int main(int argc, char *argv[])
{
    create_server(4444, child);

    // Env * env = cons_env(string("PATH"), string("bin:."), nil_env());
    // Line * command0 = parse_line(string("ls -a | number |1"));
    // Line * command1 = parse_line(string("cat"));
    // List * program = cons(box_line(command0), cons(box_line(command1), nil()));
    //
    //
    //
    // free_list(program);
    // free_env(env);

    // Env * e = cons_env(string("PATH"), string("bin:."), nil_env());
    // String * commands = string("ls -a | number");
    // // for (int i = 0; i < 1000; i++) {
    // //     commands = append_string(commands, string(" | cat | cat"));
    // // }
    // // // commands = append_string(commands, string(" > test/temp"));
    // fprintf(stderr, "%s\n", commands -> content);
    // Line * l = parse_line(commands);
    // exec_line(e, l, std);
    //
    // free_line(l);
    // free_env(e);

    // Env * env = cons_env(string("HA"), string("bin:."), cons_env(string("PATH"), string("bin:."), nil_env()));
    // print_env(env);
    // env = remove_env(env, string("PATH"));
    // print_env(env);
    // free_env(env);


    return 0;
}
