#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

// nhận vào một chuỗi path(đường dẫn đầy đủ) 
// và trả về con trỏ đến phần tên file nằm ở cuối đường dẫn
// Giúp trích xuất tên tập tin từ đường dẫn để so sánh với tên đihcs
char* getFileName(char *path) {
  char *p;

  //Sau khi chạy xong vòng lặp for này, con trỏ sẽ tới dấu / ngay trước tên(nếu có)
  for (p = path+strlen(path); p >= path && *p != '/'; p--);
  // phải p++ lại để con trỏ trỏ tới ngay vị trí bắt đầu của tên
  p++;

  return p;
}

// đây là hàm chính thực hiện tìm kiếm đệ quy trong cây thư mục/    
void find(char *path, char *target) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  // mở tệp ở chế độ chỉ đọc (O_RDONLY)
  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  // lấy thông tin tệp
  // đưa thông tin tệp vào struct st
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
    case T_DEVICE:
    case T_FILE:
        // so sánh tên của tập tin(hoặc thiết bị) với target, nếu trùng tên, in ra đường dẫn
      if (strcmp(getFileName(path), target) == 0) printf("%s\n", path);
      break;
    // nếu là thư mục: duyệt qua từng entry trong thư mục và gọi đệ quy
    // hàm find() cho mỗi entry hợp lệ
    case T_DIR:
      // kiểm tra độ dài đường dẫn.
      if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        printf("find: path too long\n");
        break;
      }
      // copy vào char buf[512]
      strcpy(buf, path);
      //đặt con trỏ p vào cuối chuỗi đường dẫn
      p = buf + strlen(buf);
      // ghi kí tự '/' vào vị trí bộ nhớ mà p đang trỏ tới(để nối với tập tin/thư mục con)
      // sau đó, dịch con trỏ p sang vị trí tiếp theo trong bộ nhớ
      *p++ = '/';
      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        // bỏ qua entry không hợp lệ
        //nếu de.inum == 0, entry này không hợp lệ hoặc đã bị xóa
        if (de.inum == 0) continue;
        // bỏ qua thư mục đặc biệt(".": thư mục hiện tại)
        // và ("..": thư mục cha) để tránh đệ quy vô hạn
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
        // sao chép tên tập tin/ thư mục từ de.name vào vị trí p trong buf
        memmove(p, de.name, DIRSIZ);
        // đảm bảo chuỗi kết thúc đúng cách(bằng ký tự \0)
        p[DIRSIZ] = 0;
        // gọi đệ quy để tiếp tục tìm kiếm trong thư mcuj con
        // hoặc kiểm tra tập tin con
        find(buf, target);
      }
      break;
  }

  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("missing arguments\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  
  exit(0);
}
