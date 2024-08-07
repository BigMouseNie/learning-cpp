# Server&Client



## 一、Data Struct



### struct sockaddr_in

```C
struct sockaddr_in{
    sa_failmy_t      sin_failmy;  // 地址组(Address Failmy)
    uint16_t         sin_port;	  // 16位TCP/UDP端口号,以网络字节序保存
    struct in_addr   sin_addr;	  // 32位IP地址
    char			 sin_zero[8]; // 辅助转换成 struct sockaddr
};

struct in_addr{
    in_addr_t        s_addr;      // 32位的IPv4地址，声明为uint32_t，以网络字节序保存
};

```



Address Failmy :

| 名称     | 含义                       |
| -------- | -------------------------- |
| AF_INET  | IPv4网络协议所使用的地址组 |
| AF_INET6 | IPv6网络协议所使用的地址组 |



### struct sockaddr

```C
struct sockaddr{
    sa_failmy_t    sin_failmy;  // 地址组
    char           sa_data[14]; // 地址信息
};

/*
* sa_data中包括IP地址和端口号，其余部分填充为0
*/

/*
NOTE:
struct sockaddr 是 bind() 所要求的，struct sockaddr_in 是保存 IPv4 地址信息的结构体，
其中的 sin_zero 也是为了 (struct sockaddr*)&(struct sockaddr_in) 时生成 bind() 所
要求的字节流
*/

```



NOTE:
struct sockaddr 是 bind() 所要求的，struct sockaddr_in 是保存 IPv4 地址信息的结构体，其中的 sin_zero

也是为了 (struct sockaddr*)&(struct sockaddr_in) 时生成 bind() 所要求的字节流



## 二、API



### socket()

```c
/*创建套接字*/
#include <sys/socket.h>
int socket(int domain, int type, int protocol);

/*
* 成功时返回文件描述符，失败返回-1
* domain   : 套接字中使用的协议族信息
* type     : 套接字数据传输类型信息,决定套接字的传输方式
* protocol : 计算机通信中使用的协议信息，一般为0，除非该协议族的该数据传输类型有多个时需指定
* 在指定的第一参数domain(ProtocolFailmy)以决定第三参数
*/

```



Protocol Famil :

| 名称     | 协议族 |
| -------- | ------ |
| PF_INET  | IPv4   |
| PF_INET6 | IPv6   |



Type :

```C
SOCK_STREAM;//面向连接(TCP)
SOCK_DGRAM; //面向消息(UDP)
    
```



### bind()

```C
/*向套接字分配网络地址*/
#include <sys/socket.h>
int bind(int sockfd, struct sockaddr* myaddr, socklen_t addrlen);

/*
* 成功时返回0，失败时返回-1
* sockfd : 需要分配地址信息(IP地址和端口号)的套接字
* myaddr : 存有地址信息的结构体指针
* myaddr : 的长度
*/

```



### listen()

```C
/*进入等待请求连接状态*/
#include <sys/socket.h>
int listen(int sock, int backlog);

/*
* 成功时返回0，失败时返回-1
* sock    : 希望进入等待请求连接的套接字，该套接字将成为服务端套接字(监听套接字)
* backlog : 连接请求队列的长度
*/

/*
NOTE:
该服务端套接字充当“门卫”的角色来受理客户端的请求，因此并不与之做数据交换
*/

```



### accept()

```c
/*受理客户端请求，创建与客户端相连的套接字*/
#include <sys/socket.h>
int accept(int sock, struct sockaddr* addr, socklen_t* addrlen);

/*
* 成功时返回创建的套接字文件描述符，失败时返回-1
* sock    : 服务端套接字
* addr    : 保存发起连接请求的客户端地址信息，调用函数后向传递来的地址变量参数填充客户端信息
* addrlen : 存有addr长度的指针，函数调用完成后，该变量被填充到客户端地址长度
/*
NOTE:
accept()所返回的自动创建套接字将用于发起请求的客户端套接字建立连接(自动建立)
*/
```



### connect()

```c
/*向客户端请求连接*/
#include <sys/socket.h>
int connect(int sock, struct sockaddr* servaddr, socklen_t addrlen);

/*
* 成功时返回0，失败时返回-1
* sock     : 客户端套接字
* servaddr : 存有目标服务端地址信息的指针
* addrlen  : servaddr的长度 
*/

/*
NOTE:
客户端的IP地址和端口在调用connect()时由操作系统(在内核)自动地分配,IP为主机IP,端口号随机
*/

```







## 三、Server&Client简单实现



### Server

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char* message);
int main(int argc, char* argv[]){
    int serv_sock; // 服务端套接字
    int clnt_sock; // 接收accept()所创建的与客户端相连的套接字
    
    struct sockaddr_in serv_addr; // 保存服务端的地址信息
    struct sockaddr_in clnt_addr; // 利用accept()接收客户端的地址信息
    socklen_t clnt_addr_size;     // 客户端地址长度  
        
    char message[]="Hello World!\n";
        
    if(argc!=2){                  // 参数个数判断
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }   

    serv_sock=socket(PF_INET, SOCK_STREAM, 0);  // 创建服务端套接字(TCP-面向连接)
    if(serv_sock == -1){error_handling("socket() error");}
	
    /*服务端地址信息初始化*/
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;                   // IPv4地址信息
    // 利用INADDR_ANY分配服务器IP地址，并利用htonl()将其转换为网络字节序
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY); 
    // 将系统传入的端口参数转换为int型，然后利用htons()将其转换为网络字节序
    serv_addr.sin_port=htons(atoi(argv[1]));
	
    // 向套接字分配网络地址
    if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
        error_handling("bind() error");
    }   
    
    // 将其套接字进入可等待连接状态,listen()之后serv_sock将成为真正的服务端套接字
    if(listen(serv_sock,5)==-1){error_handling("listen() error");}

    clnt_addr_size=sizeof(clnt_addr);  // 为accept()做准备
    // 接收accept()所创建的套接字，利用clnt_sock与客户端进行数据交换
    clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if(clnt_sock==-1){error_handling("accept() error");}

    write(clnt_sock,message,sizeof(message)); // 向客户端发送数据
    close(clnt_sock); // 关闭套接字
    close(serv_sock);       
    return 0;
}

void error_handling(char* message){
    fputs(message, stderr);
    fputs('\n', stderr);
    exit(1);
}


```



### Client

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char* message);

int main(int argc,char* argv[]){
    int sock;        
    struct sockaddr_in serv_addr;
    char message[30];
    int str_len;

    if(argc!=3){
        printf("Usage : %s <IP> <port>\n",argv[0]);
        exit(1);
    }

    sock=socket(PF_INET,SOCK_STREAM,0);
    if(sock==-1){error_handling("socket() error");}
	
    /*初始化服务端地址信息*/
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));
	
    // 向服务端发起请求连接，此过程中客户端的IP地址和端口由操作系统自动地分配,IP为主机IP,端口号随机
    if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
        error_handling("connect() error");
    }       

    str_len=read(sock,message,sizeof(message)-1);
    if(str_len==-1){error_handling("read() error");}

    printf("Message from server : %s \n",message);
    close(sock);
    return 0;
}


void error_handling(char* message){
    fputs(message,stderr);
    fputs('\n',stderr);
    exit(1);
}

```





















