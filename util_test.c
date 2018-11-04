#include "9cc.h"

int expect(int line, int expected, int actual)
{
    if (expected == actual)
        return;
    fprintf(stderr, "%d: %d expected, but got %d\n",
            line, expected, actual);
    exit(1);
}

void test_vector()
{
    printf("test_vector\n");
    Vector *vec = new_vector();
    expect(__LINE__, 0, vec->len);

    for (int i = 0; i < 100; i++)
        vec_push(vec, (void *)i);

    expect(__LINE__, 100, vec->len);
    expect(__LINE__, 0, (int)vec->data[0]);
    expect(__LINE__, 50, (int)vec->data[50]);
    expect(__LINE__, 99, (int)vec->data[99]);
}

void test_map()
{
    printf("test_map\n");
    Map *map = new_map();
    expect(__LINE__, 0, (int)map_get(map, "foo"));
    printf("test_map: pass 0\n");

    map_put(map, "foo", (void *)2);
    expect(__LINE__, 2, (int)map_get(map, "foo"));
    printf("test_map: pass 2\n");

    map_put(map, "bar", (void *)4);
    expect(__LINE__, 4, (int)map_get(map, "bar"));
    printf("test_map: pass 4\n");

    map_put(map, "foo", (void *)6);
    expect(__LINE__, 6, (int)map_get(map, "foo"));
    printf("test_map: pass 6\n");
}

void runtest()
{
    test_vector();
    test_map();
    printf("OK\n");
}