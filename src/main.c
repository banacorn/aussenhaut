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
        print_line(line);
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
                String * path = search(env, string("PATH"));
                String * message = append_string(string("PATH="), append_string(path, string("\n")));
                send_message(socket, message);
            } else {
                // printf("PRINT !!\n");
            }
            free_command(first_command);
        }
    }
}

int main(int argc, char *argv[])
{
    create_server(7000, child);

    // Env * d = cons_env(string("a"), string("000000000000"), cons_env(string("b"), string("1"), nil_env()));
    // d = insert(d, string("a"), string("8"));
    // print_env(d);
    // printf("\n");
    // // String * result = search(d, string("a"));
    // // if (result) {
    // //     print_string(result);
    // //     free_string(result);
    // // }
    // free_env(d);


    return 0;
}
