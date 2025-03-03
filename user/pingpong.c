#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int p_1[2];
    int p_2[2];
    char byte, output[14] = "received p.ng";

    if (pipe(p_1) < 0 || pipe(p_2) < 0) {
        fprintf(2, "pipe creation failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        close(p_1[1]);
        close(p_2[0]);
        if (read(p_1[0], &byte, 1) != 1) {
            fprintf(2, "child: read failed\n");
            exit(1);
        }
        close(p_1[0]);
        
        output[10] = byte;
        printf("%d: %s\n", getpid(), output);

        byte = 'o';
        if (write(p_2[1], &byte, 1) != 1) {
            fprintf(2, "child: write failed\n");
            exit(1);
        }
        close(p_2[1]);
        exit(0);

    } else {
        close(p_2[1]);
        close(p_1[0]);
        byte = 'i';
        if (write(p_1[1], &byte, 1) != 1) {
            fprintf(2, "parent: write failed\n");
            exit(1);
        }
        close(p_1[1]);

        wait(0);
        if (read(p_2[0], &byte, 1) != 1) {
            fprintf(2, "parent: read failed\n");
            exit(1);
        }

        output[10] = byte;
        printf("%d: %s\n", getpid(), output);
        close(p_2[0]);
        exit(0);
    }
}