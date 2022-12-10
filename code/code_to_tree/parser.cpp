#include "code_to_tree.h"

#define SKIP_SPACES(text) while(isspace(*text)) text++

#define OP_CMP(op) (!strncmp(#op, *str, strlen(#op)))

#define STORE_BRACKET(bracket_type)                 \
    ans[tokens_found].type = CELL_BRACKET;          \
    ans[tokens_found].bracket = bracket_type;       \
    str++;

#define DEF_OP(name, text, ...)                  \
    else if(!strncmp(#text, str, strlen(#text))) \
    {                                            \
        str += strlen(#text);                    \
                                                 \
        ans[tokens_found].type = CELL_OP;        \
        ans[tokens_found].op = OP_##name;        \
    }

#define GR_VIZ_PRINT(...) fprintf(gr_viz, __VA_ARGS__)

lexic_cell* lexic(const char *filename, char **text)
{
    assert(filename);
    assert(text);

    FILE *src_code = fopen(filename, "r");
    assert(src_code);

    *text = read_text(src_code);
    fclose(src_code);

    const char *str = *text;

    SKIP_SPACES(str);

    lexic_cell *ans = (lexic_cell*)calloc(DEFAULT_ARR_SIZE, sizeof(lexic_cell));

    size_t tokens_found = 0, current_size = DEFAULT_ARR_SIZE;

    while(*str)
    {
        //printf("%s\n\n\n\n", str);
        if(tokens_found == current_size)
        {
            current_size *= 2;
            ans = (lexic_cell*)realloc(ans, current_size*sizeof(lexic_cell));
            assert(ans);
        }

        char *end = NULL;

        double val = strtod(str, &end);

        if(str != end)
        {
            str = end;

            ans[tokens_found].type = CELL_NUM;
            ans[tokens_found].val = val;

            SKIP_SPACES(str);
            tokens_found++;

            continue;
        }

        if(0);    //for else if in macro
            #include "../lang_operators.h"
        else if(*str == '(')
        {
            STORE_BRACKET(OPEN_ROUND);
        }
        else if(*str == ')')
        {
            STORE_BRACKET(CLOSE_ROUND);
        }
        else if(*str == '{')
        {
            STORE_BRACKET(OPEN_CURLY);
        }
        else if(*str == '}')
        {
            STORE_BRACKET(CLOSE_CURLY);
        }
        else
        {
            ans[tokens_found].type = CELL_NAME;
            ans[tokens_found].var_or_func.name = str;

            ssize_t len = 0;
            while(isalnum(*str))
            {
                str++;
                len++;
            }

            ans[tokens_found].var_or_func.len = len;
        }

        SKIP_SPACES(str);
        tokens_found++;
    }

    lexic_dump(ans, tokens_found);

    return ans;
}
#undef DEF_OP

const char* parse_src_code(const char *filename, my_tree *tree)
{
    assert(filename);
    assert(tree);

    FILE *src_code = fopen(filename, "r");
    assert(src_code);

    const char *text_ptr = read_text(src_code);
    fclose(src_code);

    const char *str = text_ptr;

    SKIP_SPACES(str);

    tree_ctor(tree, DEFAULT_TREE_CAP);

    tree->root = get_scope(tree, &str);//this is where parsing starts

    SKIP_SPACES(str);

    assert(*str == '\0');

    return text_ptr;
}

tree_node* get_math(my_tree *tree, const char **str)
{
    SKIP_SPACES((*str));

    tree_node *expr = get_expr(tree, str);

    SKIP_SPACES((*str));

    if(OP_CMP(->))
    {
        *str += strlen("->");
        SKIP_SPACES((*str));

        tree_node *new_node;
        NEW_OP_NODE(ASSIGN);

        tree_node *ans = new_node;

        RIGHT(ans) = get_var(tree, str);
        LEFT(ans) = expr;

        return ans;
    }
    else
    {
        return expr;
    }
}

tree_node* get_num(my_tree *tree, const char **str)
{
    SKIP_SPACES((*str));

    const char *s_old = *str;
    char *end = NULL;

    double val = strtod(*str, &end);
    *str = end;

    if(*str != s_old)
    {
        tree_node *new_node;
        NEW_NUM_NODE(val);

        SKIP_SPACES((*str));

        return new_node;
    }
    else
    {
        return get_var(tree, str);
    }

    ERR_UNDEFINED;
    return NULL;
}

tree_node* get_expr(my_tree *tree, const char **str)
{
    tree_node* ans = get_turn(tree, str);

    SKIP_SPACES((*str));

    tree_node* new_node;

    if(**str == '+' || (**str == '-' && *(*str + 1) != '>'))
    {
        char op = **str;
        (*str)++;

        tree_node *r_expr = get_expr(tree, str);

        if(op == '+')
        {
            NEW_OP_NODE(ADD);

            LEFT(new_node) = ans;
            RIGHT(new_node) = r_expr;
            ans = new_node;
        }
        else
        {
            NEW_OP_NODE(SUB);

            LEFT(new_node) = ans;
            RIGHT(new_node) = r_expr;
            ans = new_node;
        }
    }

    SKIP_SPACES((*str));

    return ans;
}

tree_node* get_turn(my_tree* tree, const char **str)
{
    tree_node* ans = get_prim(tree, str);

    SKIP_SPACES((*str));

    tree_node* new_node;

    if(**str == '*' || **str == '/')
    {
        char op = **str;
        (*str)++;

        tree_node *r_expr = get_turn(tree, str);

        if(op == '*')
        {
            NEW_OP_NODE(MUL);

            LEFT(new_node) = ans;
            RIGHT(new_node) = r_expr;
            ans = new_node;
        }
        else
        {
            NEW_OP_NODE(DIV);

            LEFT(new_node) = ans;
            RIGHT(new_node) = r_expr;
            ans = new_node;
        }
    }

    SKIP_SPACES((*str));

    return ans;
}

tree_node* get_prim(my_tree *tree, const char **str)
{
    tree_node *ans;

    SKIP_SPACES((*str));

    if(**str == '(')
    {
        (*str)++;

        ans = get_expr(tree, str);

        if(**str != ')')
        {
            ERR_MISSING_CLOSE_BRACKET;
            return NULL;
        }
        (*str)++;
    }
    else ans = get_unary(tree, str);

    SKIP_SPACES((*str));

    return ans;
}

tree_node* get_unary(my_tree *tree, const char **str)
{
    tree_node* ans;

    SKIP_SPACES((*str));

    tree_node *new_node;

    if(OP_CMP(sin))
    {
        *str += strlen("sin");

        tree_node* arg = get_arg(tree, str);

        NEW_OP_NODE(SIN);
        ans = new_node;

        RIGHT(ans) = arg;
    }
    else if(OP_CMP(cos))
    {
        *str += strlen("cos");

        tree_node* arg = get_arg(tree, str);

        NEW_OP_NODE(COS);
        ans = new_node;

        RIGHT(ans) = arg;
    }
    else ans = get_num(tree, str);

    SKIP_SPACES((*str));

    return ans;
}

tree_node* get_arg(my_tree *tree, const char **str)
{
    SKIP_SPACES((*str));

    assert(**str == '(');
    (*str)++;

    tree_node* ans = get_expr(tree, str);

    assert(**str == ')');
    (*str)++;

    SKIP_SPACES((*str));

    return ans;
}

tree_node* get_var(my_tree *tree, const char **str)
{
    SKIP_SPACES((*str));

    tree_node *new_node;
    NEW_NODE;
    new_node->type = NODE_VAR;
    (new_node->var).name = *str;

    ssize_t len = 0;
    while(isalnum(**str))
    {
        (*str)++;
        len++;
    }
    (new_node->var).len = len;

    SKIP_SPACES((*str));

    return new_node;
}

tree_node* get_scope(my_tree *tree, const char **str)
{
    SKIP_SPACES((*str));

    assert(**str == '{');
    (*str)++;
    SKIP_SPACES((*str));

    tree_node *ans = get_op(tree, str);

    SKIP_SPACES((*str));
    assert(**str == '}');
    (*str)++;
    SKIP_SPACES((*str));

    return ans;
}

tree_node* get_condition(my_tree *tree, const char **str)
{
    SKIP_SPACES((*str));

    assert(**str == '(');
    (*str)++;

    tree_node *ans = get_expr(tree, str);

    SKIP_SPACES((*str));

    assert(**str == ')');
    (*str)++;

    return ans;
}

tree_node* get_op(my_tree *tree, const char **str)
{
    SKIP_SPACES((*str));

    tree_node *new_node;
    tree_node *ans = NEW_OP_NODE(GLUE);

    if(OP_CMP(var))
    {
        *str += strlen("var");

        NEW_OP_NODE(VAR_DECL);

        RIGHT(new_node) = get_var(tree, str);

        LEFT(ans) = new_node;
    }
    else if(OP_CMP(read))
    {
        *str += strlen("read");

        NEW_OP_NODE(READ);

        RIGHT(new_node) = get_var(tree, str);

        LEFT(ans) = new_node;
    }
    else if(OP_CMP(write))
    {
        *str += strlen("write");

        NEW_OP_NODE(WRITE);

        RIGHT(new_node) = get_var(tree, str);

        LEFT(ans) = new_node;
    }
    else if(OP_CMP(if))
    {
        *str += strlen("if");

        NEW_OP_NODE(IF);
        tree_node *if_node = new_node;

        LEFT(if_node) = get_condition(tree, str);
        //RIGHT(new_node) = get_scope(tree, str);

        tree_node *if_code = get_scope(tree, str);

        if(OP_CMP(else))
        {
            *str += strlen("else");

            NEW_OP_NODE(ELSE);
            RIGHT(if_node) = new_node;
            RIGHT(new_node) = if_code;
            LEFT(new_node) = get_scope(tree, str);
        }
        else RIGHT(if_node) = if_code;

        LEFT(ans) = if_node;
    }
    else if(OP_CMP(while))
    {
        *str += strlen("while");

        NEW_OP_NODE(WHILE);

        LEFT(new_node) = get_condition(tree, str);
        RIGHT(new_node) = get_scope(tree, str);

        LEFT(ans) = new_node;
    }
    else
    {
        new_node = get_math(tree, str);
        LEFT(ans) = new_node;
    }

    SKIP_SPACES((*str));

    if(**str != '}') RIGHT(ans) = get_op(tree, str);

    SKIP_SPACES((*str));

    return ans;
}

size_t get_file_size(FILE *stream)
{
    assert(stream != NULL);

    size_t ans = 0;

    assert(!fseek(stream, 0, SEEK_END));   //fseek returns zero upon success and non-zero otherwise
    ans = ftell(stream);
    assert(!ferror(stream));

    assert(!fseek(stream, 0, SEEK_SET));

    return ans;
}

char* read_text(FILE *stream)
{
    assert(stream != NULL);

    size_t len = get_file_size(stream);
    assert(len != 0);

    char* text = (char*)calloc(len + 1, sizeof(char)); //+1 for null terminator
    assert(text != NULL);

    fread(text, sizeof(char), len, stream);
    assert(!ferror(stream));

    return text;
}

#define DEF_OP(name, text, ...)                                                                 \
    case(OP_##name):                                                                            \
        GR_VIZ_PRINT("\t" "\"%p\"[label = \"{op: %s}\"];\n\n", &(array[cells_printed]), #text); \
        break;
void lexic_dump(const lexic_cell *array, size_t size)
{
    assert(array);

    FILE *gr_viz = fopen("lexic_dump.txt", "w");
    assert(gr_viz);

    GR_VIZ_PRINT("digraph dump\n{\n"
                 "\trankdir = LR;"
                 "\tnode[shape = \"record\", style = \"rounded\"];\n\n");

    for(size_t cells_printed = 0; cells_printed < size; cells_printed++)
    {
        if(cells_printed < size - 1)
        GR_VIZ_PRINT("\t" "\"%p\"->\"%p\";\n", &(array[cells_printed]), &(array[cells_printed + 1]));

        switch(array[cells_printed].type)
        {
            case CELL_BRACKET:
                switch(array[cells_printed].bracket)
                {
                    case OPEN_ROUND:
                        GR_VIZ_PRINT("\t" "\"%p\"[label = \"{%c}\"];\n\n", &(array[cells_printed]), '(');
                        break;
                    case CLOSE_ROUND:
                        GR_VIZ_PRINT("\t" "\"%p\"[label = \"{%c}\"];\n\n", &(array[cells_printed]), ')');
                        break;
                    case OPEN_CURLY:
                        GR_VIZ_PRINT("\t" "\"%p\"[label = \"{%s}\"];\n\n",
                            &(array[cells_printed]), "open curly");
                        break;
                    case CLOSE_CURLY:
                        GR_VIZ_PRINT("\t" "\"%p\"[label = \"{%s}\"];\n\n",
                            &(array[cells_printed]), "close curly");
                        break;
                }
                break;
            case CELL_NUM:
                GR_VIZ_PRINT("\t" "\"%p\"[label = \"{%.3lf}\"];\n\n",
                    &(array[cells_printed]), array[cells_printed].val);
                break;
            case CELL_NAME:
                GR_VIZ_PRINT("\t" "\"%p\"[label = \"{var: ", &(array[cells_printed]));
                fwrite(array[cells_printed].var_or_func.name, array[cells_printed].var_or_func.len,
                    1, gr_viz);
                GR_VIZ_PRINT("}\"];\n\n");
                break;
            case CELL_OP:
                switch(array[cells_printed].op)
                {
                    #include "../lang_operators.h"
                }

        }
    }

    putc('}', gr_viz);

    fclose(gr_viz);

    system("dot.exe -T png -o lexic_dump.png lexic_dump.txt");
    system("lexic_dump.png");

    return;
}
#undef DEF_OP

#undef SKIP_SPACES
#undef OP_CMP
#undef WRITE_OP
#undef GR_VIZ_PRINT
