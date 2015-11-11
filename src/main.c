#include "Cello.h"
#include "parser.h"
#include "exec.h"
#include "network.h"


char * welcome_msg =
    "****************************************\n"
    "** Welcome to the information server. **\n"
    "****************************************\n";


// decrease indices and close some sockets
var shift_table(var table)
{
    var new_table = new(Table, Int, Socket);
    foreach (index in table) {
        int i = c_int(index);
        var pipe = get(table, index);    // get
        if (pipe) {
            if (i <= 0) {                   // too old
                close_socket(pipe);
            } else {
                set(new_table, $I(i - 1), pipe);   // set
            }
        }
    }

    return new_table;
}

var incoming_pipe(var table)
{
    if (mem(table, $I(0)))
        return get(table, $I(0));
    else
        return NULL;
}

var child_process(var args)
{
    // get sockets plugged
    struct Socket* socket = get(args, $I(0));
    struct File* err = get(args, $I(1));

    // flush the bloody buffer, fuck knows why
    print("\n");

    replace_socket(socket);

    // initialize env
    var env = new(Table, String, String);
    set(env, $S("PATH"), new(String, $S("bin:.")));

    // initialize pipe table
    var table = new(Table, Int, Socket);
    var yidam = new(Table, Int, Int);
    // var duplicated_pipes = new(List, Int);
    // var duplicated_pipes_in = new(List, Int);

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
                int line_result;
                int to_close = -1;
                int sin = 0;
                int sout = 1;
                int serr = 2;

                // check incoming pipes
                struct Socket *incoming = incoming_pipe(table);
                if (incoming) {
                    print_to(err, 0, ">>> incoming pipe pending %$\n", incoming);
                    sin = incoming->sin;
                    to_close = incoming->sout;
                } else {
                    sin = 0;
                }

                // check outgoing pipes
                if (line->socket->sout > 0) {
                    var out_index = $I(line->socket->sout);
                    struct Socket *outgoing = mem(table, out_index) ? get(table, out_index) : NULL;
                    if (outgoing) {
                        print_to(err, 0, ">>> %$ already existed %$\n", out_index, outgoing);
                        int new_sout = dup(outgoing->sout);
                        if (new_sout == -1)
                            perror("duplicating existing sout");
                        sout = new_sout;
                    } else {
                        outgoing = create_pipe();
                        set(table, out_index, outgoing);
                        // print_to(err, 0, ">>> %$ doesn't exists yet, creating %$\n", out_index, outgoing);
                        int new_sout = dup(outgoing->sout);
                        if (new_sout == -1)
                            perror("duplicating new sout");
                        sout = new_sout;
                    }
                } else {
                    sout = 1;
                }



                struct Socket *pipe = $(Socket, sin, sout, serr);


                if (to_close != -1) {
                    close(to_close);
                    to_close = -1;
                }

                line_result = exec_line(env, line, pipe);

                if (line_result != -1) {
                    close_socket(pipe);
                    assign(table, shift_table(table));
                }
            }
        }


    }

    return NULL;
}



int main(int argc, char *argv[])
{
    create_server($I(4444), $(Function, child_process));

    // struct Socket *p = create_pipe();
    // println("creating socket: %$", p);
    //
    //
    // char *test = "hey";
    // dprintf(p->sout, "hey\n");
    // // write(p->sout, test, strlen(test) + 1);
    //
    // // printf("%d\n", p->sin);
    // // fdopen(p->sout, "r");
    // // var sout = $(File, fdopen(p->sout, "w"));
    // // var message = $S("fuck you");
    // // swrite(sout, message, len(message) + 1);
    // // sflush(sout);
    //
    // // write(p->sout, test, strlen(test) + 1);
    // // var sout = $(File, fdopen(p->sout, "r"));
    // // print_to(sin, 0, "fuck you\n");
    //
    // // println("creating socket: %$", (sin));
    // // println("creating socket: %$", $I(seof(sout)));
    //
    // // puts("stuck");
    //
    //
    // // IO
    // var sin = $(File, fdopen(p->sin, "r"));
    // char buffer[1024];
    // int result = sread(sin, buffer, sizeof(buffer));
    // if (result == -1) perror("??");
    // printf("%d\n", result);
    // printf("%s\n", buffer);
    //

    //
    // // IO
    // char buffer[1024];
    // int result = read(p->sin, buffer, sizeof(buffer));
    // if (result == -1) perror("??");
    // printf("%d\n", result);
    // printf("%s\n", buffer);

    // close_socket(p);

    // create_server($I(4444), $(Function, child_process));
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
