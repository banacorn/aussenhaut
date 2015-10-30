#include "parser.h"
#include "type.h"
#include "net.h"
#include "env.h"

const char * welcome_msg =
    "****************************************\n"
    "** Welcome to the information server. **\n"
    "****************************************\n";

void child(int socket)
{
    Env * env = insert(nil_env(), string("PATH"), string("bin:."));

    send_message(socket, string((char *)welcome_msg));

    while (1) {
        send_message(socket, string("% "));
        Line * line = parse_line(read_message(socket));
        // print_line(line);
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
                List * paths = get_path(env);
                print_list(paths);
                free_list(paths);
                send_message(socket, append_string(copy_string(command_name), string("\n")));
                // printf("PRINT !!\n");
            }
            free_command(first_command);
            free_line(line);

        }
    }
}



void exec(String * path, String * name, List * args)
{
    pid_t pid = fork();
    if (pid == -1) {
        perror("exec fork error");
    } else if (pid == 0) {
        String * full_path = append_string(path, name);
        printf("executing %s\n", full_path -> content);

        // char ** arg_array = clone_char_array(cmd);



        // int result = execv("./ras/bin/ls", arg);
        // if (result) {
        //     perror("Return not expected. Must be an execv error.n");
        // }
    }
}
//
// int exec(Env * env, Command * cmd)
// {
//     pid_t pid = fork();
//     if (pid == -1) {
//         perror("exec fork error");
//     } else if (pid == 0) {
//         char ** arg_array = clone_char_array(cmd);
//         // int result = execv("./ras/bin/ls", arg);
//         // if (result) {
//         //     perror("Return not expected. Must be an execv error.n");
//         // }
//     }
// }

int main(int argc, char *argv[])
{
    // exec(string("ras/bin/"), string("ls"), nil());
    // create_server(7000, child);
    // char * const arg[] = {"ras/bin/lss", "-a", NULL};
    // pid_t pid = fork();
    // if (pid == -1) {
    //     perror("fork error");
    // } else if (pid == 0) {
    //     int result = execv("ras/bin/lss", arg);
    //     if (result) {
    //         perror("Return not expected. Must be an execv error.n");
    //     }
    // } else {
    //     // exit(1);
    //     // create_server(7000, child);
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
