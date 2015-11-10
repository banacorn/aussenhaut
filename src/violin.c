#include "violin.h"

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
