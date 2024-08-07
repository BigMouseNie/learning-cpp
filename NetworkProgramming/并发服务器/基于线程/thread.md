



```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

void* send_msg(void* arg);
void* recv_msg(void* arg);
void error_handling(char* msg);

char name[NAME_SIZE]="[DEFAULT]";
char msg[BUF_SIZE];

int main(int argc,char* argv[]){
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t send_thread,recv_thread;
    void* thread_return;
    if(argc!=4){
        printf("Usage : %s <IP> <PORT> <name>\n",argv[0]);
        exit(1);
    }
   
    sprintf(name,"[%s]",argv[3]);
   
    sock=socket(PF_INET,SOCK_STREAM,0);
    
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){error_handling("connect() error!\n");}
    else{puts("Connected!");}
    pthread_create(&send_thread,NULL,send_msg,(void*)&sock);
    pthread_create(&recv_thread,NULL,recv_msg,(void*)&sock);

    pthread_join(send_thread,&thread_return);
    pthread_join(recv_thread,&thread_return);

    close(sock);
    return 0;
}


void* send_msg(void* arg){
    int sock=*((int*)arg);
    char name_msg[NAME_SIZE+BUF_SIZE];
    
    while(1){
        fgets(msg,BUF_SIZE,stdin);
        if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n")){
            close(sock);
            exit(0);
        }
        sprintf(name_msg,"%s %s",name,msg);
        write(sock,name_msg,strlen(name_msg));
    }
    return NULL;
}

void* recv_msg(void* arg){
    int sock=*((int*)arg);
    char name_msg[NAME_SIZE+BUF_SIZE];
    int str_len;

    while(1){
        str_len=read(sock,name_msg,NAME_SIZE+BUF_SIZE-1);
        if(str_len==-1){
            return (void*)-1;
        }
        name_msg[str_len]=0;
        fputs(name_msg,stdout);
    }
    return NULL;
}

void error_handling(char* message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}






```









```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void* handle_clnt(void* arg);
void send_msg(char* msg,int len);
void error_handling(char* msg);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc,char* argv[]){
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_addr,clnt_addr;
    int clnt_addr_sz;
    pthread_t t_id;

    if(argc!=2){printf("Usage : %s <port>\n",argv[0]);exit(1);}

    pthread_mutex_init(&mutx,NULL);
    serv_sock=socket(PF_INET,SOCK_STREAM,0);

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
        error_handling("bind() error!");
    }
        
    if(listen(serv_sock,5)==-1){error_handling("listen() error!");}

    while(1){
        clnt_addr_sz=sizeof(clnt_addr);
        clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_sz);
        
        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++]=clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id,NULL,handle_clnt,(void*)&clnt_sock);
        pthread_detach(t_id);
        printf("Connected clint IP : %s\n",inet_ntoa(clnt_addr.sin_addr));
    }
    close(serv_sock);
    return 0;
}

void* handle_clnt(void* arg){
    int clnt_sock=*((int*)arg);
    int str_len=0,i;
    char msg[BUF_SIZE];

    while((str_len=read(clnt_sock,msg,sizeof(msg)))!=0){
        send_msg(msg,str_len);
    }
    pthread_mutex_lock(&mutx);
    for(i=0;i<clnt_sock;++i){    
        if(clnt_sock==clnt_socks[i]){
            while(i++<clnt_cnt-1){clnt_socks[i]=clnt_socks[i+1];}
            break;                      
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return 0;
}


void send_msg(char* msg,int len){
    int i;
    pthread_mutex_lock(&mutx);
    for(i=0;i<clnt_socks[i];++i){write(clnt_socks[i],msg,len);}
    pthread_mutex_unlock(&mutx);
}

void error_handling(char* message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}




```

