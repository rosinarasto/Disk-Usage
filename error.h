#ifndef ERROR_H
#define ERROR_H


enum error_type
{
    INVALID_DEPTH,
    DUPLICATED_OR_INVALID_SWITCH,
    INVALID_STAT,
    ALLOCATION_FAILED,
    OPEN_FAILED,
    MISSING_ARGUMENTS,
};

void print_error_msg(enum error_type);

#endif // ERROR_H
