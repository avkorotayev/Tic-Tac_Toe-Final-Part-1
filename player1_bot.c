#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));
    int move = 1 + rand() % 9;
    printf("%d\n", move); // output move to stdout
    return 0;
}
