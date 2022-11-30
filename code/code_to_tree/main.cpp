#include "code_to_tree.h"

int main(int argc, const char *argv[])
{
//    const char *str = "12 - 8->  aaa";
//    my_tree tree = get_math(&str);
//    tree_dump(&tree);

    const char *src_filename = "src_code.txt";
    if(argc > 1) src_filename = argv[1];

    my_tree tree;
    const char *src_code = parse_src_code(src_filename, &tree);

    tree_dump(&tree);

    free((void*)src_code);

    return 0;
}
