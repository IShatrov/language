#include "code_to_tree.h"

int main(int argc, const char *argv[])
{
    const char *src_filename = "src_code.txt";
    if(argc > 1) src_filename = argv[1];

    char *src_code = NULL;
    lexic_cell *tokens = lexic_analysis(src_filename, &src_code);

    my_tree tree;
    parse_src_code(&tree, tokens);

    //tree_dump(&tree);

    tree_to_asm(&tree);

    free(src_code);
    free(tokens);
    tree_dtor(&tree);

    return 0;
}
