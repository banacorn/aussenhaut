#include "exec.h"
#include "parser.h"

var env_path(var env)
{
    if (mem(env, $S("PATH"))) {
        return tokenize(get(env, $S("PATH")), $S(":"));
    } else {
        perror("no PATH in env");
        return NULL;
    }
}


var search_exec(var env, var name)
{
    var prefixes = env_path(env);

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
