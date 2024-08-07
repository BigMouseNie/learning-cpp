# Server&Client



## 一、基于UDP的I/O函数



### sendto()

```C
/*类write()*/
#include <sys/socket.h>
ssize_t sendto(int sock,void* buff,size_t nbytes,
                       				int flags,struct sockaddr* to,socklen_t addrlen);

/*
* 成功时返回0，失败时返回-1
* sock   : 用于传输数据的UDP套接字文件描述符
* buff   : 保存待传输数据得缓冲地址
* nbytes : 待传输的数据长度，以字节为单位
* flags  : 可选参数
* to     : 存有目标地址信息的指针
* addrlen : 传递给参数to的地址值变量的长度
*/

/*
NOTE:
调用sendto()时尚未发现分配地址信息，则在首次调用senfto()时给对应的套接字自动分配IP和端口，
并且此时分配的地址一直保留到程序结束为止
*/

```



### recvfrom()

```c
/*类read()*/
#include <sys/socket.h>
ssize_t recvfrom(int sock,void* buff,size_t nbytes,int flags,
                 							struct sockaddr* from,socklen_t* addrlen);

/*
* 成功时返回0，失败时返回-1
* sock    : 用于接收数据的UDP套接字文件描述符
* buff    : 保存接收数据的缓冲地址
* nbytes  : 可接收的最大字节数，不能超出buff所指的缓冲大小
* flags   : 可选参数
* from    : 存有发送端地址信息的指针
* addrlen : 保存参数from所指的值的长度
*/

```



## 二、Server&Client的简单实现



### Server

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BF_SIZE 30
void error_handling(char* message);

int main(int argc,char* argv[]){
    int serv_sock;
    int str_len;
    struct sockaddr_in serv_addr,clnt_addr;
    socklen_t clnt_addr_sz;
    char message[BF_SIZE];

    if(argc!=2){printf("Usage:%s <port>\n",argv[0]);exit(1);}

    serv_sock=socket(PF_INET,SOCK_DGRAM,0); // 创建UDP套接字
    if(serv_sock==-1){error_handling("socket() error!\n");}
	
    /*初始化服务端地址信息*/
    memset(&serv_addr,0,sizeof(serv_sock));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));  
	
    /*分配IP地址和端口号*/
    if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1{
        error_handling("bind() error!\n");
    }

    while(1){
        clnt_addr_sz=sizeof(clnt_addr);
        str_len=recvfrom(serv_sock,message,BF_SIZE,0,
                         (struct sockaddr*)&clnt_addr,&clnt_addr_sz);
        sendto(serv_sock,message,str_len,0,(struct sockaddr*)&clnt_addr,clnt_addr_sz);
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



### Client

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char* message);

int main(int argc,char* argv[]){
    int sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t addr_sz;
    struct sockaddr_in serv_addr,from_addr;

    if(argc!=3){printf("Usage:%s <IP> <port>\n",argv[0]);exit(1);}

    sock=socket(PF_INET,SOCK_DGRAM,0);
    if(sock==-1){error_handling("socket() error!\n");}

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    while(1){
        fputs("Input message(q to quit):",stdout);
        fgets(message,sizeof(message),stdin);
        if(!strcmp(message,"q\n")||!strcmp(message,"Q\n")){break;}
		
        /*自动分配客户端的IP地址和端口号，并保留到程序结束*/
        sendto(sock,message,strlen(message),0,
                                 (struct sockaddr*)&serv_addr,sizeof(serv_addr));
        addr_sz=sizeof(from_addr);
        str_len=recvfrom(sock,message,BUF_SIZE,0,(struct sockaddr*)&from_addr,&addr_sz);
        message[str_len]=0;
        printf("Message from server: %s",message);
    }

    close(sock);
    return 0;
}


void error_handling(char* message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}

```

