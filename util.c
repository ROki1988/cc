#include "9cc.h"

Vector *new_vector()
{
    Vector *vec = malloc(sizeof(Vector));
    vec->capacity = 16;
    vec->len = 0;
    vec->data = realloc(NULL, sizeof(void *) * vec->capacity);

    return vec;
}

void vec_push(Vector *vec, void *elem)
{
    if (vec->capacity == vec->len)
    {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
    }

    vec->data[vec->len++] = elem;
}

Map *new_map()
{
    Map *map = malloc(sizeof(Map));
    map->keys = new_vector();
    map->vals = new_vector();

    return map;
}

void map_put(Map *map, char *key, void *val)
{

    vec_push(map->keys, key);
    vec_push(map->vals, val);
}

void *map_get(Map *map, char *key)
{
    for (int i = map->keys->len - 1; i >= 0; i--)
    {
        if (strcmp(map->keys->data[i], key) == 0)
            return map->vals->data[i];
    }

    return NULL;
}