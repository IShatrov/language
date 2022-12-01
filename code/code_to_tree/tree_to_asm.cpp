#include "code_to_tree.h"

#define PRINT(...) fprintf(code, __VA_ARGS__)
#define TRY_PRINT(node) try_print(code, node, vars, n_vars)

#define DEFAULT_PRINT(text)                     \
    TRY_PRINT(LEFT(node));                      \
    TRY_PRINT(RIGHT(node));                     \
    PRINT("%s\n", #text);                       \

void tree_to_asm(my_tree *tree)
{
    assert(tree);

    FILE *code = fopen("code.txt", "w");
    assert(code);

    var_info *vars = (var_info*)calloc(MAX_VARS, sizeof(var_info));
    ssize_t n_vars = 0;

    print_asm(code, tree->root, vars, &n_vars);
    PRINT("halt\n");

    fclose(code);
    free(vars);

    return;
}

void print_asm(FILE *code, tree_node *node, var_info *vars, ssize_t *n_vars)
{
    assert(code);
    assert(node);
    assert(vars);
    assert(n_vars);

    ssize_t var_id = 0;

    if(TYPE(node) == NODE_OP)
    {
        switch(OP(node))
        {
            case OP_VAR_DECL:
                vars[*n_vars].name = (RIGHT(node)->var).name;
                vars[*n_vars].len = (RIGHT(node)->var).len;
                vars[*n_vars].id = *n_vars;

                //fwrite(vars[*n_vars].name, vars[*n_vars].len, 1, stdout);
                //putchar('\n');

                (*n_vars)++;
                break;
            case OP_ASSIGN:
                TRY_PRINT(LEFT(node));
                var_id = get_var_id(RIGHT(node), vars, n_vars);
                PRINT("pop [%lld]\n", var_id);
                break;
            case OP_GLUE:
                TRY_PRINT(LEFT(node));
                TRY_PRINT(RIGHT(node));
                break;
            case OP_READ:
                PRINT("in\n");
                var_id = get_var_id(RIGHT(node), vars, n_vars);
                PRINT("pop [%lld]\n", var_id);
                break;
            case OP_WRITE:
                var_id = get_var_id(RIGHT(node), vars, n_vars);
                PRINT("push [%lld]\n", var_id);
                PRINT("out\n");
                break;
            case OP_IF:
                print_if(code, node, vars, n_vars);
                break;
            case OP_ADD:
                DEFAULT_PRINT(add);
                break;
            case OP_SUB:
                DEFAULT_PRINT(sub);
                break;
            case OP_MUL:
                DEFAULT_PRINT(mul);
                break;
            case OP_DIV:
                DEFAULT_PRINT(div);
                break;
            case OP_SIN:
                DEFAULT_PRINT(sin);
                break;
            case OP_COS:
                DEFAULT_PRINT(cos);
                break;
            default:
                ERR_N_OP(OP(node));
        }
    }
    else if(TYPE(node) == NODE_NUM)
    {
        PRINT("push %.3lf\n", node->val);
    }
    else if(TYPE(node) == NODE_VAR)
    {
        var_id = get_var_id(node, vars, n_vars);

        PRINT("push [%lld]\n", var_id);
    }

    return;
}

void try_print(FILE *code, tree_node *node, var_info *vars, ssize_t *n_vars)
{
    assert(code);
    assert(vars);
    assert(n_vars); //no need to assert node, "if" below will check it

    if(node) print_asm(code, node, vars, n_vars);

    return;
}

ssize_t get_var_id(tree_node *node, var_info *vars, ssize_t *n_vars)
{
    assert(node);
    assert(vars);
    assert(n_vars);

    for(int vars_checked = 0; vars_checked < *n_vars; vars_checked++)
    {
        if(!strncmp(vars[vars_checked].name, (node->var).name, (node->var).len))
        {
            return vars_checked;
        }
    }

    return -1;
}

void print_if(FILE *code, tree_node *node, var_info *vars, ssize_t *n_vars)
{
    assert(code);
    assert(node);
    assert(vars);
    assert(n_vars);

    static int ifs_printed = 0, elses_printed = 0;

    PRINT("\n");

    TRY_PRINT(LEFT(node));//condition

    if(OP(RIGHT(node)) == OP_ELSE)
    {
        PRINT("push 0\n"
              "je else%d\n\n"
              "if%d:\n", elses_printed, ifs_printed);

        TRY_PRINT(RIGHT(RIGHT(node)));  //executed if condition is true
        PRINT("jmp if%dEnd\n", ifs_printed);

        PRINT("else%d:\n", elses_printed);
        TRY_PRINT(LEFT(RIGHT(node)));

        PRINT("if%dEnd:\n\n", ifs_printed);

        elses_printed++;
    }
    else
    {
        PRINT("push 0\n"
              "je if%dEnd\n\n"
              "if%d:\n", ifs_printed, ifs_printed);

        TRY_PRINT(RIGHT(node));
        PRINT("if%dEnd:\n\n", ifs_printed);
    }

    ifs_printed++;

    return;

}

#undef PRINT
#undef TRY_PRINT
#undef DEFAULT_PRINT
