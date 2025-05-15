# Semaphore

## 一、相关函数

```c++
#include <sys/sem.h>

// 信号量创建
int semget(key_t key, int nsems, int semflg);
/*
* key	用于标识信号量集合的键值（可通过 ftok 生成）
* nsems	信号量的数量（通常为 1，表示一个信号量；也可以是多个）
* semflg	权限标志，比如 `IPC_CREAT
* 成功：返回信号量集合的 标识符（semid）
* 失败：返回 -1，可用 perror 查看错误原因
*/
int semid = semget(key, 3, IPC_CREAT | 0666);	// 就是创建一个包含 3个信号量的集合，这个集合的 ID 是 semid
semctl(semid, 0, SETVAL, arg); // 设置第 0 个信号量
semctl(semid, 1, SETVAL, arg); // 设置第 1 个信号量


// 控制管理信号量
int semctl(int semid, int semnum, int cmd, ...);
/*
* semid	semget 返回的信号量集合 ID
* semnum	信号量集合中第几个信号量（通常为 0）
* cmd	控制命令（见下）
* ...	可选参数（通常是 union semun 类型，用于设置值等）
*/
union semun {
    int val;                // 用于 SETVAL
    struct semid_ds *buf;   // IPC_STAT, IPC_SET
    unsigned short *array;  // GETALL, SETALL
};


// PV操作
int semop(int semid, struct sembuf *sops, size_t nsops);
/*
* semid	信号量集合的 ID（来自 semget）
* sops	操作数组指针（描述对哪些信号量执行哪些操作）
* nsops	要操作的个数（数组中结构体数量）
*/
struct sembuf {
    unsigned short sem_num;  // 信号量编号
    short sem_op;            // 操作值（-1 表示P操作，+1表示V操作）
    short sem_flg;           // 0 或 IPC_NOWAIT、SEM_UNDO 等
};

```



## 二、简单例子

```c++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define SHM_SIZE 1024

// 定义 semun 联合体，用于 semctl
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

/*
struct sembuf {
    unsigned short sem_num;  // 信号量编号
    short sem_op;            // 操作值（-1 表示P操作，+1表示V操作）
    short sem_flg;           // 0 或 IPC_NOWAIT、SEM_UNDO 等
};
 */
// P操作（获取资源）
void sem_p(int semid) {
    struct sembuf sb = {0, -1, 0};
    semop(semid, &sb, 1);
}

// V操作（释放资源）
void sem_v(int semid) {
    struct sembuf sb = {0, 1, 0};
    semop(semid, &sb, 1);
}

int main () {
    key_t shmkey = ftok(".", 'm');
    key_t semkey = ftok(".", 's');

    if (shmkey == -1 || semkey == -1) {
        perror("ftok");
        exit(1);
    }

    // 创建共享内存
    int shmid = shmget(shmkey, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    // 创建一个信号量集合，包含 1 个信号量
    int semid = semget(semkey, 1, IPC_CREAT | 0666);
        if (semid == -1) {
        perror("semget");
        exit(1);
    }

    union semun arg;
    arg.val = 0;
    if (semctl(semid, 0, SETVAL, arg)) {
        perror("semctl");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) { // sub read
        // sleep(1);
        char* shmaddr = shmat(shmid, NULL, 0);
        if (shmaddr == (char*)-1) {
            perror("shmat (child)");
            exit(1);
        }

        sem_p(semid);
        printf("Child read: %s\n", shmaddr);
        // sem_v(semid);

        shmdt(shmaddr);
        exit(0);
    } else { // main write
        sleep(2);
        char* shmaddr = (char*)shmat(shmid, NULL, 0);
        if (shmaddr == (char*)-1) {
            perror("shmat (parent)");
            exit(1);
        }

        // sem_p(semid);
        strcpy(shmaddr, "HELLO!!");
        printf("Parent wrote to shared memory.\n");
        sem_v(semid); // 释放资源
        shmdt(shmaddr);

        int status;
        pid_t chid = waitpid(-1, &status, 0);
        if(WIFEXITED(status)){
            printf("Child send %d\n", chid);
        }

        // 删除共享内存和信号量
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);
        printf("Resources cleaned up.\n");
    }

    return 0;
}

```

