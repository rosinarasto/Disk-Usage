#ifndef TREE_H
#define TREE_H

#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


enum file_type
{
    IS_UNKNOW,
    IS_DIR,
    IS_FILE,
    IS_ERROR,
};

struct node
{
    size_t size;
    size_t index;
    blkcnt_t data_block;
    off_t data_size;
    mode_t mode;
    char name[256];
    char path[4096];
    char prefix[8192];
    char error[3];
    struct node *succs;
};

struct tree
{
    struct node root;
    bool error;
};


bool read_directory(char *path, struct node *node, struct tree *tree);

enum file_type read_tree(char *path, struct tree *tree);

#endif  //TREE_H
