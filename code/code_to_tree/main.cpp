#include "code_to_tree.h"

int main(int argc, const char *argv[])
{
    const char *src_filename = "src_code.txt";
    if(argc > 1) src_filename = argv[1];

    my_tree tree;
    const char *src_code = parse_src_code(src_filename, &tree);

    tree_dump(&tree);

    tree_to_asm(&tree);

    free((void*)src_code);
    tree_dtor(&tree);

    return 0;
}
