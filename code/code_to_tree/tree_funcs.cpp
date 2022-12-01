#include "tree.h"

#define GO_DEEPER(where) if(where) action(where)
#define GR_VIZ_PRINT(...) fprintf(gr_viz, __VA_ARGS__)

tree_node* tree_ctor(my_tree *dest, ssize_t capacity)
{
    assert(dest);
    assert(capacity > 0);

    dest->data = (tree_node*)calloc(capacity, sizeof(tree_node));
    assert(dest->data);

    dest->size = 1;
    dest->capacity = capacity;

    tree_node *root_ptr = node_ctor(dest);

    root_ptr->l_child = NULL;
    root_ptr->r_child = NULL;

    dest->root = root_ptr;

    return root_ptr;
}

void tree_realloc(my_tree *tree, ssize_t new_cap)
{
    assert(tree);
    assert(new_cap > tree->capacity);

    tree->data = (tree_node*)realloc(tree->data, new_cap*sizeof(tree_node));
    assert(tree->data);

    tree->capacity = new_cap;

    return;
}

tree_node* node_ctor(my_tree *tree)
{
    assert(tree);
    assert(tree->data);

    if(tree->size == tree->capacity) tree_realloc(tree, 2*(tree->capacity));

    tree->data[tree->size].l_child = NULL;
    tree->data[tree->size].r_child = NULL;

    tree->data[tree->size].type = TYPE_POISON;

    (tree->size)++;

    return &(tree->data[tree->size - 1]);
}

void tree_dtor(my_tree *tree)
{
    assert(tree);

    tree_free(tree);

    tree->root = NULL;

    tree->size = SIZE_POSION;
    tree->capacity = CAP_POSION;

    free(tree->data);
    tree->data = NULL;

    return;
}

void tree_dump(my_tree *tree)
{
    assert(tree);

    FILE *gr_viz = fopen("graphviz_code.txt", "w");
    assert(gr_viz);

    GR_VIZ_PRINT("digraph dump\n{\n\t"
                    "node[shape = \"record\", style = \"rounded\"];\n\n");

    print_branch(tree->root, gr_viz);

    putc('}', gr_viz);

    fclose(gr_viz);

    system("dot.exe -T png -o dump_pic.png graphviz_code.txt");
    system("dump_pic.png");

    return;
}

                                                                                       \
#define DEF_OP(name, text, ...)                                                        \
    case(OP_##name):                                                                   \
        GR_VIZ_PRINT("\t" "\"%p\"[label = \"{op: %s}\"];\n\n", node, #text);           \
        break;

void print_branch(tree_node *node, FILE *gr_viz)
{
    assert(node);
    assert(gr_viz);

    if(node->l_child)
    {
        GR_VIZ_PRINT("\t" "\"%p\"->\"%p\";\n",
            node, node->l_child);

        print_branch((tree_node*)node->l_child, gr_viz);
    }

    if(node->r_child)
    {
        GR_VIZ_PRINT("\t" "\"%p\"->\"%p\";\n", node, node->r_child);

        print_branch((tree_node*)node->r_child, gr_viz);
    }

    switch(node->type)
    {
        case NODE_NUM:
            GR_VIZ_PRINT("\t" "\"%p\"[label = \"{num: %.3lf}\"];\n\n", node, node->val);
            break;
        case NODE_VAR:
            GR_VIZ_PRINT("\t" "\"%p\"[label = \"{var: ", node);
            fwrite((node->var).name, (node->var).len, 1, gr_viz);
            GR_VIZ_PRINT("}\"];\n\n");
            break;
        case NODE_OP:
            switch(node->op)
            {
                #include "../lang_operators.h"
                default:
                    ERR_N_OP(node->op);
            }
            break;
        default:
            ERR_TYPE(node->op);

    }
    return;
}
#undef DEF_OP

void tree_free(my_tree *tree)
{
    assert(tree);

    tree_visitor(tree->root, POST_ORDER, &node_free);

    return;
}

void node_free(tree_node *node)
{
    assert(node);

    node->type = TYPE_POISON;
    if(node->l_child) node->l_child = NULL;
    if(node->r_child) node->r_child = NULL;

    return;
}

void tree_visitor(tree_node *node, const char mode, void (*action)(tree_node *node))
{
    assert(node);
    assert(action);

    switch(mode)
    {
        case PRE_ORDER:
            action((tree_node*)node);
            GO_DEEPER((tree_node*)node->l_child);
            GO_DEEPER((tree_node*)node->r_child);
            break;
        case IN_ORDER:
            GO_DEEPER((tree_node*)node->l_child);
            action((tree_node*)node);
            GO_DEEPER((tree_node*)node->r_child);
            break;
        case POST_ORDER:
            GO_DEEPER((tree_node*)node->l_child);
            GO_DEEPER((tree_node*)node->r_child);
            action((tree_node*)node);
            break;
        default:
            return;
    }

    return;
}

#undef GO_DEEPER
#undef GR_VIZ_PRINT
