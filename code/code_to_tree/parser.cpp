#include "code_to_tree.h"

#define SKIP_SPACES(text) while(isspace(*text)) text++

#define STORE_BRACKET(br_type)                 \
    ans[tokens_found].type = CELL_BRACKET;     \
    ans[tokens_found].bracket = br_type;       \
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

#define CHECK_OP(op_num) ((**lexic).type == CELL_OP && (**lexic).op == op_num)

#define CHECK_BRACKET(bracket_type) ((**lexic).type == CELL_BRACKET && (**lexic).bracket == bracket_type)

#define RECURS_ASSERT      \
    assert(tree);          \
    assert(lexic);         \
    assert(*lexic);        \

lexic_cell* lexic_analysis(const char *filename, char **text)
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
        if(tokens_found == current_size)
        {
            current_size *= ARRAY_SIZE_MULTIPLIER;
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
        else
        {
            int bracket_type = 0;
            if((bracket_type = detect_bracket(str)))
            {
                STORE_BRACKET(bracket_type);
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
        }

        SKIP_SPACES(str);
        tokens_found++;
    }

    //lexic_dump(ans, tokens_found);

    return ans;
}
#undef DEF_OP

int detect_bracket(const char *str)
{
    assert(str);

    switch(*str)
    {
        case '(':
            return OPEN_ROUND;
        case ')':
            return CLOSE_ROUND;
        case '{':
            return OPEN_CURLY;
        case '}':
            return CLOSE_CURLY;
        case '[':
            return OPEN_SQUARE;
        case ']':
            return CLOSE_SQUARE;
        default:
            return 0;
    }

    return 0;
}

void parse_src_code(my_tree *tree, lexic_cell *lexic)
{
    assert(tree);
    assert(lexic);

    tree_ctor(tree, DEFAULT_TREE_CAP);

    tree->root = get_func(tree, &lexic);

    return;
}

tree_node* get_func(my_tree *tree, lexic_cell **lexic)
{
    RECURS_ASSERT;

    if((**lexic).type != CELL_NAME) return NULL;

    tree_node *new_node;

    tree_node *ans = NEW_OP_NODE(GLUE);

    NEW_NODE;
    new_node->type = NODE_VAR;
    (new_node->var).name = (**lexic).var_or_func.name;
    (new_node->var).len = (**lexic).var_or_func.len;

    (*lexic)++;

    LEFT(ans) = new_node;

    assert(CHECK_BRACKET(OPEN_SQUARE));
    (*lexic)++;

    LEFT(LEFT(ans)) = get_func_args(tree, lexic);

    assert(CHECK_BRACKET(CLOSE_SQUARE));
    (*lexic)++;

    RIGHT(LEFT(ans)) = get_scope(tree, lexic);

    RIGHT(ans) = get_func(tree, lexic);

    return ans;
}

tree_node* get_func_args(my_tree *tree, lexic_cell **lexic)
{
    RECURS_ASSERT;

    if(CHECK_BRACKET(CLOSE_SQUARE)) return NULL;

    tree_node *new_node;

    new_node = get_math(tree, lexic);

    assert(CHECK_OP(OP_ARG_SEP));
    (*lexic)++;

    RIGHT(new_node) = get_func_args(tree, lexic);

    return new_node;
}

tree_node* get_math(my_tree *tree, lexic_cell **lexic)
{
    RECURS_ASSERT;

    tree_node *expr = get_expr(tree, lexic);

    if(CHECK_OP(OP_ASSIGN))
    {
        (*lexic)++;

        tree_node *new_node;
        NEW_OP_NODE(ASSIGN);

        tree_node *ans = new_node;

        RIGHT(ans) = get_var(tree, lexic);
        LEFT(ans) = expr;

        return ans;
    }
    else
    {
        return expr;
    }
}

tree_node* get_num(my_tree *tree, lexic_cell **lexic)
{
    RECURS_ASSERT;

    if((**lexic).type == CELL_NUM)
    {
        tree_node *new_node;
        NEW_NUM_NODE((**lexic).val);

        (*lexic)++;

        return new_node;
    }
    else
    {
        return get_var(tree, lexic);
    }

    ERR_UNDEFINED;
    return NULL;
}

tree_node* get_expr(my_tree *tree, lexic_cell **lexic)
{
    RECURS_ASSERT;

    tree_node* ans = get_turn(tree, lexic);

    tree_node* new_node;

    if(CHECK_OP(OP_ADD) || CHECK_OP(OP_SUB))
    {
        char op = (**lexic).op;
        (*lexic)++;

        tree_node *r_expr = get_expr(tree, lexic);

        if(op == OP_ADD)
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

    return ans;
}

tree_node* get_turn(my_tree* tree, lexic_cell **lexic)
{
    RECURS_ASSERT;

    tree_node* ans = get_prim(tree, lexic);

    tree_node* new_node;

    if(CHECK_OP(OP_MUL) || CHECK_OP(OP_DIV))
    {
        char op = (**lexic).op;
        (*lexic)++;

        tree_node *r_expr = get_turn(tree, lexic);

        if(op == OP_MUL)
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

    return ans;
}

tree_node* get_prim(my_tree *tree, lexic_cell **lexic)
{
    RECURS_ASSERT;

    tree_node *ans;

    if(CHECK_BRACKET(OPEN_ROUND))
    {
        (*lexic)++;

        ans = get_expr(tree, lexic);

        if(!CHECK_BRACKET(CLOSE_ROUND))
        {
            ERR_MISSING_CLOSE_BRACKET;
            return NULL;
        }
        (*lexic)++;
    }
    else ans = get_unary(tree, lexic);

    return ans;
}

tree_node* get_unary(my_tree *tree, lexic_cell **lexic)
{
    RECURS_ASSERT;

    tree_node* ans;

    tree_node *new_node;

    if(CHECK_OP(OP_SIN))
    {
        (*lexic)++;

        tree_node* arg = get_arg(tree, lexic);

        NEW_OP_NODE(SIN);
        ans = new_node;

        RIGHT(ans) = arg;
    }
    else if(CHECK_OP(OP_COS))
    {
        (*lexic)++;

        tree_node* arg = get_arg(tree, lexic);

        NEW_OP_NODE(COS);
        ans = new_node;

        RIGHT(ans) = arg;
    }
    else if(CHECK_OP(OP_ABS))
    {
        (*lexic)++;

        tree_node* arg = get_arg(tree, lexic);

        NEW_OP_NODE(ABS);
        ans = new_node;

        RIGHT(ans) = arg;
    }
    else if(CHECK_OP(OP_SQRT))
    {
        (*lexic)++;

        tree_node* arg = get_arg(tree, lexic);

        NEW_OP_NODE(SQRT);
        ans = new_node;

        RIGHT(ans) = arg;
    }
    else ans = get_num(tree, lexic);

    return ans;
}

tree_node* get_arg(my_tree *tree, lexic_cell **lexic)
{
    RECURS_ASSERT;

    assert(CHECK_BRACKET(OPEN_ROUND));
    (*lexic)++;

    tree_node* ans = get_expr(tree, lexic);

    assert(CHECK_BRACKET(CLOSE_ROUND));
    (*lexic)++;

    return ans;
}

tree_node* get_var(my_tree *tree, lexic_cell **lexic)
{
    RECURS_ASSERT;

    tree_node *new_node;
    NEW_NODE;
    new_node->type = NODE_VAR;
    (new_node->var).name = (**lexic).var_or_func.name;
    (new_node->var).len = (**lexic).var_or_func.len;

    (*lexic)++;

    if(CHECK_BRACKET(OPEN_SQUARE))
    {
        (*lexic)++;

        tree_node *func_name = new_node;

        NEW_OP_NODE(CALL);

        LEFT(new_node) = func_name;

        RIGHT(new_node) = get_func_args(tree, lexic);

        assert(CHECK_BRACKET(CLOSE_SQUARE));
        (*lexic)++;
    }

    return new_node;
}

tree_node* get_scope(my_tree *tree, lexic_cell **lexic)
{
    RECURS_ASSERT;

    assert(CHECK_BRACKET(OPEN_CURLY));
    (*lexic)++;

    tree_node *ans = get_op(tree, lexic);

    assert(CHECK_BRACKET(CLOSE_CURLY));
    (*lexic)++;

    return ans;
}

tree_node* get_condition(my_tree *tree, lexic_cell **lexic)
{
    RECURS_ASSERT;

    assert(CHECK_BRACKET(OPEN_ROUND));
    (*lexic)++;

    tree_node *ans = get_expr(tree, lexic);

    assert(CHECK_BRACKET(CLOSE_ROUND));
    (*lexic)++;

    return ans;
}

tree_node* get_op(my_tree *tree, lexic_cell **lexic)
{
    RECURS_ASSERT;

    tree_node *new_node;
    tree_node *ans = NEW_OP_NODE(GLUE);

    if(CHECK_OP(OP_VAR_DECL))
    {
        (*lexic)++;

        NEW_OP_NODE(VAR_DECL);

        RIGHT(new_node) = get_var(tree, lexic);

        LEFT(ans) = new_node;
    }
    else if(CHECK_OP(OP_READ))
    {
        (*lexic)++;

        NEW_OP_NODE(READ);

        RIGHT(new_node) = get_var(tree, lexic);

        LEFT(ans) = new_node;
    }
    else if(CHECK_OP(OP_WRITE))
    {
        (*lexic)++;

        NEW_OP_NODE(WRITE);

        RIGHT(new_node) = get_var(tree, lexic);

        LEFT(ans) = new_node;
    }
    else if(CHECK_OP(OP_RETURN))
    {
        (*lexic)++;

        NEW_OP_NODE(RETURN);

        RIGHT(new_node) = get_math(tree, lexic);

        LEFT(ans) = new_node;
    }
    else if(CHECK_OP(OP_IF))
    {
        (*lexic)++;

        NEW_OP_NODE(IF);
        tree_node *if_node = new_node;

        LEFT(if_node) = get_condition(tree, lexic);

        tree_node *if_code = get_scope(tree, lexic);

        if(CHECK_OP(OP_ELSE))
        {
            (*lexic)++;

            NEW_OP_NODE(ELSE);
            RIGHT(if_node) = new_node;
            RIGHT(new_node) = if_code;
            LEFT(new_node) = get_scope(tree, lexic);
        }
        else RIGHT(if_node) = if_code;

        LEFT(ans) = if_node;
    }
    else if(CHECK_OP(OP_WHILE))
    {
        (*lexic)++;

        NEW_OP_NODE(WHILE);

        LEFT(new_node) = get_condition(tree, lexic);
        RIGHT(new_node) = get_scope(tree, lexic);

        LEFT(ans) = new_node;
    }
    else
    {
        new_node = get_math(tree, lexic);
        LEFT(ans) = new_node;
    }

    if((**lexic).type != CELL_BRACKET || (**lexic).bracket != CLOSE_CURLY)
        RIGHT(ans) = get_op(tree, lexic);

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
