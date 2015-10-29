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
            free_line(line);
            String * command_name = first_command -> name;
            if (compare_string(string("exit"), command_name)) {
                free_command(first_command);
                free_env(env);
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
                // printf("PRINT !!\n");
            }
            free_command(first_command);
        }
    }
}

int main(int argc, char *argv[])
{
    // create_server(7000, child);

    // Env * d = nil_env();
    // Env * d = cons_env(string("PATH"), string("bin::."), cons_env(string("b"), string("1"), nil_env()));
    // Env * d = cons_env(string("PATH"), string("bin:."), cons_env(string("b"), string("1"), nil_env()));
    // List * paths = get_path(d);
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


    return 0;
}
