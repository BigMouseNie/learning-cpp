# Share Memory

## 一、相关函数（System V）

```c++
#include <sys/ipc.h>
#include <sys/shm.h>

// 创建共享内存
int shmget(key_t key, size_t size, int shmflg);	// shmget(ftok(".", 1), 1024, IPC_CREAT | 0666)
/*
* key	唯一标识共享内存的键值（可以用 ftok() 生成）
* size	要申请的共享内存大小（单位：字节）
* shmflg	权限标志 + 控制标志，常用 IPC_CREAT 等
* 成功：返回一个 共享内存标识符（shmid）
* 失败：返回 -1，并设置 errno
*/


// 建立映射
void *shmat(int shmid, const void *shmaddr, int shmflg);
/*
* shmid	共享内存标识符（由 shmget 返回）
* shmaddr	指定映射的内存地址，通常为 NULL，由系统自动分配
* shmflg	访问权限，一般为 0，或者 SHM_RDONLY 表示只读映射
* 成功：返回共享内存映射的地址（void* 指针）
* 失败：返回 (void*) -1，并设置 errno
*/


// 取消映射
int shmdt(const void *shmaddr);
/*
* shmaddr	映射共享内存时返回的地址（由 shmat 返回）
* 成功：返回 0
* 失败：返回 -1，并设置 errno
*/


// 控制共享内存（包括销毁）
int shmctl(int shmid, int cmd, struct shmid_ds *buf);
/*
* shmid	共享内存 ID（由 shmget 返回）
* cmd	控制命令（见下表）
* buf	共享内存信息结构体指针，根据 cmd 决定是否使用
*/

```



**shmflg**

| 标志名      | 含义                                      |
| ----------- | ----------------------------------------- |
| `IPC_CREAT` | 如果共享内存不存在则创建                  |
| `IPC_EXCL`  | 和 `IPC_CREAT` 一起使用，如果已存在则失败 |
| `0666`      | 权限（如读写权限：`rw-rw-rw-`）           |



**cmd**

| 值         | 说明                           |
| ---------- | ------------------------------ |
| `IPC_STAT` | 获取共享内存状态，填充到 `buf` |
| `IPC_SET`  | 设置共享内存参数（通过 `buf`） |
| `IPC_RMID` | 删除共享内存段（由内核回收）   |



## 二、简单例子

```c++
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define SHM_SIZE 1024

int main() {
    key_t key = ftok(".", 'a');
    if (key == -1) {
        printf("gen key err!\n");
        return -1;
    }

    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        printf("get shm err!\n");
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        printf("fork err!\n");
        return -1;
    } else if (pid == 0) {  // sub
        sleep(1);
        char* shmaddr = (char*)shmat(shmid, NULL, 0);
        if (shmaddr == (char *)-1) {
            printf("shmat err!\n");
            return -1;
        }
        printf("Child read from shared memory: %s\n", shmaddr);
        shmdt(shmaddr);
        return 0;
    } else {    // main
        char* shmaddr = (char*)shmat(shmid, NULL, 0);
        if (shmaddr == (char *)-1) {
            printf("shmat err!\n");
            return -1;
        }
        strcpy(shmaddr, "Hello from parent!");
        printf("Parent wrote to shared memory.\n");
        shmdt(shmaddr); // 解除映射

        int status;
        pid_t chid = waitpid(-1, &status, 0);	// 不用WNOHANG(Wait No Hang, 等待不挂起)，一般同信号sigaction SIGCHLD一起使用
        if(WIFEXITED(status)){
            printf("Child send %d\n", chid);
        }

        // 删除共享内存段
        shmctl(shmid, IPC_RMID, NULL);
        printf("Parent removed shared memory.\n");
    }
    return 0;
}

```

