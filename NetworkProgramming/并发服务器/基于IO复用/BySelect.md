# By Select



## 一、select()



### select()

```C
#include <sys/select.h>
#include <sys/time.h>

int select(int maxfd, fd_set* readset, fd_set* writeset, fd_set* exceptset,
                                                 const struct timeval* timeout);

/*
* 成功时返回大于0的值，失败时返回-1
* maxfd     : 监视对象文件描述符的数量
* readset   : 将关注的所有"是否存在待读取数据"的文件描述符注册到其指针指向值
* writeset  : 将关注的所有"是否可传输无阻塞数据"的文件描述符注册到其指针指向值
* exceptset : 将关注的所有"是否发生异常"的文件描述符注册到其指针指向值
* timeout   : 调用select()后，为防止陷入无限阻塞，传递超时信息
* 返回值     : 发生错误返回-1，超时返回0，因发生关注事件返回时，该值为发生事件的文件描述符数
*/


// 设置监视范围 : 最大的文件描述符加1(文件描述符值从0开始)

// select()只有在监视的文件描述符发生变化时才返回，若无则进入阻塞状态，因此需制定超时时间

struct timeval{
    long tv_sec;  //秒
    long tu_usec; //微秒，百万分之一秒
};

// 制定timeout到了规定的时间如果监视的文件描述符没有变化就会返回0，不需要可以指定NULL



/*
* NOTE:
* 调用select()后，向其传递的fd_set变量将发生变化，
* 原来为1的变成0，除非发生变化的文件描述符，所以可认为值仍为1的位置上的文件描述符发生了变化
*/
```







### 设置监视文件描述符



```C
/*
* 首先需要将要监听的文件描述符集中到一起，集中时按照监视项(接收、传输、异常)
* 进行区分，按照监视项分成三类
*/

/*fd_set 数组变量执行该操作，该数组是存有0和1的位数组，若第n位是1，说明该文件描述符是监听对象*/


/*关于 fd_set 的宏*/

FD_ZERO(fd_set* fdset);        // 将fd_set变量的所有位初始化为0
FD_SET(int fd,fd_set* fdset);  // 向fdset指向的变量注册文件描述符fd的信息
FD_CLR(int fd,fd_set* fdset);  // 从fdset指向的变量清除文件描述符fd的信息
FD_ISSET(int fd,fd_set* fdset) // 若setfd指向的变量中包含文件描述符fd的信息，则返回真



```



### 简单示例



```C
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 32

int main(int argc,char* argv[]){
    fd_set reads,temps;
    int result,str_len;
    char buf[BUF_SIZE];
    struct timeval timeout;
    FD_ZERO(&reads);   // 初始化fd_Set变量
    FD_SET(0,&reads);  // 监视标准输入变化
    
    /*不应该在此处设置timeval的超时时间，应该在select()之前调用*/
    //timeout.tv_sec=5;
    //timeout.tv_usec=5000;
    
    while(1){
        temps=reads;    // 将temps恢复为初始
        timeout.tv_sec=5;
        timeout.tv_usec=0;
        result=select(1,&temps,0,0,&timeout);
        if(result==-1){
            puts("select() error!");
            break;
        }
        else if(result==0){
            puts("Time-out!");
        }       
        else{
            if(FD_ISSET(0,&temps)){ // 标准输出文件描述符是否发生变化
                str_len=read(0,buf,BUF_SIZE);
                buf[str_len]=0;
                printf("message from console: %s",buf);
            }
        }
    }
    return 0;
}


```



## 二、简单实现



```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 128

void error_handling(char* message);

int main(int argc,char* argv[]){
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_addr,clnt_addr;
    socklen_t addr_sz;

    struct timeval timeout;  // 用于设置select()的超时时间
    fd_set reads,cpy_reads;  // 创建fd_set位数组

    int fd_max,i,str_len,fd_num; // fd_max为监视的最大值，fd_num用于接受select返回值
    char buf[BUF_SIZE];

    if(argc!=2){printf("Usage:%s <port>\n",argv[0]);exit(1);}

    serv_sock=socket(PF_INET,SOCK_STREAM,0);
    if(serv_sock==-1){error_handling("socket() error!\n");}

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));
        
    if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
        error_handling("bind() error!\n");
    }
        
    if(listen(serv_sock,5)==-1){error_handling("listen() error!\n");}
 
    FD_ZERO(&reads);  // 初始化
    FD_SET(serv_sock,&reads); // 将serv_sock设置为监视对象
    fd_max=serv_sock;         // fd_max 为监视的最大值，监视范围为fd_max+1
    while(1){
        cpy_reads=reads; // 保存初始态
        
        /*一定要在select()执行之前设置，可以看出，它跟select()一样是伴随这循环的*/
        timeout.tv_sec=5;
        timeout.tv_usec=5000;
        
        if((fd_num=select(fd_max+1,&cpy_reads,0,0,&timeout))==-1){break;}
        if(fd_num==0){continue;}    // 说明监视范围的文件描述符无变化，cpy_reads都为0
        
        for(i=3;i<fd_max+1;++i){   // 忽略标准的文件描述符从3开始
            if(FD_ISSET(i,&cpy_reads)){  // 文件描述符i发生变化
                if(i==serv_sock){// 若i为serv_sock说明有客户端请求连接，需要调用accept()      
                    addr_sz=sizeof(clnt_addr);
                    clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&addr_sz);
                    FD_SET(clnt_sock,&reads); // 将clnt_sock更新到初始态
                    if(fd_max<clnt_sock){fd_max=clnt_sock;} // 检查是否需要更新fd_max
                    printf("Connected client: %d\n",clnt_sock);
                }
                else{ // 若i为与客户端连接的套接字文件描述符
                    str_len=read(i,buf,BUF_SIZE);   // 读数据
                    if(str_len==0){ // 接收了EOF信息客户端，read()返回0,
                        FD_CLR(i,&reads); // 清除该套接字在初始态的信息
                        close(i); // 关闭该套接字文件描述符
                        printf("Close client :%d\n",i);
                        if(fd_max==i){fd_max-=1;} // 检查是否需要更新fd_max
                    }
                    else{write(i,buf,str_len);} // 向客户端写
                }
            }
        }
    } 
    close(serv_sock);
    return 0; 
}

void error_handling(char* message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}

```

