#include "parser.h"
#include "type.h"


////////////////////////////////////////////////////////////////////////////////
//  Command
////////////////////////////////////////////////////////////////////////////////

Command * command(String_ * name, List_ * args)
{
    Command * node = malloc(sizeof(Command));
    node -> name = name;
    node -> args = args;
    return node;
}

Command * copy_command(Command * node)
{
    return command(copy_string(node -> name), copy__list(node -> args));
}

Command * parse_command(String_ * str)
{
    str = trim(str);
    if (null_string(str)) {
        free_string(str);
        return command(string(""), nil());
    } else {
        List_ * tokens = compact(tokenize(str , string(" ")));
        String_ * name = head(tokens);
        List_ * args = tail(tokens);
        free_list(tokens);
        return command(name, args);
    }
}

char ** clone_char_array(Command * cmd, String_ * path)
{
    // determine the "width" of matrix by measuring the longest string + 1
    size_t longest = string_size(path);
    int number_of_args = arg_length(cmd);
    int i;
    for (i = 0; i < number_of_args; i++) {
        String_ * s = elemAt(cmd -> args, i);
        size_t len = string_size(s);
        free_string(s);
        if (longest < len)
            longest = len;
    }

    // allocate memory, longest x length args + 2 (NULL terminating array)
    char ** space = malloc((number_of_args + 2) * sizeof(char *));

    // copy command name
    space[0] = malloc(longest);
    strncpy(space[0], path -> content, string_size(path));
    free_string(path);

    // copy command args
    for (i = 0; i < number_of_args; i++) {
        space[i + 1] = malloc(longest);

        String_ * s = elemAt(cmd -> args, i);
        strncpy(space[i + 1], s -> content, string_size(s));
        free_string(s);
    }
    // NULL terminating
    space[number_of_args + 1] = NULL;

    return space;
}

void free_command_char_array(Command * cmd, char ** space)
{
    int i;
    for (i = 0; i < arg_length(cmd) + 1; i++) {
        free(space[i]);
    }
    free(space);
}


int arg_length(Command * node)
{
    return length(node -> args);
}

void free_command(Command * node)
{
    free_string(node -> name);
    free_list(node -> args);
    free(node);
}


void print_command(Command * node)
{
    if (node -> args -> Nil) {
        printf("%s", node -> name -> content);
    } else {
        printf("%s ", node -> name -> content);
        print__list(node -> args);
    }
}

Box_ * box_cmd(Command * cmd)
{
    return box(cmd, (void (*)(void *))free_command, (void * (*)(void *))copy_command, (void (*)(void *))print_command);
}


////////////////////////////////////////////////////////////////////////////////
//  Line
////////////////////////////////////////////////////////////////////////////////

Line * line(List_ * cmds, Bool redirect, String_ * target, int out, int err)
{
    Line * node = malloc(sizeof(Line));
    node -> cmds = cmds;
    node -> redirect = redirect;
    node -> target = target;
    node -> out = out;
    node -> err = err;
    return node;
}

Box_ * parse_command_boxed(Box_ * data)
{
    return box_cmd(parse_command(unbox(data)));
}

// String_ -> Maybe (List_ Command, String_)
Maybe * parse_redirection(String_ * str)
{
    str = trim(str);
    Pair * splitted = rsplit(str, string(" > "));
    String_ * first_part = fst(splitted);
    String_ * second_part = snd(splitted);

    if (null_string(first_part)) {  // no " > "
        free_string(first_part);
        free_string(second_part);
        free_pair(splitted);
        return nothing();
    } else {                        // has " > "
        List_ * snd_part_cmds = compact(tokenize(second_part, string(" ")));
        if (length(snd_part_cmds) == 1) {   // OK
            String_ * last_str = head(snd_part_cmds);
            List_ * commands = map_(parse_command_boxed, tokenize(first_part, string(" | ")));
            free_list(snd_part_cmds);
            free_pair(splitted);
            return just(box_pair(pair(box_list(commands), box_str(last_str))));
        } else {
            free_list(snd_part_cmds);
            free_string(first_part);
            free_pair(splitted);
            return nothing();
        }
    }
}

// String_ -> Int
int parse_numbered_pipe(String_ * str)
{
    int result = -1;
    String_ * head = take_string(str, 1);
    String_ * tail = drop_string(str, 1);
    String_ * pipe = string("|");

    if (compare_string(head, pipe) && numeral(tail)) {
        result = to_int(tail);
    } else {
        free_string(tail);
    }
    free_string(head);
    free_string(pipe);
    free_string(str);
    return result;
}

// String_ -> Int
int parse_numbered_bang(String_ * str)
{
    int result = -1;
    String_ * head = take_string(str, 1);
    String_ * tail = drop_string(str, 1);
    String_ * bang = string("!");

    if (compare_string(head, bang) && numeral(tail)) {
        result = to_int(tail);
    } else {
        free_string(tail);
    }
    free_string(head);
    free_string(bang);
    free_string(str);
    return result;
}

// String_ -> Line
Line * parse_piping(String_ * str)
{
    // preprocess
    str = trim(str);
    List_ * tokens = compact(tokenize(str, string(" ")));
    if (length(tokens) > 1) {
        String_ * last_str = last(tokens);
        List_ * temp_init = init(tokens);
        String_ * snd_last_str = last(temp_init);
        free_list(temp_init);
        int last_is_pipe = parse_numbered_pipe(copy_string(last_str));
        int last_is_bang = parse_numbered_bang(last_str);

        int snd_last_is_pipe = parse_numbered_pipe(copy_string(snd_last_str));
        int snd_last_is_bang = parse_numbered_bang(snd_last_str);

        if (snd_last_is_bang > -1 && last_is_pipe > -1) { // !N |N
            List_ * temp_init = init(tokens);
            String_ * cmds_str = intercalate_string(init(temp_init), string(" "));
            List_ * cmds = map_(parse_command_boxed, tokenize(cmds_str, string(" | ")));
            free_list(temp_init);
            free_list(tokens);
            return line(cmds, FALSE, string(""), last_is_pipe, snd_last_is_bang);
        } else if (snd_last_is_pipe > -1 && last_is_bang > -1) { // |N !N
            List_ * temp_init = init(tokens);
            String_ * cmds_str = intercalate_string(init(temp_init), string(" "));
            List_ * cmds = map_(parse_command_boxed, tokenize(cmds_str, string(" | ")));
            free_list(temp_init);
            free_list(tokens);
            return line(cmds, FALSE, string(""), snd_last_is_pipe, last_is_bang);
        } else if (last_is_bang > -1) { // !N
            String_ * cmds_str = intercalate_string(init(tokens), string(" "));
            List_ * cmds = map_(parse_command_boxed, tokenize(cmds_str, string(" | ")));
            free_list(tokens);
            return line(cmds, FALSE, string(""), -1, last_is_bang);
        } else if (last_is_pipe > -1) { // |N
            String_ * cmds_str = intercalate_string(init(tokens), string(" "));
            List_ * cmds = map_(parse_command_boxed, tokenize(cmds_str, string(" | ")));
            free_list(tokens);
            return line(cmds, FALSE, string(""), last_is_pipe, -1);
        } else  { // nothing

            String_ * cmds_str = intercalate_string(tokens, string(" "));
            List_ * cmds = map_(parse_command_boxed, tokenize(cmds_str, string(" | ")));
            return line(cmds, FALSE, string(""), -1, -1);
        }
    } else if (length(tokens) == 1) {
        List_ * cmds = map_(parse_command_boxed, tokenize(head(tokens), string(" | ")));
        free_list(tokens);
        return line(cmds, FALSE, string(""), -1, -1);
    } else {
        free_list(tokens);
        return line(nil(), FALSE, string(""), -1, -1);
    }
}


Line * parse_line(String_ * str)
{
    str = trim(str);
    Maybe * result_redirection = parse_redirection(copy_string(str));
    if (result_redirection -> Nothing == FALSE) {    // redirected
        Pair * splitted = from_just(result_redirection);
        Line * result = line(fst(splitted), TRUE, snd(splitted), -1, -1);

        // cleanup
        free_pair(splitted);
        free_maybe(result_redirection);
        free_string(str);
        return result;
    } else {
        free_maybe(result_redirection);
        return parse_piping(str);
    }
}

Line * copy_line(Line * node)
{
    return line(copy__list(node -> cmds), node -> redirect, node -> target, node -> out, node -> err);
}

void print_line(Line * node)
{
    if (node -> redirect) {         // has ">"
        print__list(node -> cmds);
        printf(" > ");
        print_string(node -> target);
    } else {
        print__list(node -> cmds);
        if (!null(node -> cmds)) {
            if (node -> out != -1)
                printf(" |%d", node -> out);
            if (node -> err != -1)
                printf(" !%d", node -> err);
        }
    }
    printf("\n");
}

void free_line(Line * node)
{
    free_list(node -> cmds);
    if (node -> target)
        free_string(node -> target);
    free(node);
}

Box_ * box_line(Line * l)
{
    return box(l, (void (*)(void *))free_line, (void * (*)(void *))copy_line, (void (*)(void *))print_line);
}
