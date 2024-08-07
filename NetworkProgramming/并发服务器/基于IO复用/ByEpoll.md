# By Epoll



## 一、data struct and function

```C
/*epoll方式中通过如下结构体epoll_event将发生变化的文件描述符单独集中在一起*/

struct epoll_event{
    __uint32_t    events;
    epoll_data_t  data;
};

typedef union_data{
    void* ptr;
    int fd;
    __uint32_t u32;
    __uint64_t u64;
} epoll_data_t;


/*
* events
* EPOLLIN      :  需要读取的情况  
* EPOLLOUT     :  输出缓冲为空，可以立即发送数据的情况
* EPOLLPRI     :  收到OOB数据的情况
* EPOLLRDHUP   :  断开连接或半连接的情况，这在边缘触发方式下很有用
* EPOLLERR     :  发生错误的情况
* EPOLLET	   :  以边缘触发的方式得到事件通知
* EPOLLONESHOT :  发生一次事件后，相应文件描述符不再收到事件通知，因此需要向epoll_ctl的函数
的第二参数EPOLL_CTL_MOD,再次设置事件
*/









#include <sys/epoll.h>
int epoll_create(int size);

/*
* 成功时返回epoll文件描述符，失败时返回-1
* 该函数创建的文件描述符保存空间称'epoll例程'
* size是建议操作系统的例程大小，但是只是建议，并不起决定作用(Linux2.6.8之后的内核将完全忽略该值)
*/



#inclcude <sys/epoll.h>
int epoll_ctl(int epfd, int op, int fd, struct epoll_event* event);

/*
* 成功时返回0，失败时返回-1
* epfd  : 用于注册监视对象的epoll例程的文件描述符
* op    : 用于指定监视对象的添加、删除或更改等操作
* fd    : 需要注册的监视对象文件描述符
* event : 监视对象的事件类型
*/

/*
* op :
* EPOLL_CTL_ADD : 将文件描述符注册到epoll例程
* EPOLL_CTL_DEL : 将文件描述符从epoll例程删除，传递该参数时向 event传递NULL
* EPOLL_CTL_MOD : 更改注册的文件描述符的关注事件发生情况
*/




#include <sys/epoll.h>
int epoll_wait(int epfd,struct epoll_event* events,int maxevents,int timeout);

/*
* 成功时返回发生事件的文件描述符数，失败时返回-1
* epfd : 表示事件发生监视范围的epoll例程的文件描述符
* events : 保存发生事件的文件描述符集合的结构体地址
* maxevent : 第二个参数可以保存的最大事件数
* timeout : 以千分之一秒为单位的等待时间，传递-1则持续等待，直到事件发生
*/

/*NOTE : 第二个参数所指缓冲需要动态分配*/

```





## 二、简单实现





```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 128
#define EPOLL_SIZE 64

void error_handling(char* message);

int main(int argc,char* argv[]){
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_addr,clnt_addr;
    socklen_t addr_sz;
    int str_len,i;
    char buf[BUF_SIZE];

    struct epoll_event* ep_events;   
    struct epoll_event event;
    int epfd,event_cnt;
    
    if(argc!=2){
        printf("Usage : %s <port>\n",argv[0]);
        exit(1);
    }
    
    serv_sock=socket(PF_INET,SOCK_STREAM,0);

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
        error_handling("bind() error!");
    }
        
    if(listen(serv_sock,5)==-1){error_handling("listen() error!");}

    epfd=epoll_create(EPOLL_SIZE);
    ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);  // 动态分配
    event.events=EPOLLIN;    // 监视读事件
    event.data.fd=serv_sock;
    epoll_ctl(epfd,EPOLL_CTL_ADD,serv_sock,&event);  // 将serv_sock注册到epoll例程
   
    while(1){
        event_cnt=epoll_wait(epfd,ep_events,EPOLL_SIZE,-1);
        if(event_cnt==-1){
            puts("epoll_wait() error!");
            break;
        }
        for(i=0;i<event_cnt;++i){
            if(ep_events[i].data.fd==serv_sock){   // 收到连接请求
                addr_sz=sizeof(clnt_addr);
                clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&addr_sz);
                if(clnt_sock==-1){continue;}
                event.events=EPOLLIN;
                event.data.fd=clnt_sock;
                epoll_ctl(epfd,EPOLL_CTL_ADD,clnt_sock,&event);         
                printf("New client connected : %d\n",clnt_sock);
            }
            else{        // 与客户端相连接的套接字出现待读事件
                str_len=read(ep_events[i].data.fd,buf,BUF_SIZE);
                if(str_len==0){ 
                    // 客户端发出EOF，将该套接字从epoll例程中删除，并将其关闭
                    epoll_ctl(epfd,EPOLL_CTL_DEL,ep_events[i].data.fd,NULL);
                    close(ep_events[i].data.fd);
                    printf("Clint disconnected : %d\n",ep_events[i].data.fd);
                }
                else{
                    write(ep_events[i].data.fd,buf,str_len);
                }
            }
        }
    }
    close(serv_sock);   // 关闭套接字和例程
    close(epfd);       
    return 0;
}


void error_handling(char* message){
    fputs(message,stderr);
    fputs('\n',stderr);
    exit(1);
}



```















