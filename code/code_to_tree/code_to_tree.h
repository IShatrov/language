#ifndef CODE_TO_TREE
#define CODE_TO_TREE

#include "tree.h"
#include "../lang_dsl.h"

const int MAX_VARS = 100;


//tree_to_asm.cpp

void tree_to_asm(my_tree *tree);

void print_asm(FILE *code, tree_node *node, var_info *vars, ssize_t *n_vars);

void try_print(FILE *code, tree_node *node, var_info *vars, ssize_t *n_vars);

ssize_t get_var_id(tree_node *node, var_info *vars, ssize_t *n_vars);

void print_if(FILE *code, tree_node *node, var_info *vars, ssize_t *n_vars);

void print_while(FILE *code, tree_node *node, var_info *vars, ssize_t *n_vars);

//parser.cpp

lexic_cell* lexic(const char *filename, char **text);

void lexic_dump(const lexic_cell *array, size_t size);

size_t get_file_size(FILE *stream);

char* read_text(FILE *stream);

const char* parse_src_code(const char *filename, my_tree *tree);

tree_node* get_math(my_tree *tree, const char **str);

tree_node* get_num(my_tree *tree, const char **str);

tree_node* get_expr(my_tree *tree, const char **str);

tree_node* get_turn(my_tree *tree, const char **str);

tree_node* get_prim(my_tree *tree, const char **str);

tree_node* get_unary(my_tree *tree, const char **str);

tree_node* get_arg(my_tree *tree, const char **str);

tree_node* get_var(my_tree *tree, const char **str);

tree_node* get_scope(my_tree *tree, const char **str);

tree_node* get_condition(my_tree *tree, const char **str);

tree_node* get_op(my_tree *tree, const char **str);

#endif
