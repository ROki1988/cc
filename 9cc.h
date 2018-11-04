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

void tokenize(char *p);
void program();
void gen(Node *node);

extern Token tokens[100];
extern Node *code[100];
extern int pos;