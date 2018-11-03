#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
    TK_NUM = 256,
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
};

typedef struct Node
{
    int op;
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

Token tokens[100];
int pos = 0;

Node *expr();
Node *mul();
Node *term();

void tokenize(char *p)
{
    int i = 0;

    while (*p)
    {
        if (isspace(*p))
        {
            p++;
        }
        else if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' )
        {
            tokens[i].ty = *p;
            tokens[i].input = p;
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

void error(char *format, int i)
{
    fprintf(stderr, format, tokens[i].input);
    exit(1);
}

Node *number() {
     if (tokens[pos].ty == TK_NUM)
    {
        return new_node_num(tokens[pos++].val);
    }
    else {
        error("数値でないトークンです: %s\n", pos);
    }   
}

Node *term() {

    if (tokens[pos].ty == TK_NUM)
    {
        return new_node_num(tokens[pos++].val);
    }
    else if (tokens[pos].ty == '(')
    {
        pos++;
        Node *node = expr();
        if (tokens[pos].ty != ')')
            error("開きカッコに対応する閉じカッコがありません: %s\n", pos);
        pos++;
        return node;
    }
    else
    {
        error("数値でも開きカッコでもないトークンです: %s\n", pos);
    }
}

Node *mul() {
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

Node *expr() {
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

void gen(Node *node) {
    if (node->op == ND_NUM) {
        printf("  push %d\n", node->val);
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

    Node* node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    gen(node);

    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}
