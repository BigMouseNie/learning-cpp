# pipe

## 一、pipe()

```c++
#include <unistd.h>
int pipe (int pipefd[2]);

/*
* 一个长度为 2 的整数数组，用于返回两个文件描述符：
* pipefd[0]：读端（read end）
* pipefd[1]：写端（write end）
* 成功时返回0；失败时返回-1，并设置 errno
*/
```



## 二、简单例子

管道是**字节流**，管道的 `read()` 会**尽可能多地读取可用字节**（最多 `n` 字节），而**不会按你“写入次数”进行分割**

```c++
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char* argv[]) {
    int pipefd[2];
    pipe(pipefd);

    pid_t pid = fork();
    if (pid == 0) { // sub
        close(pipefd[0]);
        char* msg = "hello world!";
        write(pipefd[1], msg, strlen(msg)+1);
        //sleep(1);
        write(pipefd[1], msg, strlen(msg)+1);
        close(pipefd[1]);
    } else { // main
        close(pipefd[1]);
        char buf[128];
        read(pipefd[0], buf, 128);
        printf("Parent received: %ld : %s\n", strlen(buf), buf);
        memset(buf, 0, 128);
        read(pipefd[0], buf, 128);
        printf("Parent received: %ld : %s\n", strlen(buf), buf);
        close(pipefd[0]);

        pid_t id;
        int status;
        id=waitpid(-1, &status, WNOHANG);
        if(WIFEXITED(status)){
            printf("Child send %d\n",id);
        }
    }

    return 0;
}

```



