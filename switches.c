#include "error.h"
#include "switches.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


bool valid_switch(const char *name, struct switch_items *switches)
{
    if (!strcmp(name, "-a") && ((switches->collection & PRINT_SIZE) == 0)) {
        switches->collection |= PRINT_SIZE;
        return true;
    }

    if (!strcmp(name, "-s") && ((switches->collection & SORT_BY_SIZE) == 0)) {
        switches->collection |= SORT_BY_SIZE;
        return true;
    }

    if (!strcmp(name, "-p") && ((switches->collection & PRINT_PERCENT) == 0)) {
        switches->collection |= PRINT_PERCENT;
        return true;
    }

    print_error_msg(DUPLICATED_OR_INVALID_SWITCH);
    return false;
}


bool parse_switches(int argc, char *argv[], struct switch_items *switches)
{
    assert(switches != NULL);
    int index = 1;

    while (index < (argc - 1)) {
        if (index < (argc - 2) && (!strcmp(argv[index], "-d") && (switches->collection & LIMITED_DEPTH) == 0)) {
            char *end;
            switches->depth = strtol(argv[index + 1], &end, 10);
            if (*end != '\0' || switches->depth < 0) {
                print_error_msg(INVALID_DEPTH);
                return false;
            }
            switches->collection |= LIMITED_DEPTH;
            ++index;
        } else if (!valid_switch(argv[index], switches))
            return false;
        ++index;
    }

    return true;
}
