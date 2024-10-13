#include "draw.h"
#include "switches.h"
#include "tree.h"

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *find_range(uint64_t number, uint64_t *divisor)
{
    if (number <= (1023)) {
        *divisor = 1;
        return "B   ";
    }

    if (number <= (((uint64_t) 2 << 19) - 1)) {
        *divisor = 1024;
        return "KiB ";
    } 

    if (number <= (((uint64_t) 2 << 29) - 1)) {
        *divisor = ((uint64_t) 2 << 19);
        return "MiB ";
    }

    if (number <= (((uint64_t) 2 << 39) - 1)) {
        *divisor = ((uint64_t) 2 << 29);
        return "GiB ";
    }

    if (number <= (((uint64_t) 2 << 49) - 1)) {
        *divisor = ((uint64_t) 2 << 39);
        return "TiB ";
    }

    if (number <= (((uint64_t) 2 << 59) - 1)) {
        *divisor = ((uint64_t) 2 << 49);
        return "PiB ";
    }

    assert(false);
}


int name_cmp(const void *a, const void *b)
{
    struct node *node1 = (struct node *) a;
    struct node *node2 = (struct node *) b;
    if (strcasecmp(node1->name, node2->name))
        return strcasecmp(node1->name, node2->name);

    return strcmp(node1->name, node2->name);
}


int size_cmp(const void *a, const void *b)
{
    struct node *node1 = (struct node *) a;
    struct node *node2 = (struct node *) b;

    return ((node2->data_size - node1->data_size) != 0) ? (node2->data_size - node1->data_size) : name_cmp(node1, node2);
}


int block_cmp(const void *a, const void *b)
{
    struct node *node1 = (struct node *) a;
    struct node *node2 = (struct node *) b;

    return ((node2->data_block - node1->data_block) != 0) ? (node2->data_block - node1->data_block) : name_cmp(node1, node2);
}


void sort_succs(struct node *node, struct switch_items switches)
{
    assert(node != NULL);
    if (((switches.collection & PRINT_SIZE) != 0) && ((switches.collection & SORT_BY_SIZE) != 0))
        qsort(&(node->succs[0]), node->size, sizeof(struct node), size_cmp);
    else if ((switches.collection & SORT_BY_SIZE) != 0)
        qsort(&(node->succs[0]), node->size, sizeof(struct node), block_cmp);
    else
        qsort(&(node->succs[0]), node->size, sizeof(struct node), name_cmp);    
}


void print_item(struct switch_items switches, struct node node, size_t index, struct tree tree)
{
    if ((switches.collection & PRINT_PERCENT) == 0) {
        char unit[6];
        uint64_t divisor = 0; 
        uint64_t number = ((switches.collection & PRINT_SIZE) == 0) ? (node.succs[index].data_block * 512) : (node.succs[index].data_size);
        strcpy(unit, find_range(number, &divisor));
        if (index == node.size - 1)
            printf("%s%6.1f %s%s\\-- %s\n", node.succs[index].error, floor(1.0 * number / divisor * 10) / 10, unit, node.prefix, node.succs[index].name);
        else 
            printf("%s%6.1f %s%s|-- %s\n", node.succs[index].error, floor(1.0 * number / divisor * 10) / 10, unit, node.prefix, node.succs[index].name);     
    } 
    else {
        uint64_t divisor = ((switches.collection & PRINT_SIZE) == 0) ? (tree.root.data_block * 512) : (tree.root.data_size); 
        uint64_t number = ((switches.collection & PRINT_SIZE) == 0) ? (node.succs[index].data_block * 512) : (node.succs[index].data_size);
        if (index == node.size - 1)
            printf("%s%5.1f%s %s\\-- %s\n", node.succs[index].error, floor(1.0 * number / divisor * 1000) / 10, "%", node.prefix, node.succs[index].name);
        else 
            printf("%s%5.1f%s %s|-- %s\n", node.succs[index].error, floor(1.0 * number / divisor * 1000) / 10, "%", node.prefix, node.succs[index].name);
    }

}


void draw_rec(struct node node, struct switch_items switches, struct tree tree, int64_t depth)
{
    sort_succs(&node, switches);

    for (size_t i = 0; i < node.size; i++) {

        if ((((switches.collection & LIMITED_DEPTH) != 0) && depth >= 0) || (switches.collection & LIMITED_DEPTH) == 0) {
            if (tree.error && node.succs[i].error[0] == '\0')
                strcpy(node.succs[i].error, "  ");
            print_item(switches, node, i, tree);
        }

        if (S_ISDIR(node.succs[i].mode)) {
            if (i == node.size - 1)
                strcat(strcpy(node.succs[i].prefix, node.prefix), "    ");
            else
                strcat(strcpy(node.succs[i].prefix, node.prefix), "|   ");
            draw_rec(node.succs[i], switches, tree, depth - 1);
        }

    }

    free(node.succs);
}


void draw_tree(struct tree tree, struct switch_items switches)
{
    if ((switches.collection & PRINT_PERCENT) == 0) {
        uint64_t number = ((switches.collection & PRINT_SIZE) == 0) ? (tree.root.data_block * 512) : (tree.root.data_size);
        uint64_t divisor = 0;
        char unit[5];
        strcpy(unit, find_range(number, &divisor));
        printf("%s%6.1f %s%s\n", tree.root.error, floor(1.0 * number / divisor * 10) / 10, unit, tree.root.name);
    }
    else
        printf("%s%5.1f%s %s\n", tree.root.error, 100.0, "%", tree.root.name);
    if (S_ISDIR(tree.root.mode)) {
        tree.root.prefix[0] = '\0';
        draw_rec(tree.root, switches, tree, switches.depth - 1);
    }
}
