#define NEW_NODE (new_node = node_ctor(tree))

#define ADD_L(new_node) node->l_child = new_node

#define ADD_R(new_node) node->r_child = new_node

#define LEFT(node) ((node)->l_child)

#define RIGHT(node) ((node)->r_child)

#define TYPE(node) ((node)->type)

#define VAL(node) ((node)->val)

#define DIFF(target) diff(tree, NEW_NODE, target, degenerator)

#define COPY(src) subtree_cpy(tree, NEW_NODE, src)

#define NODE_COPY(src) subtree_cpy(tree, node, src)

#define EQUALS(n1, n2) (are_doubles_equal((n1), (n2)))

#define OP(node) ((node)->op)

#define IS_NUM(node) ((TYPE(node) == NODE_NUM)?(1):(0))

#define CUT(node)        \
    LEFT(node) = NULL;   \
    RIGHT(node) = NULL;  \

#define PRINT(arg)                              \
    if(arg->priority < node->priority)          \
    {                                           \
        TEX_PRINT("(");                         \
        tex_print_math(tex, arg);               \
        TEX_PRINT(")");                         \
    }                                           \
    else tex_print_math(tex, arg);

#define FIND(node, var) tree_find(node, var)

#define NEW_OP_NODE(name)              \
    NEW_NODE;                          \
    new_node->type = NODE_OP;          \
    new_node->op = OP_##name;          \

#define NEW_NUM_NODE(arg)              \
    NEW_NODE;                          \
    new_node->type = NODE_NUM;         \
    new_node->val = arg;               \

#define FILL_NODE(num)           \
    TYPE(node) = NODE_NUM;       \
    VAL(node) = (num);           \
    CUT(node);
