#include "9cc.h"

Token tokens[100];
Node *code[100];
int pos = 0;

Node *expr();
Node *mul();
Node *term();
Node *assign();
Node *assign_();

void tokenize(char *p)
{
    int i = 0;

    while (*p)
    {
        if (isspace(*p))
        {
            p++;
        }
        else if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '=' || *p == ';')
        {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
        }
        else if ('a' <= *p && *p <= 'z')
        {
            tokens[i].ty = TK_IDENT;
            tokens[i].input = *p;
            i++;
            p++;
        }
        else if (isdigit(*p))
        {
            tokens[i].ty = TK_NUM;
            tokens[i].input = *p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
        }
        else
        {
            fprintf(stderr, "トークナイズできません。: %s\n", p);
            exit(1);
        }
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}

Node *new_node(int op, Node *lhs, Node *rhs)
{
    Node *node = malloc(sizeof(Node));
    node->op = op;
    node->lhs = lhs;
    node->rhs = rhs;

    return node;
}

Node *new_node_num(int val)
{
    Node *node = malloc(sizeof(Node));
    node->op = ND_NUM;
    node->val = val;

    return node;
}

Node *new_node_ident(char name)
{
    Node *node = malloc(sizeof(Node));
    node->op = ND_IDENT;
    node->name = name;

    return node;
}

void error(char *format, int i)
{
    fprintf(stderr, format, tokens[i].input);
    exit(1);
}

Node *term()
{
    if (tokens[pos].ty == TK_NUM)
    {
        return new_node_num(tokens[pos++].val);
    }
    else if (tokens[pos].ty == TK_IDENT)
    {
        return new_node_ident(tokens[pos++].input);
    }
    else if (tokens[pos].ty == '(')
    {
        pos++;
        Node *node = expr();
        if (tokens[pos].ty != ')')
            error("開きカッコに対応する閉じカッコがありません: '%s'\n", pos);
        pos++;
        return node;
    }
    else
    {
        error("数値でも開きカッコでもないトークンです: '%s'\n", pos);
    }
}

Node *mul()
{
    Node *lhs = term();

    if (tokens[pos].ty == TK_EOF)
    {
        return lhs;
    }
    else if (tokens[pos].ty == '*')
    {
        pos++;
        return new_node('*', lhs, mul());
    }
    else if (tokens[pos].ty == '/')
    {
        pos++;
        return new_node('/', lhs, mul());
    }
    else
    {
        return lhs;
    }
}

Node *expr()
{
    Node *lhs = mul();

    if (tokens[pos].ty == TK_EOF)
    {
        return lhs;
    }
    else if (tokens[pos].ty == '+')
    {
        pos++;
        return new_node('+', lhs, expr());
    }
    else if (tokens[pos].ty == '-')
    {
        pos++;
        return new_node('-', lhs, expr());
    }
    else
    {
        return lhs;
    }
}

Node *assign()
{
    Node *lhs = expr();

    if (tokens[pos].ty == TK_EOF) {
        return lhs;
    }
    else {
        Node *r = NULL;
        if (tokens[pos].ty == '=') {
            pos++;
            r =  new_node('=', lhs, assign());
        }
        else {
            r = lhs;
        }

        if (tokens[pos].ty == ';') pos++;

        return r;
    }
}

void program()
{
    int i = 0;
    while (tokens[pos].ty != TK_EOF)
    {
        code[i] = assign();
        i++;
    }
    code[i] = NULL;
    return;
}