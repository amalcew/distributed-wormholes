#include <stdio.h>
#include <stdlib.h>

int main() {
    int rank = 1;
    srandom(rank);
    printf("%d\n", srandom(rank));
    return 0;
}