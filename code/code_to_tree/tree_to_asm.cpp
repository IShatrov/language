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

    PRINT("push 0\n"
          "push 0\n"
          "pop rax\n"
          "call main\n"
          "halt\n");

    print_prog(code, tree->root);

    fclose(code);

    return;
}

void print_prog(FILE *code, tree_node *node)
{
    assert(code);
    assert(node);

    tree_node *func = LEFT(node);

    var_info *vars = (var_info*)calloc(MAX_VARS, sizeof(var_info));
    ssize_t n_vars = 0;

    PRINT("\n");
    fwrite(func->var.name, func->var.len, 1, code);
    PRINT(":\n");

    tree_node *var = LEFT(func);

    while(var)
    {
        vars[n_vars].name = var->var.name;
        vars[n_vars].len = var->var.len;
        vars[n_vars].id = n_vars;
        n_vars++;

        var = RIGHT(var);
    }

    print_asm(code, RIGHT(func), vars, &n_vars);

    if(RIGHT(node)) print_prog(code, RIGHT(node));

    return;
}

void print_asm(FILE *code, tree_node *node, var_info *vars, ssize_t *n_vars)
{
    assert(code);
    assert(node);
    assert(vars);
    assert(n_vars);

    ssize_t var_id = 0;
    int args_passed = 0, args_found = 0;
    tree_node *arg = NULL;

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
                PRINT("pop [rax + %lld];", var_id);
                fwrite(vars[var_id].name, vars[var_id].len, 1, code);
                putc('\n', code);
                break;
            case OP_GLUE:
                TRY_PRINT(LEFT(node));
                TRY_PRINT(RIGHT(node));
                break;
            case OP_READ:
                PRINT("in\n");
                var_id = get_var_id(RIGHT(node), vars, n_vars);
                PRINT("pop [rax + %lld];", var_id);
                fwrite(vars[var_id].name, vars[var_id].len, 1, code);
                putc('\n', code);
                break;
            case OP_WRITE:
                var_id = get_var_id(RIGHT(node), vars, n_vars);
                PRINT("push [rax + %lld];", var_id);
                fwrite(vars[var_id].name, vars[var_id].len, 1, code);
                putc('\n', code);
                PRINT("out\n");
                break;
            case OP_IF:
                print_if(code, node, vars, n_vars);
                break;
            case OP_WHILE:
                print_while(code, node, vars, n_vars);
                break;
            case OP_RETURN:
                TRY_PRINT(RIGHT(node));
                PRINT("pop rbx\n"
                      "push rax\n"
                      "sub\n"
                      "push -1\n"
                      "mul\n"
                      "pop rax\n"
                      "ret\n");
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
            case OP_ABS:
                TRY_PRINT(RIGHT(node));
                PRINT("dup\n"
                      "mul\n"
                      "sqr\n");
                break;
            case OP_SQRT:
                TRY_PRINT(RIGHT(node));
                PRINT("sqr\n");
                break;
            case OP_CALL:
                PRINT("push %lld\n", *n_vars);

                args_found = 0;
                arg = RIGHT(node);
                while(arg)
                {
                    var_id = get_var_id(arg, vars, n_vars);
                    PRINT("push [rax + %lld]\n", var_id);
                    args_found++;
                    arg = RIGHT(arg);
                }

                PRINT("push %lld\n", *n_vars);

                PRINT("push rax\n"
                      "add \n"
                      "pop rax\n");

                for(args_passed = 0; args_passed < args_found; args_passed++)
                {
                    PRINT("pop [rax + %d]\n", args_found - args_passed - 1);
                }

                PRINT("call ");
                fwrite(LEFT(node)->var.name, LEFT(node)->var.len, 1, code);
                PRINT("\n"
                      "push rbx\n");

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

        PRINT("push [rax + %lld];", var_id);
        fwrite(vars[var_id].name, vars[var_id].len, 1,code);
        putc('\n', code);
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
        ssize_t len = (node->var.len);

        if(len < vars[vars_checked].len) len = vars[vars_checked].len;

        if(!strncmp(vars[vars_checked].name, (node->var).name, len))
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

void print_while(FILE *code, tree_node *node, var_info *vars, ssize_t *n_vars)
{
    assert(code);
    assert(node);
    assert(vars);
    assert(n_vars);

    static int whiles_printed = 0;

    PRINT("\nwhile%d:\n", whiles_printed);
    TRY_PRINT(LEFT(node));//condition
    PRINT("push 0\n"
          "je while%dEnd\n\n", whiles_printed);

    TRY_PRINT(RIGHT(node));
    PRINT("jmp while%d\n"
          "while%dEnd:\n\n", whiles_printed, whiles_printed);

    whiles_printed++;

    return;
}

#undef PRINT
#undef TRY_PRINT
#undef DEFAULT_PRINT
