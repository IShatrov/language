#ifndef TREE
#define TREE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

#include "../err_msgs.h"

const char TREE_BYPASS = 1;

const int DEFAULT_TREE_CAP = 20;

enum poison
{
    CAP_POSION = -1,
    SIZE_POSION = -1,
    TYPE_POISON = -1,
};

enum tree_visitor_modes
{
    PRE_ORDER,
    IN_ORDER,
    POST_ORDER,
};

enum node_types
{
    NODE_OP,
    NODE_VAR,
    NODE_NUM,
};

#define DEF_OP(name, text, ...)\
    OP_##name,
enum operators
{
    #include "../lang_operators.h"
};
#undef DEF_OP

typedef double tree_elmt;

typedef struct
{
    const char *name;
    ssize_t len;
} var_info;

typedef struct tree_node tree_node;

struct tree_node
{
    char type;

    tree_node *l_child;
    tree_node *r_child;

    union
    {
        double val;
        enum operators op;
        var_info var;
    };
};

typedef struct
{
    tree_node *root;

    ssize_t size;
    ssize_t capacity;

    tree_node *data;
} my_tree;


//
//! @param[in] dest Uninitialised tree.
//! @param[in] capacity Tree capacity.
//! @param[in] root_val Root value.
//! @param[out] dest Initialised tree.
//! @return Returns pointer to root.
//! @brief Initialises tree.
tree_node *tree_ctor(my_tree *dest, ssize_t capacity);

void tree_realloc(my_tree *tree, ssize_t new_cap);

//
//! @param[in] tree Pointer to tree.
//! @param[in] val Value to add.
//! @return Returns pointer to new node.
//! @brief Creates node.
tree_node* node_ctor(my_tree *tree);

//! @param[in] tree Pointer to tree.
//! @brief Destroys tree.
void tree_dtor(my_tree *tree);

//! @param[in] tree Pointer to tree.
//! @brief Creates tree dump.
void tree_dump(my_tree *tree);

//! @param[in] node Pointer to tree node.
//! @param[in] gr_viz File to print graphviz code to.
//! @brief Prints graphviz code to file.
void print_branch(tree_node *node, FILE *gr_viz);

//! @param[in] tree Pointer to tree.
//! @brief Frees tree nodes and fills them with poison.
void tree_free(my_tree *tree);

//! @param[in] node Pointer to node.
//! @brief Frees tree node and fills it with poison.
void node_free(tree_node *node);

//! @param[in] node Node to start from.
//! @param[in] mode Pre order, in order or post order.
//! @param[in] action Action to perform.
//! @brief visits tree.
void tree_visitor(tree_node *node, const char mode, void (*action)(tree_node *node));

#endif

