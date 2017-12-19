#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define MAX_FD_SIZE 1024
#define PORT        8888
typedef struct _TServerContext{
    int listenSock;//监听套接字
    int iCliCnt;//客户端套接字数量
    int iCliFds[MAX_FD_SIZE];//客户端套接字数组
    struct timeval tv;//select时间参数结构体
    struct sockaddr_in SrvAddr;//服务端地址结构体
    int MaxFd;//数值最大的套接字
    fd_set AllFdSet;
}T_SrvContext;
typedef T_SrvContext  *PT_SrvContext;
static PT_SrvContext g_ptSrvContext;

int main(int argc,char* argv[])
{
    unsigned int i ; 
    //首先给结构体指针分配内存
    g_ptSrvContext = (PT_SrvContext)malloc(sizeof(T_SrvContext));
    memset(g_ptSrvContext,0,sizeof(T_SrvContext));
    if(g_ptSrvContext == NULL)
    {   
        printf("malloc server object failed\n");
        exit(1);
    }   
    //创建套接字
    g_ptSrvContext->listenSock = socket(AF_INET,SOCK_STREAM,0);
    if(g_ptSrvContext->listenSock == -1) 
    {   
        perror("create listen socket error");
        exit(1);
    }   
    //绑定套接字
    g_ptSrvContext->SrvAddr.sin_family = AF_INET;
    g_ptSrvContext->SrvAddr.sin_port = htons(PORT);
    g_ptSrvContext->SrvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(g_ptSrvContext->listenSock,(struct sockaddr*)&g_ptSrvContext->SrvAddr,sizeof(g_ptSrvContext->SrvAddr)) == -1) 
    {   
        perror("bind error");
        exit(1);
    }   
    //监听
    if(listen(g_ptSrvContext->listenSock,MAX_FD_SIZE) == -1) 
    {   
        perror("listen error");
        exit(1);
    }   
    //初始化所有的fd数组
    for(i = 0;i<MAX_FD_SIZE;++i)
    {   
        g_ptSrvContext->iCliFds[i] = -1; 
    }   
    //把listen套接字放入select
    fd_set *pReadSet = &g_ptSrvContext->AllFdSet;
    while(1)
    {   
        int iRet;
        //初始化select集合
        FD_ZERO(pReadSet);
        FD_SET(g_ptSrvContext->listenSock,pReadSet);
        g_ptSrvContext->MaxFd = g_ptSrvContext->listenSock;
        for(i = 0;i<MAX_FD_SIZE;++i)
        {   
            int iTmpFd = g_ptSrvContext->iCliFds[i];
            FD_SET(iTmpFd,pReadSet);
            g_ptSrvContext->MaxFd = g_ptSrvContext->MaxFd > iTmpFd?g_ptSrvContext->MaxFd:iTmpFd;
        }   
        g_ptSrvContext->tv.tv_sec = 20; 
        g_ptSrvContext->tv.tv_usec = 0;
        //进行轮询
        iRet = select(g_ptSrvContext->MaxFd+1,pReadSet,NULL,NULL,&g_ptSrvContext->tv);
        if(iRet == 0)
        {   
            printf("select time out\n");
            continue;
        }   
        else if(iRet < 0)
        {   
            perror("select error");
            exit(1);
        }   
        //如果是监听
        if(FD_ISSET(g_ptSrvContext->listenSock,pReadSet))
        {   
            int iTmpFd = -1; 
            struct sockaddr_in CliNew;
            socklen_t len = sizeof(CliNew);
            while(iTmpFd == -1) 
            {   
                iTmpFd = accept(g_ptSrvContext->listenSock,(struct sockaddr*)&CliNew,&len);
                if(iTmpFd == -1) 
                {   
                    printf("accept error\n");
                    continue;
                }   
            }   

            for(i = 0;i<MAX_FD_SIZE;++i)
            {   
                if(g_ptSrvContext->iCliFds[i] == -1) 
                {   
                    g_ptSrvContext->iCliFds[i] = iTmpFd;
                    g_ptSrvContext->iCliCnt++;
                    break;
                }   
            }   
        }   
        for(i = 0;i<g_ptSrvContext->iCliCnt;++i)
        {   
            int iTmpFd = g_ptSrvContext->iCliFds[i];
            if(iTmpFd == -1) 
                continue;
            if(FD_ISSET(iTmpFd,pReadSet))
            {   
                char recvbuf[1024] = {0};
                iRet = read(iTmpFd,recvbuf,1024);
                if(iRet == -1&&errno == EINTR)//系统中断引起的情况下
                {   
                    continue;   
                }   
                else if(iRet == -1 || iRet == 0)
                {   
                    perror("read error");
                    FD_CLR(iTmpFd,pReadSet);
                    close(iTmpFd);
                    g_ptSrvContext->iCliFds[i] = -1; 
                    continue;
                }   
                printf("recv %s\n",recvbuf);
                write(iTmpFd,"hello client\n",strlen("hello client\n"));
            }   
        }   
    }   
    return 0;
}
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
~                                                                                                                                                                                                                                                                   
:set nonu                                                                                                                                                                                                                                         1,1          全部
