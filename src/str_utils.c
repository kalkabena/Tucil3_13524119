#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include "str_utils.h"

int strtoi(const char *data, char **endptr, int base)
{
    int old_errno = errno;
    errno = 0;
    long lval = strtol(data, endptr, base);
    if (lval > INT_MAX)
    {
        errno = ERANGE;
        lval = INT_MAX;
    }
    else if (lval < INT_MIN)
    {
        errno = ERANGE;
        lval = INT_MIN;
    }
    if (errno == 0)
        errno = old_errno;
    return (int)lval;
}
