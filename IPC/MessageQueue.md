# MessageQueue

## 一、相关函数

```c++
#include <sys/msg>

// 创建消息队列
int msgget(key_t key, int msgflg);
/*
* key	消息队列的键（通常用 ftok() 生成）
* msgflg	权限标志（如 `IPC_CREAT）
* 成功：返回 消息队列的 ID（int 类型）
* 失败：返回 -1
*/


// 发送消息
int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
/*
* msqid	消息队列 ID（由 msgget() 返回）
* msgp	指向要发送的消息结构体指针，必须以 long mtype 开头
* msgsz	消息正文的大小（不包括 mtype）
* msgflg	发送选项（如 0 表示阻塞发送；IPC_NOWAIT 非阻塞）
* 成功：返回 0
* 失败：返回 -1，并设置 errno
*/


// 接收消息
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
/*
* msqid	消息队列 ID
* msgp	接收消息结构体的指针，必须以 long mtype 开头
* msgsz	能接收的最大正文大小
* msgtyp	指定消息类型：
* 	- 为 0：取第一个可用消息
* 	- 大于 0：取类型等于 msgtyp 的消息
* 	- 小于 0：取类型 ≤ abs(msgtyp) 中最小类型
* msgflg	行为标志（如 IPC_NOWAIT 非阻塞）
* 成功：返回接收到的正文大小（不含 mtype）
* 失败：返回 -1，并设置 errno
*/


// 控制消息队列
int msgctl(int msqid, int cmd, struct msqid_ds *buf);
/*
* msqid	消息队列 ID
* cmd	控制命令：
* 	- IPC_STAT: 获取状态
* 	- IPC_SET: 设置属性
* 	- IPC_RMID: 删除队列
* buf	用于存取消息队列的信息结构体（如 struct msqid_ds）
*/

```



## 二、简单例子

```c++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_TEXT 128

struct msgbuf {
    long mtype;
    char mtext[MAX_TEXT];
};

int main() {
    key_t key = ftok(".", 'm');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    // 创建消息队列
    int msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // 子进程：接收消息
        struct msgbuf msg;
        msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0); // 接收 type 为 1 的消息
        printf("Child received: %s\n", msg.mtext);
        exit(0);
    } else {
        // 父进程：发送消息
        struct msgbuf msg;
        msg.mtype = 1; // 自定义消息类型
        strcpy(msg.mtext, "Hello from parent");

        msgsnd(msgid, &msg, strlen(msg.mtext)+1, 0);
        printf("Parent sent message.\n");

        wait(NULL); // 等待子进程退出

        // 删除消息队列
        msgctl(msgid, IPC_RMID, NULL);
        printf("Message queue removed.\n");
    }

    return 0;
}

```

