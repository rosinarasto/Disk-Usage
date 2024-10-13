#include "draw.h"
#include "error.h"
#include "switches.h"
#include "tree.h"

#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[])
{
    if (argc == 1) {
        print_error_msg(MISSING_ARGUMENTS);
        return EXIT_FAILURE;
    }

    struct switch_items switches = { .depth = 0, .collection = EMPTY};
    if (!parse_switches(argc, argv, &switches))
        return EXIT_FAILURE;

    struct tree tree = { .error = false };
    memset(&tree.root, 0, sizeof(struct node));
    enum file_type root_type = read_tree(argv[argc - 1], &tree);

    switch (root_type) {
    case IS_ERROR:
        return EXIT_FAILURE;
    case IS_UNKNOW:
        return EXIT_SUCCESS;
    default:
        draw_tree(tree, switches);        
    }

    return EXIT_SUCCESS;
}

