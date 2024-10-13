#include "error.h"
#include "tree.h"

#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


bool realloc_succs(struct node *node) {
    assert(node != NULL);
    node->size *= 2;
    struct node *temp = realloc(node->succs, node->size * sizeof(struct node));
    if (temp == NULL)
        return false;

    node->succs = temp;
    return true;
}


bool init_directory(char *path, struct node *node, struct tree *tree)
{
    assert(path != NULL);
    assert(node != NULL);
    assert(tree != NULL);
    node->error[0] = '\0';
    node->index = 0;
    node->size = 6;
    node->data_block = 0;
    node->data_size = 0;
    node->succs = calloc(node->size, sizeof(struct node));
    strcat(node->path, "/");
    if (node->succs == NULL) {
        print_error_msg(ALLOCATION_FAILED);
        return false;
    }

    struct stat stats;
    if (stat(path, &stats) == -1) {
        print_error_msg(INVALID_STAT);
        return false;
    }

    node->data_block = stats.st_blocks;
    node->data_size = stats.st_size;
    node->mode = stats.st_mode;

    return true;
}


bool file_type(struct node *node, struct tree *tree, struct stat stats)
{
    assert(node != NULL);
    assert(tree != NULL);

    switch (node->succs[node->index].mode & S_IFMT) {
    case S_IFREG: {
        node->data_block += stats.st_blocks;
        node->data_size += stats.st_size;
        node->succs[node->index].data_block = stats.st_blocks;
        node->succs[node->index].data_size = stats.st_size;   
        break;
    }
    case S_IFDIR: {
        if (!read_directory(node->succs[node->index].path, &(node->succs[node->index]), tree))
            return false;
        if (strcmp(node->error, "? "))
            strcpy(node->error, node->succs[node->index].error);
        node->data_block += node->succs[node->index].data_block;
        node->data_size += node->succs[node->index].data_size;
        break;
    }
    default: {
        --node->index;
        break;
    }
    }   

    return true;
}


bool init_succ(struct node *node, struct tree *tree, struct dirent *dir_item, struct stat *stats)
{
    assert(node != NULL);
    assert(dir_item != NULL);
    assert(tree != NULL);
    assert(stats != NULL);
    strcpy(node->succs[node->index].name, dir_item->d_name);
    strcpy(node->succs[node->index].path, node->path);
    strcat(node->succs[node->index].path, dir_item->d_name);
    node->succs[node->index].error[0] = '\0';
    if (stat(node->succs[node->index].path, stats) == -1) {
        print_error_msg(INVALID_STAT);
        return false;
    }

    node->succs[node->index].mode = stats->st_mode;

    return true;
}


bool read_directory(char *path, struct node *node, struct tree *tree)
{
    assert(path != NULL);
    assert(node != NULL);
    assert(tree != NULL);
    if (!init_directory(path, node, tree)) {
        return false;
    }

    DIR *dir = NULL;
    if ((dir = opendir(path)) == NULL) {
        strcpy(node->error, "? ");
        tree->error = true;
        print_error_msg(OPEN_FAILED);
        node->size = 0;
        return true;
    }

    struct dirent *dir_item = NULL;
    struct stat stats;

    while ((dir_item = readdir(dir)) != NULL) {
        if (!strcmp(dir_item->d_name, ".") || !strcmp(dir_item->d_name, ".."))
            continue;
        if (node->index == node->size && !realloc_succs(node)) {
            print_error_msg(ALLOCATION_FAILED);
            closedir(dir);
            return false;
        }

        if (!init_succ(node, tree, dir_item, &stats) || !file_type(node, tree, stats))
            return false;

        node->index++;
    }

    node->size = node->index;
    closedir(dir);
    return true;
}


enum file_type read_tree(char *path, struct tree *tree)
{
    assert(path != NULL);
    assert(tree != NULL);
    strcpy(tree->root.name, path);
    strcpy(tree->root.path, path);
    struct stat temp;

    if (stat(path, &temp) == -1) {
        print_error_msg(INVALID_STAT);
        return IS_ERROR;
    }

    switch (temp.st_mode & S_IFMT) {
    case S_IFDIR:
        return ((read_directory(path, &(tree->root), tree)) ? (IS_DIR) : (IS_ERROR));
    case S_IFREG: {
        tree->root.mode = temp.st_mode;
        tree->root.data_block = temp.st_blocks;
        tree->root.data_size = temp.st_size;
        return IS_FILE;
    }
    default:
        return IS_UNKNOW;
    }

    assert(false);
}
