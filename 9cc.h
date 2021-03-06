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

typedef struct
{
    void **data;
    int capacity;
    int len;
} Vector;

typedef struct
{
    Vector *keys;
    Vector *vals;
} Map;

void tokenize(char *p);
void program();
void gen(Node *node);

Vector *new_vector();
void vec_push(Vector *vec, void *elem);

Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);

void runtest();

extern Token tokens[100];
extern Node *code[100];
extern int pos;