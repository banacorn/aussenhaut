#include "violin.h"

var head(var xs)
{
    if (len(xs) == 0) {
        perror("head on empty list");
        return NULL;
    } else {
        return get(xs, $I(0));
    }
}


var last(var xs)
{
    if (len(xs) == 0) {
        perror("last on empty list");
        return NULL;
    } else {
        return get(xs, $I(len(xs) - 1));
    }
}


var take(var xs, int n)
{
    int i = 0;
    var result = new(List, iter_type(xs));
    foreach (x in xs) {
        if (i < n) {
            push(result, get(xs, $I(i)));
        }
        i++;
    }
    return result;
}

var drop(var xs, int n)
{
    int i = 0;
    var result = new(List, iter_type(xs));
    foreach (x in xs) {
        if (i >= n) {
            push(result, get(xs, $I(i)));
        }
        i++;
    }
    return result;
}
