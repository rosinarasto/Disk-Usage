#include "error.h"

#include <stdio.h>
#include <stdlib.h>


const char *chose_error_msg(enum error_type error_name)
{
    switch (error_name) {
    case INVALID_DEPTH:
        return "Parameter of -d switch is invalid.\n";
    case DUPLICATED_OR_INVALID_SWITCH:
        return "Duplicated or invalid switch.\n";
    case INVALID_STAT:
        return "Fucntion \"stat()\" failed.\n";
    case ALLOCATION_FAILED:
        return "Allocation function failed.\n";
    case OPEN_FAILED:
        return "Folder cannot be opened.\n";
    case MISSING_ARGUMENTS:
        return "Missing arguments.\n";
    default:
        return "Unknown error.\n";
    }

}

void print_error_msg(enum error_type error_name)
{
    fprintf(stderr, "%s", chose_error_msg(error_name));
}
