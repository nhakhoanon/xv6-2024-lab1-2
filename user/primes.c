#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int) __attribute__((noreturn));

void primes(int readfd) {
    int p, n;
    // Read first number - this will be a prime
    if (read(readfd, &p, sizeof(int)) != sizeof(int)) {
        close(readfd);  
        exit(0);
    }
    printf("prime %d\n", p);

    // Create pipe for next process
    int fds[2];
    pipe(fds);

    int pid = fork();

    if (pid == 0) {
        // Child process
        close(readfd);  // Don't need previous pipe's read end
        close(fds[1]);  // Don't need write end of new pipe
        primes(fds[0]); // Continue with next prime
    } else if (pid > 0) { // Parent process
        close(fds[0]);  // Don't need read end of new pipe

        // Read all numbers from previous process
        while (read(readfd, &n, sizeof(int)) > 0) {
            // If n is not divisible by p, pass it to next process
            if (n % p != 0) {
                write(fds[1], &n, sizeof(int));
            }
        }

        close(readfd); // Done reading from previous pipe
        close(fds[1]); // Done writing to next pipe
        wait(0);       // Wait for child to finish
        exit(0);
    } else {
        printf("Fork error\n");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    int fds[2];
    pipe(fds);
    int pid = fork();
    if (pid == 0) {
        // Child process
        close(fds[1]);  // Don't need write end
        primes(fds[0]); // Start processing primes
    } else if (pid > 0) { // Parent process
        close(fds[0]);  // Don't need read end
        // Feed numbers 2 through 280 into the pipeline
        int to = 0;
        if (argc == 1) 
            to = 280;
        else if (argc == 2) {
            to = atoi(argv[1]);
        }
        else {
            printf("number of arguments must be 1 or 2!");
            exit(1);
        }
        for (int i = 2; i <= to; i++) {
            write(fds[1], &i, sizeof(int));
        }   
        close(fds[1]); // Done writing
        wait(0);       // Wait for all processes to finish
    } else {
        printf("Fork error\n");
        exit(1);
    }

    exit(0);
}