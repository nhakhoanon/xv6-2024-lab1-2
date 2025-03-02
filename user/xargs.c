#include <kernel/types.h>
#include <kernel/stat.h>
#include <kernel/param.h>
#include <user/user.h>

void readLine(char* result) {
    char buffer;
    int result_size = 0;
    while (read(0, &buffer, 1)) {
        if (buffer == '\n' || buffer == '\0') break;
        result[result_size++] = buffer;
    }
    result[result_size] = '\0';
}


int main(int argc, char* argv[]) {
    char* xargv[MAXARG]; // Parameters for the command specified in xargs.
    int xargc = 0; // Number of parameters of xargs' base command.
    
    if (argc == 1) {
        argv[1] = "echo"; // default to echo.
        argc++;
    }

    for (int i = 1; i < argc; i++) {
        xargv[i-1] = argv[i];
    }
    
    char buffer[512], *p;
    while (1) {
        readLine(buffer);

        if (strlen(buffer) == 0) break;
        
        xargc = argc - 1;
        p = buffer;
        while (*p != '\0') {
            if (*p == ' ') {
                while (*p == ' ') *p++ = '\0';
            } else if (p == buffer || *(p - 1) == '\0') {
                if (xargc >= MAXARG - 1) {
                    printf("xargs: too many arguments\n");
                    exit(1);
                }

                xargv[xargc++] = p;
                ++p;
            } else ++p;
        }
        xargv[xargc] = 0;

        int pid = fork();
        if (pid < 0) {
            printf("xargs: fork error\n");
            exit(1);
        } else if (pid == 0) {
            exec(xargv[0], xargv);

            printf("xargs: exec error\n");
            exit(1);
        } else {
            wait(0);
        }
    }

    exit(0);
}