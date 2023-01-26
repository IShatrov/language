#ifndef CODE_TO_TREE
#define CODE_TO_TREE

#include "tree.h"
#include "../lang_dsl.h"

const int MAX_VARS = 100;

const int ARRAY_SIZE_MULTIPLIER = 2;

//tree_to_asm.cpp

//! @param[in] tree Tree to convert.
//! @brief Converts tree to assembler code.
void tree_to_asm(my_tree *tree);

//! @param[in] code File to write to.
//! @param[in] node Tree node to convert.
//! @brief Converts tree node to function.
void func_to_asm(FILE *code, tree_node *node);

//! @param[in] code File to write to.
//! @param[in] node Tree node to print.
//! @param[in] vars Variable array.
//! @param[in] n_vars Amount of variables.
//! @brief Prints assembler code to file.
void print_asm(FILE *code, tree_node *node, var_info *vars, ssize_t *n_vars);

//! @param[in] code File to write to.
//! @param[in] node Tree node to print.
//! @param[in] vars Variable array.
//! @param[in] n_vars Amount of variables.
//! @brief If node is not NULL, prints assembler code to file.
void try_print(FILE *code, tree_node *node, var_info *vars, ssize_t *n_vars);

//! @param[in] node Node to check.
//! @param[in] vars Variable array.
//! @param[in] n_vars Amount of variables.
//! @return Returns index of variable in node, if variable with such name was not found returns -1.
//! @brief Gets variable index.
ssize_t get_var_id(tree_node *node, var_info *vars, ssize_t *n_vars);

//! @param[in] code File to write to.
//! @param[in] node Tree node to convert.
//! @param[in] vars Variable array.
//! @param[in] n_vars Amount of variables.
//! @brief Converts tree node to if.
void if_to_asm(FILE *code, tree_node *node, var_info *vars, ssize_t *n_vars);

//! @param[in] code File to write to.
//! @param[in] node Tree node to convert.
//! @param[in] vars Variable array.
//! @param[in] n_vars Amount of variables.
//! @brief Converts tree node to while.
void while_to_asm(FILE *code, tree_node *node, var_info *vars, ssize_t *n_vars);

//! @param[in] code File to write to.
//! @param[in] node Tree node to convert.
//! @param[in] vars Variable array.
//! @param[in] n_vars Amount of variables.
//! @brief Converts tree node to function call.
void call_to_asm(FILE *code, tree_node *node, var_info *vars, ssize_t *n_vars);

//parser.cpp

//! @param[in] filename Filename.
//! @param[in] text File text.
//! @return Returns array of lexic cells.
//! @brief Reads text from filename and analyses it.
lexic_cell* lexic_analysis(const char *filename, char **text);

//! @param[in] str String to check.
//! @return Returns type of bracket, if *str is not a bracket returns 0/
//! @brief Detects brackets.
int detect_bracket(const char *str);

//! @param[in] array Array to dump.
//! @param[in] size Size of array.
//! @brief Dumps array.
void lexic_dump(const lexic_cell *array, size_t size);

//! @param[in] stream File to examine.
//! @return Returns amount of characters in stream.
//! @brief Gets amount of characters in stream.
size_t get_file_size(FILE *stream);

//! @param[in] stream File to read from.
//! @return Returns pointer to char array.
//! @brief Reads text from stream and stores it as char array.
char* read_text(FILE *stream);

//! @param[in] tree Tree to store at.
//! @param[in] lexic Lexic array.
//! @brief Parses lexic array to tree.
void parse_src_code(my_tree *tree, lexic_cell *lexic);

tree_node* get_func(my_tree *tree, lexic_cell **lexic);

tree_node* get_func_args(my_tree *tree, lexic_cell **lexic);

tree_node* get_math(my_tree *tree, lexic_cell **lexic);

tree_node* get_num(my_tree *tree, lexic_cell **lexic);

tree_node* get_expr(my_tree *tree, lexic_cell **lexic);

tree_node* get_turn(my_tree *tree, lexic_cell **lexic);

tree_node* get_prim(my_tree *tree, lexic_cell **lexic);

tree_node* get_unary(my_tree *tree, lexic_cell **lexic);

tree_node* get_arg(my_tree *tree, lexic_cell **lexic);

tree_node* get_var(my_tree *tree, lexic_cell **lexic);

tree_node* get_scope(my_tree *tree, lexic_cell **lexic);

tree_node* get_condition(my_tree *tree, lexic_cell **lexic);

tree_node* get_op(my_tree *tree, lexic_cell **lexic);

#endif
