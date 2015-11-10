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
            var list = tokenize(message, $S(" "));
            print_to($(File, stderr), 0, "%$\n", list);



            // var first_command = get(list, $I(0));




            // print_to($(File, stderr), 0, "> %$\n", message);
        }
    }

    return NULL;
}



int main(int argc, char *argv[])
{
    // create_server($I(4444), $(Function, child_process));
    // println("%$", tokenize($S("hey | you  | bitch"), $S(" | ")));
    // printf("%s\n", c_str(s));

    var result = parse_line($S("ls -a sdf > hey"));
    // var result = drop(tokenize($S("hey | you  | bitch"), $S(" | ")), -10);
    println("%$", result);
    return 0;
}
