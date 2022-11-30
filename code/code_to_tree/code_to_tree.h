#ifndef CODE_TO_TREE
#define CODE_TO_TREE

#include "tree.h"
#include "../lang_dsl.h"

//parser.cpp

size_t get_file_size(FILE *stream);

char* read_text(FILE *stream);

const char* parse_src_code(const char *filename, my_tree *tree);

tree_node* get_math(my_tree *tree, const char **str);

tree_node* get_num(my_tree *tree, const char **str);

tree_node* get_expr(my_tree *tree, const char **str);

tree_node* get_turn(my_tree *tree, const char **str);

tree_node* get_prim(my_tree *tree, const char **str);

tree_node* get_pow(my_tree *tree, const char **str);

tree_node* get_unary(my_tree *tree, const char **str);

tree_node* get_arg(my_tree *tree, const char **str);

tree_node* get_var(my_tree *tree, const char **str);

#endif
