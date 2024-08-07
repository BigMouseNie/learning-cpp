# send()&recv()



## 一、Define of send()&recv()



### send() 

```C
#include <sys/socket.h>
ssize_t send(int sockfd, const void* buf, size_t nbytes, int flags);

/*
* 成功时返回发送的字节数，失败时返回-1
* sockfd : 与数据传输对象相连的套接字文件描述符
* buf    : 保存待传输数据的缓冲地址
* nbytes : 待传输的字节数
* flags  : 待传输数据时指定的可选选项
*/

```





### recv()

```C
#include <sys/socket.h>
ssize_t recv(int sockfd, const void* buf, size_t nbytes, int flags);

/*
* 成功时返回接收的字节数(收到EOF返回0)，失败时返回-1
* sockfd : 与数据接收对象相连的套接字文件描述符
* buf    : 保存接收的数据的缓冲地址
* nbytes : 接收的字节数
* flags  : 接收数据时指定的可选选项
*/


```



## 二、Options of flags 



| option       | 含义                                   | send() | recv() |
| ------------ | -------------------------------------- | ------ | ------ |
| MSG_OOB      | 用于传输带外数据                       | √      | √      |
| MSG_PEEK     | 验证缓冲地址是否存在接收的数据         |        | √      |
| MSG_DONTWAIT | 调用I/O函数时不阻塞，用于使用非阻塞I/O | √      | √      |
| MSG_WAITALL  | 防止函数返回，直到接收全部字节数       |        | √      |



### MSG_OOB







































