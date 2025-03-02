#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(2, "The number of arguments is invalid!\n");
        exit(1);
    }
    
    printf("(nothing happens for a little while)\n");
    sleep(atoi(argv[1]));

    exit(0);
}