#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // đầu đọc là p[0] và đầu ghi là p[1]
    //p_1 sẽ là pipe để tiến trình cha gửi dữ liệu cho tiến trình con
    //p_2 sẽ là pipe để tiến trình con gửi dữ liệu cho tiến trình cha
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
    // kiểm tra liệu đây là tiến trình con hay không
    if (pid == 0) {
        // Con không ghi vào p_1, nên đóng đầu ghi
        close(p_1[1]);
        // Con không đọc từ p_2, nên đóng đầu đọc
        close(p_2[0]);
        // kiểm tra xem liệu có đọc được 1 byte từ tiến trình cha hay không, nếu không thì trả về mã thoát bằng 1
        // ở đây nếu đọc được thì byte = 'i'
        if (read(p_1[0], &byte, 1) != 1) {
            fprintf(2, "child: read failed\n");
            exit(1);
        }
        // Con đã đọc từ p_1 xong, đóng đầu đọc.
        close(p_1[0]);
        
        output[10] = byte;
        printf("%d: %s\n", getpid(), output);

        byte = 'o';
        // ghi vào p_2 kí tự byte = 'o'
        if (write(p_2[1], &byte, 1) != 1) {
            fprintf(2, "child: write failed\n");
            exit(1);
        }
        // Cha đã ghi vào p_2 xong, đóng đầu ghi
        close(p_2[1]);
        exit(0);

    } 
    // nếu đây là tiến trình cha
    else {
        // Cha không ghi vào p_2, nên đóng đầu ghi
        close(p_2[1]);
        //Cha không đọc từ p_1, nên đóng đầu đọc
        close(p_1[0]);
        byte = 'i';
        // ghi vào p_1 kí tự byte = 'i'
        if (write(p_1[1], &byte, 1) != 1) {
            fprintf(2, "parent: write failed\n");
            exit(1);
        }
        // Cha đã ghi vào p_1 xong, đóng đầu ghi
        close(p_1[1]);
        // chờ tiến trình con kết thúc
        wait(0);
        // kiểm tra xem liệu có đọc được 1 byte từ tiến trình con hay không, nếu không thì trả về mã thoát bằng 1
        // ở đây nếu đọc được thì byte = 'o'
        if (read(p_2[0], &byte, 1) != 1) {
            fprintf(2, "parent: read failed\n");
            exit(1);
        }

        output[10] = byte;
        printf("%d: %s\n", getpid(), output);
        // Cha đã đọc từ p_2 xong, đóng đầu đọc.
        close(p_2[0]);
        exit(0);
    }
}