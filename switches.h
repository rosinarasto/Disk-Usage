#ifndef SWITCHES_H
#define SWITCHES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

enum switches
{
    EMPTY = 0,
    PRINT_SIZE = 1,
    SORT_BY_SIZE = 2,
    LIMITED_DEPTH = 4,
    PRINT_PERCENT = 8,
};

struct switch_items
{
    uint8_t collection;
    int64_t depth; 
};

bool parse_switches(int argc, char *argv[], struct switch_items *switches);

#endif //SWITCHES_H
