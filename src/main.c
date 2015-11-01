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

void close_socket(Socket sckt)
{
    if (sckt.in != 0)
        close(sckt.in);
    if (sckt.out != 1)
        close(sckt.out);
}

int exec_command(Env * env, Command * cmd, Socket sckt, Bool redirect, String * target)
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

            // if redirected, replace stdout
            int redirected_fd;
            if (redirect) {
                redirected_fd = open(target -> content, O_WRONLY | O_CREAT, 0777);
                close(1);
                dup(redirected_fd);
                close(redirected_fd);
            }

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

            close_socket(sckt);
            free_string(executable);
            return 0;
        }
    } else {
        return -1;
    }
}

// returns a String if there's an unknown command, else NULL
String * exec_line(Env * env, Line * line)
{
    if (line -> redirect) {
        List * cursor = line -> cmds;
        Socket last_pipe = {0, 1};
        Socket next_pipe = {0, 1};
        Socket sckt;
        int redirect;
        while (cursor -> Nil == FALSE) {
            Command * cmd = head(cursor);
            if (length(cursor) == 1) {           // last
                sckt.in = last_pipe.in;
                sckt.out = 1;
                redirect = TRUE;
            } else {
                next_pipe = create_pipe();
                sckt.in = last_pipe.in;
                sckt.out = next_pipe.out;
                last_pipe = next_pipe;
                redirect = FALSE;
            }

            // returns -1 if cannot find command
            int exec_result = exec_command(env, cmd, sckt, redirect, line -> target);
            if (exec_result == -1) {
                String * unknown = copy_string(cmd -> name);
                free_command(cmd);
                return unknown;
            } else {
                free_command(cmd);
                cursor = cursor -> Cons;
            }
        }
        return NULL;
    } else {
        if (line -> out == -1 && line -> err == -1) {
            List * cursor = line -> cmds;
            Socket last_pipe = {0, 1};
            Socket next_pipe = {0, 1};
            Socket sckt;
            while (cursor -> Nil == FALSE) {
                Command * cmd = head(cursor);
                if (length(cursor) == 1) {           // last
                    sckt.in = last_pipe.in;
                    sckt.out = 1;
                } else {
                    next_pipe = create_pipe();
                    sckt.in = last_pipe.in;
                    sckt.out = next_pipe.out;
                    last_pipe = next_pipe;
                }

                // returns -1 if cannot find command
                int exec_result = exec_command(env, cmd, sckt, FALSE, NULL);
                if (exec_result == -1) {
                    String * unknown = copy_string(cmd -> name);
                    free_command(cmd);
                    return unknown;
                } else {
                    free_command(cmd);
                    cursor = cursor -> Cons;
                }
            }
            return NULL;
        } else {
            puts("not supported yet");
            return NULL;
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
                String * message = show_all_env(env);
                send_message(socket, message);
            } else if (compare_string(string("setenv"), command_name)) {
                if (arg_length(first_command) == 2) {
                    String * key = trim(head(first_command -> args));
                    String * val = trim(last(first_command -> args));
                    env = insert(env, key, val);
                } else {
                    send_message(socket, string("ERROR: wrong number of arguments for \"setenv\"\n"));
                }
            } else {
                String * unknown = exec_line(env, line);
                print_line(line);
                puts("");
                if (unknown) {
                    String * message = append_string(
                            string("Unknown command: ["),
                            append_string(
                                unknown,
                                string("]\n")
                            )
                        );
                    send_message(socket, message);
                } else {
                    replace_socket(std);
                    fprintf(stderr, "幹幹幹\n");
                    send_message(socket, string("幹\n"));
                }

                // send_message(socket, append_string(copy_string(command_name), string("\n")));

                // Socket std = {0, 1};
                // int exec_result = exec_command(env, first_command, std);
                // if (exec_result == -1) {
                //     String * message = append_string(
                //         string("Unknown command: ["),
                //         append_string(
                //             copy_string(command_name),
                //             string("]\n")
                //         )
                //     );
                //     send_message(socket, message);
                // } else {
                //     send_message(socket, append_string(copy_string(command_name), string("\n")));
                // }
            }
            free_command(first_command);
            free_line(line);

        }
    }
}


int main(int argc, char *argv[])
{
    // create_server(7000, child);

    Env * e = cons_env(string("PATH"), string("bin:."), nil_env());
    String * commands = string("ls -a | number");
    for (int i = 0; i < 10; i++) {
        commands = append_string(commands, string(" | cat | cat"));
    }
    // commands = append_string(commands, string(" > test/temp"));
    puts(commands -> content);
    Line * l = parse_line(commands);
    String * unkown = exec_line(e, l);
    if (unkown) {
        perror(unkown -> content);
    }
    free_line(l);
    free_env(e);




    //
    // Socket std = {0, 1};
    // Command * cmd = parse_command(string("ls -a"));
    // exec_command(e, cmd, std);
    // Command * cmd1 = parse_command(string("ls"));
    // exec_command(e, cmd1, std);



    // exit(0);
    // Env
    // Env * e = cons_env(string("PATH"), string("bin:."), nil_env());
    //
    // // Line
    // String * str = string("ls -a | cat | cat | cat | cat");
    // Line * l = parse_line(str);
    // Command * ls = head(l -> cmds);
    // Command * cat0 = head(l -> cmds -> Cons);
    // Command * cat1 = head(l -> cmds -> Cons -> Cons);
    // Command * cat2 = head(l -> cmds -> Cons -> Cons -> Cons);
    // Command * cat3 = head(l -> cmds -> Cons -> Cons -> Cons -> Cons);
    //
    // Socket bet0 = create_pipe();
    // Socket bet1 = create_pipe();
    // Socket ls_socket = {0, bet0.out};
    // Socket cat0_socket = {bet0.in, bet1.out};
    // Socket cat1_socket = {bet1.in, 1};
    // exec_command(e, ls, ls_socket);
    // exec_command(e, cat0, cat0_socket);
    // exec_command(e, cat1, cat1_socket);
    //
    // // Cleanup
    // free_command(ls);
    // free_command(cat0);
    // free_command(cat1);
    // free_command(cat2);
    // free_command(cat3);
    //
    // free_line(l);
    // free_env(e);




    // Command * cmd0 = command(string("ls"), cons(box_chars("ls"), cons(box_chars("-a"), nil())));
    // Command * cmd1 = command(string("cat"), cons(box_chars("cat") nil()));
    // Command * cmd2 = command(string("cat"), cons(box_chars("cat") nil()));
    //
    // Line * l = line()


    // create_server(7000, child);


    // List * args = cons(box_chars("-a"), nil());
    // Command * cmd = command(string("ls"), args);
    // exec_command(string("ras/bin/"), cmd);
    // printf("result: %d\n", exec_command(string("ras/bin/"), cmd));
    // free_command(cmd);
    //






    // int pipe_fds[2];
    // int status;
    // int pipe_result = pipe(pipe_fds);
    // if (pipe_result == -1) {
    //     perror("creating pipe: failed");
    //     exit(EXIT_FAILURE);
    // }
    //
    // char * const arg[] = {"ras/bin/cat", NULL};
    // // char * const arg[] = {"ras/bin/ls", "-a", NULL};
    // pid_t pid = fork();
    // if (pid == -1) {
    //     perror("fork error");
    // } else if (pid == 0) {      // child
    //     close(1);   // close stdout
    //     dup(pipe_fds[1]);
    //     int result = execv(arg[0], arg);
    //     if (result) {
    //         perror("Return not expected. Must be an execv error.n");
    //     }
    // } else {                    // parent
    //     // if( (pid = wait(&status)) < 0){
    //     //   perror("wait");
    //     //   _exit(1);
    //     // }
    //     // if ( WIFEXITED(status) ){
    //     //   printf("Parent: Child exited with status: %d\n", WEXITSTATUS(status));
    //     // }
    //     printf("Parent: finished\n");
    // }
    // printf("Parent: start cat\n");
    //
    // char * const arg2[] = {"ras/bin/ls", NULL};
    // pid_t pid2 = fork();
    // if (pid2 == -1) {
    //     perror("fork error");
    // } else if (pid2 == 0) {      // child
    //     close(0);   // close stdin
    //     dup(pipe_fds[0]);
    //     int result = execv(arg2[0], arg2);
    //     if (result) {
    //         perror("Return not expected. Must be an execv error.n");
    //     }
    // } else {                    // parent
    //     // if( (pid = wait(&status)) < 0){
    //     //   perror("wait");
    //     //   _exit(1);
    //     // }
    //     //
    //     // printf("Parent: finished\n");
    // }



    // Env * d = nil_env();
    // Env * d = cons_env(string("PATH"), string("bin::."), cons_env(string("b"), string("1"), nil_env()));
    // Env * d = cons_env(string("PATH"), string("bin:."), cons_env(string("b"), string("1"), nil_env()));
    // List * paths = get_path(d, string("s"));
    // print_list(paths);
    // free_list(paths);
    // print_env(d);
    // free_env(d);

    // Env * e = copy_env(d);
    // print_env(e);
    // free_env(d);
    // free_env(e);
    // // Env * d = nil_env();
    // // d = insert(d, string("a"), string("8"));
    // printf("\n");
    // String * s = show_all_env(d);
    // printf("%s", s -> content);
    // free_string(s);

    // List * args = cons(box_chars("sdf"), cons(box_chars("12asdfasdf3"), nil()));
    // Command * cmd = command(string("ls"), args);
    // char ** space = clone_char_array(cmd, string("ras/bin/"));
    // for (int i = 0; i < arg_length(cmd) + 1; i++)
    //     puts(space[i]);
    // free_command_char_array(cmd, space);
    // free_command(cmd);

    return 0;
}
