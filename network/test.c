#include <stdio.h>
#include <stdlib.h>

typedef struct dd
{
    int i;
    struct dd *next;
}d;

int main()
{
    d *t;
    t = NULL;
    if (t->next == NULL)
        printf("nnn\n");
    return 0;
}