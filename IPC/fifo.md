# fifo

# 一、fifo()

```c++
#include <sys/stat.h>

int mkfifo(const char *pathname, mode_t mode);

/*
* pathname: fifo文件的创建地址
* mode: 文件访问权限
* 成功返回0，失败返回-1
*/
```



## 二、简单例子

**read**

```c++
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    char* fifoPath = "./myfifo";
    int fd = open(fifoPath, O_RDONLY);	// 如果没有写fd会阻塞
    char buf[128];
    printf("=== begin read ===\n");
    sleep(1);
    read(fd, buf, 128);	// 可一次读完 hello!!hello\0
    printf("Received: %s\n", buf);
    close(fd);
    return 0;
}

```



**write**

```c++
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main() {
    char* fifoPath = "./myfifo";
    mkfifo(fifoPath, 0666);
    // int fd = open(fifoPath, O_WRONLY); // 如果没有读fd会阻塞
    int fd = open(fifoPath, O_WRONLY | O_NONBLOCK);	// 无阻塞打开，如果没有读fd会返回-1
    if (fd == -1) {
        printf("open %s failed!\n", fifoPath);
        return -1;
    }
    char* msg = "hello!!";
    printf("=== begin write ===\n");
    write(fd, msg, strlen(msg));
    write(fd, msg, strlen(msg) + 1);
    close(fd);
    return 0;
}
```

