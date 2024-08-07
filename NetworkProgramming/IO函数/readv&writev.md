# readv&writev()



## readv()

```C
#include <sys/uio.h>
ssize_t writev(int filedes, const struct iovec* iov, int iovcnt);

/*
* 成功时返回发送的字节数，失败时返回-1
* filedes : 表示数据传输对象的文件描述符
* iov     : iovec结构体的数组指针
* iovcnt  : 数组长度
*/

struct iovec{
    void* iov_base; // 缓冲地址
    size_t iov_len; // 缓冲大小
};


/*代码示例*/ 

#include <stdio.h>
#include <sys/uio.h>

int main(int argc,char* argv[]){
    struct iovec vec[2];
    char buf1[]="ABCD";
    char buf2[]="12345";
    int str_len;
    vec[0].iov_base=buf1;
    vec[0].iov_len=3;
    vec[1].iov_base=buf2;
    vec[1].iov_len=4;
    str_len=writev(1,vec,2);  // 1为标准输出，即向控制台写
    puts("");
    printf("str_len=%d\n",str_len);
    return 0;
}

```



## readv()

```C
#include <sys/uio.h>
ssize_t readv(int filedes, const struct iovec* iov, int iovcnt);

/*
* 成功时返回接收的字节数，失败时返回-1
* filedes : 表示数据接收对象的文件描述符
* iov     : iovec结构体的数组指针
* iovcnt  : 数组长度
*/

/*代码示例*/

#include <stdio.h>
#include <sys/uio.h>

int main(int argc,char* argv[]){
    struct iovec vec[2];
    char buf1[64]={0};
    char buf2[64]={0};
    int str_len;
    vec[0].iov_base=buf1;
    vec[0].iov_len=3;
    vec[1].iov_base=buf2;
    vec[1].iov_len=7;

    str_len=readv(0,vec,2);  // 0为标准输入，即从控制台读
    printf("str_len=%d\n",str_len);
    printf("%s\n%s\n",buf1,buf2);
    return 0;
}


```

