#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"

int
main(int argc, char *argv[]) {
    int x1 = getreadcount();
    printf("XV6_TEST_OUTPUT %d %d %d\n", x1, 0, 0);
    // int x2 = getreadcount();
    // char buf[100];
    // (void) read(4, buf, 1);
    // int x3 = getreadcount();
    // int i;
    // for (i = 0; i < 1000; i++) {
    //     (void) read(4, buf, 1);
    // }
    // int x4 = getreadcount();
    exit(0);
}
