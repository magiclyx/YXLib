//
//  sample_client.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/6.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "sample_timer.h"

#include <time.h>

#include <sys/socket.h>
#include <arpa/inet.h>

yx_bool ipv4_client(const yx_char* address)
{
   // int n;
    
    const yx_size bufsize = 1024;
    yx_char recvline[bufsize + 1];
    
    
    int sockfd = -1; //socked
    struct sockaddr_in servaddr; //address
    
    /*创建网际(PF_INET)字节流(SOCK_STREAM)套接字*/
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (yx_unlikely( -1 == sockfd ))
        goto errout;
    
    
    /*初始化套接字地址结构*/
    yx_os_memzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; //地址簇
    servaddr.sin_port = htons(13); //端口号, htons->主机到网络端口号
    if ( yx_unlikely( inet_pton(AF_INET, address, &(servaddr.sin_addr)) < 0 )) //IP地址,这个结构支持IPV6, 以前使用inet_addr.
        goto errout;
    
    /*建立连接*/
    if (yx_unlikely( -1 == connect(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) ))
        goto errout;
    
    
    yx_int n;
    while ( (n = (yx_int)read(sockfd, recvline, bufsize)) > 0)
    {
        recvline[n] = '\0';
        if ( EOF == fputs(recvline, stdout))
        {
            goto errout;
        }
    }
    
    
    if (n < 0)
        goto errout;
    
    
    
    close(sockfd);
    
    
    return yx_true;
    
    
errout:
    
    if (-1 != sockfd)
        close(sockfd);
    
    return yx_false;
}


#define YX_OS_SOCKET_LISTENQUEUE_LEN 1024


yx_bool ipv4_server()
{
    
    const yx_size bufsize = 1024;
    yx_char buff[bufsize + 1];
    
    time_t ticks;

    
    int listenfd = -1; //socked
    struct sockaddr_in servaddr;
    
    /*创建网际(PF_INET)字节流(SOCK_STREAM)套接字*/
    listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if (yx_unlikely( -1 == listenfd ))
        goto errout;

    
    /*初始化套接字地址结构*/
    yx_os_memzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; //地址簇
    servaddr.sin_port = htons(13); //端口号, htons->主机到网络端口号
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //绑定到任意地址
    
    
    /*绑定端口*/
    if (yx_unlikely( -1 == bind(listenfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) ))
        goto errout;
    
    
    /*监听端口*/
    if (yx_unlikely( -1 == listen(listenfd, YX_OS_SOCKET_LISTENQUEUE_LEN ) )) //将套接字转换为监听套接字
        goto errout;
    
    
    
    for (;;)
    {
        int connfd = accept(listenfd, NULL, NULL);
        if (yx_unlikely( -1 == connfd ))
            continue;
        
        ticks = time(NULL);
        
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        if (yx_unlikely( -1 == write(connfd, buff, strlen(buff)) ))
        {
            close(connfd);
            continue;
        }
        
        close(connfd);
    }
    
    
    close(listenfd);
    
    return yx_true;
    
errout:
    
    if (-1 != listenfd)
        close(listenfd);
    
    return yx_false;
}















