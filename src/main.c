#include "Cello.h"
#include "parser.h"
#include "exec.h"
#include "network.h"


char * welcome_msg =
    "****************************************\n"
    "** Welcome to the information server. **\n"
    "****************************************\n";

var child_process(var args)
{
    // get sockets plugged
    struct Socket* socket = get(args, $I(0));

    // flush the bloody buffer, fuck knows why
    print("\n");

    replace_socket(socket);

    // initialize env
    var env = new(Table, String, String);
    set(env, $S("PATH"), new(String, $S("bin:.")));

    send_message($S(welcome_msg));

    while(1) {
        send_message($S("% "));
        var message = read_message();
        struct Line* line = parse_line(message);
        var commands = line->commands;

        if (len(commands) > 0) {
            var first_command = head(commands);
            var first_command_name = head(first_command);

            if (eq(first_command_name, $S("exit"))) {
                break;
            } else if (eq(first_command_name, $S("printenv"))) {
                if (len(first_command) == 1) {  // print all
                    foreach (key in env) {
                        println("%s=%s", key, get(env, key));
                    }
                } else if (len(first_command) == 2) {  // print specific
                    var key = get(first_command, $I(1));
                    if (mem(env, key)) {
                        println("%s=%s", key, get(env, key));
                    } else {
                        println("%s=", key);
                    }
                } else {
                    println("error: wrong number of arguments for 'printenv'");
                }
            } else if (eq(first_command_name, $S("setenv"))) {
                if (len(first_command) == 3) {
                    var key = get(first_command, $I(1));
                    var val = get(first_command, $I(2));
                    set(env, key, val);
                } else {
                    println("error: wrong number of arguments for 'setenv'");
                }
            } else {
                // print_to($(File, stderr), 0, "> %$\n", first_command_name);
                exec_line(env, line, $(Socket, 0, 1, 2));
                // var executable = search_exec(env, first_command_name);
                // if (executable) {
                //     exec_command(env, first_command, new(Socket, NULL, NULL, NULL));
                //
                // } else {
                //     println("Unknown command: [%s].", first_command_name);
                // }
            }
        }


        //
        // if (eq(message, $S("exit"))) {
        //     break;
        // } else if (eq(message, $S("printenv"))) {
        //     print_to($(File, stderr), 0, "%$\n", env);
        // } else {
        //     var result = parse_line(message);
        //     print_to($(File, stderr), 0, "%$\n", result);
        //
        //

            // var first_command = get(list, $I(0));




            // print_to($(File, stderr), 0, "> %$\n", message);
    }

    return NULL;
}



int main(int argc, char *argv[])
{
    create_server($I(4444), $(Function, child_process));
    // var command = new(List, String, $S("lssdfs dfsdf"), $S("-sdfs dfsdfsfsfsfsdfa"));
    // var executable = $S("asdfasdfas");
    // char ** space = clone_char_array(command, executable);
    // free_command_char_array(command, space);

    // println("%$", parse_line($S("ls -a sdf | hey wef asd |123")));
    // println("%$", parse_line($S("ls -a sdf | hey wef asd !123")));
    // println("%$", parse_line($S("ls -a sdf | hey wef asd |123 !435")));
    // println("%$", parse_line($S("ls -a sdf | hey wef asd > gasdg")));
    // println("%$", parse_line($S("ls -a sdf | hey wef asd ")));
    // println("%$", parse_line($S("ls -a sdf | hey wef asd | 123")));
    return 0;
}
