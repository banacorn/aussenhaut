#include "exec.h"
#include "parser.h"

#include <sys/wait.h>
#include <fcntl.h>


var env_path(var env)
{
    if (mem(env, $S("PATH"))) {
        // print_to($(File, stderr), 0, " %$\n", tokenize( get(env, $S("PATH")), $S(":")));
        return tokenize(get(env, $S("PATH")), $S(":"));
    } else {
        perror("no PATH in env");
        return NULL;
    }
}


var search_exec(var env, var name)
{
    var prefixes = env_path(env);
    // print_to($(File, stderr), 0, "> %$\n", prefixes);

    foreach (prefix in prefixes) {
        var x = new(String, $S("ras/"));
        concat(x, prefix);
        concat(x, $S("/"));
        concat(x, name);
        if (access(c_str(x), X_OK) != -1) {
            return copy(x);
        }
    }
    return NULL;
}

char ** clone_char_array(var command, var executable)
{
    // determine the "width" of matrix by measuring the longest string + 1
    size_t longest = len(executable) + 1;
    int rows = len(command) + 1;
    foreach (s in command) {
        size_t length = len(s) + 1;
        if (longest < length)
            longest = length;
    }

    // allocate memory, longest x length args + 2 (NULL terminating array)
    char ** space = malloc(rows * sizeof(char *));

    // copy args
    int i = 0;
    foreach (s in command) {
        space[i] = malloc(longest);
        strncpy(space[i], c_str(s), len(s) + 1);
        i++;
    }
    // overwrite command name with executable path
    strncpy(space[0], c_str(executable), len(executable) + 1);

    // NULL terminating
    space[rows - 1] = NULL;
    return space;
}

void free_command_char_array(var command, char ** space)
{
    int i = 0;
    foreach (s in command) {
        free(space[i]);
        i++;
    }
    free(space[len(command)]);
    free(space);
}


int exec_command(var env, var command, var socket)
{
    var executable = search_exec(env, head(command));
    int status;
    if (!executable) {
        return -1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("exec fork error");
        return -1;
    } else if (pid == 0) {          // child process!
        replace_socket(socket);
        char ** arg_array = clone_char_array(command, executable);
        int result = execv(c_str(executable), arg_array);
        free_command_char_array(command, arg_array);
        // not reachable
        return -1;
    } else {                        // parent process!
        //waiting for child to terminate
        int pid = wait(&status);
        // close some shits
        close_socket(socket);
        return 0;
    }

}

int exec_line(var env, struct Line* line, struct Socket* socket)
{
    int status = 0;
    // print_to($(File, stderr), 0, "error\n");
    // replace_socket(socket);
    // print_to($(File, stderr), 0, "error after\n");

    if (len(line->redirection)) {
        socket->sout = open(c_str(line->redirection), O_WRONLY | O_CREAT, 0777);
    }

    var commands = line->commands;
    struct Socket *bridge_in, *bridge_out;
    int i = 0;
    foreach (command in commands) {
        bool is_first = i == 0;
        bool is_last = i == len(commands) - 1;

        // print_to($(File, stderr), 0, "executing %$ \n", head(command));

        if (is_first) {
            bridge_in = socket;
        }

        if (is_last) {
            bridge_out = socket;
        } else {
            bridge_out = create_pipe();
        }

        int exec_result = exec_command(env, command, $(Socket, bridge_in->sin, bridge_out->sout, 2));
        if (exec_result == -1) {
            println("Unknown command: [%s].", head(command));
            if (is_first)
                status = -1;
            break;
        }

        bridge_in = bridge_out;
        i++;
    }
    return status;
}
