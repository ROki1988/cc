#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
    TK_NUM = 256,
    TK_IDENT,
    TK_EOF,
};

typedef struct
{
    int ty;
    int val;
    char *input;
} Token;

enum
{
    ND_NUM = 256,
    ND_IDENT,
};

typedef struct Node
{
    int op;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    char name;
} Node;

Token tokens[100];
Node *code[100];
int pos = 0;

Node *expr();
Node *mul();
Node *term();
Node *assign();
Node *assign_();
void program();

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

void gen_lval(Node *node) {
    if (node->op == ND_IDENT) {
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", ('z' - node->name + 1) * 8);
        printf("  push rax\n");
        return;
    }
    else {
        exit(2);
    }
    
}

void gen(Node *node)
{
    if (node->op == ND_NUM)
    {
        printf("  push %d\n", node->val);
        return;
    }

    
    if (node->op == ND_IDENT) {
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    }

     
     if (node->op == '=') {
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
     }
     

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->op)
    {
    case '+':
        printf("  add rax, rdi\n");
        break;
    case '-':
        printf("  sub rax, rdi\n");
        break;
    case '*':
        printf("  mul rdi\n");
        break;
    case '/':
        printf("  mov rdx, 0\n");
        printf("  div rdi\n");
        break;
    }

    printf("  push rax\n");
}

int main(int argc, char const *argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    tokenize(argv[1]);

    program();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");
    
    for(size_t i = 0; code[i]; i++)
    {
        gen(code[i]);
        printf("  pop rax\n");
    }

    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");   
    printf("  ret\n");
    return 0;
}
