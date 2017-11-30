//
//  RemoteCallEngine.c
//  TestSelectClient
//
//  Created by Yuxi Liu on 10/17/12.
//  Copyright (c) 2012 Yuxi Liu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include <unistd.h>
#include <errno.h>

#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h> //for SL_DEBUG mode

/*the header about the socket*/
#include <sys/socket.h> /* socket bind listen connect accept send recv */
#include <arpa/inet.h>  /* htons ntohs htonl ntohl inet_addr inet_ntoa */
#include <netinet/in.h> /* sockaddr_in */


#include "../debug/yx_core_assert.h"

//#include "cmMem.h"


#include "../thread/threadpool.h"
#include "../encryption/yx_core_encrypt_randomdict.h"

#include "common_engine.h"

#include "client_engine.h"
//#include "../../debug/libDebug.h"




#define _SL_CLIENT_MSGTHREAD_SEM_FILE_FORMAT "/tmp/%dclient.semb"


#define SLCLIENTHANDLE2IMPL(h) ((_SL_select_client_context_ref)h)





typedef struct _cliJobMsg{
    void* buf;
    int nLen; 
    
    yx_allocator allocator;
    
    HSLClientMsgHandleFun callback_msgHandleFun;
}cliJobMsg, *cliJobMsgRef;




typedef struct ___select_client_context_{
    int cliFd;
    
    yx_allocator allocator;
    
    pthread_t msgThreadID;
    sem_t* msgSem;
    
    yx_core_threadpool_ref threadPool;
    
    pthread_mutex_t mutex;
    
    HSLClientMsgHandleFun callback_msgHandleFun;
    HSLClientConnectionClosedHandleFun callback_connectionClosedHandleFun;
    
    
    yx_core_encrypt_randomdict encryption;
    
}_SL_select_client_context, *_SL_select_client_context_ref;

//server operation
static int _SLF_InitIPV4TCPStreamClientAddress(struct sockaddr_in* addr, const char* address, int port);
static int _SLF_InitIPV4Client(const struct sockaddr_in *addr, socklen_t sockLen);
static int _beginClientSelect(int sd);
static int _SLF_MagageServerMsg(_SL_select_client_context_ref context);
static int _askServerToCloseConnection(_SL_select_client_context_ref context);


static void _job_msg(void* msgPtr);



HSLSelectClient SLFSetupClient(yx_allocator allocator, const char* address, int port, HSLClientMsgHandleFun callback_msgHandle, HSLClientConnectionClosedHandleFun callback_connectionClosed){
    
    _SL_select_client_context_ref context = NULL;
    
    struct sockaddr_in cliAddr;
    socklen_t sockLen = sizeof(struct sockaddr);
    int cliFd;
    
    pthread_t msgThreadID = NULL;
    
    sem_t* msgSem = SEM_FAILED;
    
    yx_bool isMutexInit = yx_false;
    
    yx_core_encrypt_randomdict encryption = NULL;
    
    
    yx_core_threadpool_ref threadPool = NULL;
    
    
    if (NULL == allocator)
        allocator = yx_get_allocator();
    
    
    _SLF_InitIPV4TCPStreamClientAddress(&cliAddr, address, port);
    
    if((cliFd = _SLF_InitIPV4Client(&cliAddr, sockLen)) < 0)
        goto errout;
    
    if(NULL == (context = (_SL_select_client_context_ref)yx_allocator_alloc(allocator, sizeof(_SL_select_client_context))))
        goto errout;
    
    context->allocator = allocator;
    
    context->cliFd = cliFd;
    
    
    if(NULL == (threadPool = yx_core_threadpool_create(context->allocator, remote_call_max_thread_in_pool)))
        goto errout;
    context->threadPool = threadPool;
    
    
    if(0 != pthread_mutex_init(&context->mutex, NULL))
        goto errout;
    isMutexInit = yx_true;
    
    
    unsigned int key = yx_core_encrypt_randomdict_seedKeyFromString("360_rpc_module", port);
    encryption = yx_core_encrypt_randomdict_create(context->allocator, key, 13);
    if(NULL == encryption)
        goto errout;
    context->encryption = encryption;
    
    
    char pszSemPath[1024];
    sprintf(pszSemPath, _SL_CLIENT_MSGTHREAD_SEM_FILE_FORMAT, (int)getpid());
    sem_unlink(pszSemPath);
    if(SEM_FAILED == (msgSem = sem_open(pszSemPath, O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH, 0)))
        goto errout;
    
    context->msgSem = msgSem;
    
    if(0 != (pthread_create(&msgThreadID, NULL, (void *(*)(void *))_SLF_MagageServerMsg, (void*)context)))
        goto errout;
    context->msgThreadID = msgThreadID;
    
    
    context->callback_msgHandleFun = callback_msgHandle;
    context->callback_connectionClosedHandleFun = callback_connectionClosed;
    
    
    sem_post(context->msgSem);
    
    
    
    return (HSLSelectClient)context;
    
errout:
    //force the thread closed -- i know, it's ugly
    if(NULL != msgThreadID)
        pthread_cancel(msgThreadID);
    
    if(cliFd>0)
        close(cliFd);
    
    if(NULL != encryption)
        yx_core_encrypt_randomdict_destroy(&encryption);
    
    
    if(NULL != threadPool)
        yx_core_theadpool_destroy(&threadPool, 1);
    
    
    if(SEM_FAILED != msgSem)
        sem_destroy(msgSem);
    
    
    if(NULL != context){
        
        if(isMutexInit == yx_true){
            pthread_mutex_destroy(&(context->mutex));
        }
        
        yx_allocator_free(allocator, context);
    }
    
    return NULL;
}



void SLFShutDownClient(HSLSelectClient* handRef, yx_bool tryToWait){
    if(NULL==handRef || NULL == *handRef)
        return;
    
    _SL_select_client_context_ref context = SLCLIENTHANDLE2IMPL(*handRef);
    
    _askServerToCloseConnection(context); //if the server is already shutdown, this function will fail. It's ok
    
    double exittimes = 0;
    int semFlag = -1;
    //just try to wait several time slice!!
    while (yx_true == tryToWait  &&  0 != (semFlag = sem_trywait(context->msgSem)) && exittimes<1000 ){
        sleep(0);//give up the time slice
        exittimes += 1;
    }
    
    
    if(semFlag != 0){
        pthread_cancel(context->msgThreadID);
    }

    pthread_join(context->msgThreadID, NULL);
    
    if(NULL != context->threadPool)
        yx_core_theadpool_destroy(&(context->threadPool), tryToWait==yx_true? 1 : 0);
    
    pthread_mutex_destroy(&(context->mutex));
    
    
    if(context->cliFd>0)
        close(context->cliFd);
    
    sem_close(context->msgSem);

    
    if(NULL != context->encryption)
        yx_core_encrypt_randomdict_destroy(&(context->encryption));
        
    
    if(NULL != context)
        yx_allocator_free(context->allocator, context);
    
    *handRef = NULL;
}



int SLFSendData(HSLSelectClient handle, void* buf, size_t size){
    
    if(NULL == handle)
        return -1;
    
    if(NULL == buf)
        return -1;
    
    if(size <= 0)
        return -1;
    
    
    _SL_select_client_context_ref context = SLCLIENTHANDLE2IMPL(handle);
    int rt;
    
    RemoteMsgRef msg = (RemoteMsgRef)yx_allocator_alloc(context->allocator, sizeof(RemoteMsg) + size);
    msg->nLen = htonl((int)size);
    strcpy(msg->szCmd, SF_RemoteMsg_data);
    memcpy(msg->szData, buf, size);
    
    yx_core_encrypt_randomdict_encrypt(context->encryption, (unsigned char*)msg->szData, (unsigned char*)msg->szData, size);
    yx_core_encrypt_randomdict_encrypt(context->encryption, (unsigned char*)msg, (unsigned char*)msg, sizeof(RemoteMsg));
    pthread_mutex_lock(&(context->mutex));
    rt = _ISF_Write(context->cliFd, (void*)(msg), sizeof(RemoteMsg)+size, NULL);
    pthread_mutex_unlock(&(context->mutex));
        
    
    yx_allocator_free(context->allocator, msg);
    
    
    return rt;
}








/*****************************************************/
/*client linked function*/
/*****************************************************/
static int _SLF_InitIPV4TCPStreamClientAddress(struct sockaddr_in* addr, const char* address, int port){
    
	memset((void *)addr, 0, sizeof(struct sockaddr));
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr(address);
	addr->sin_port = htons(port);
    
    return 0;
}


static int _SLF_InitIPV4Client(const struct sockaddr_in *addr, socklen_t sockLen){
    int fd;
    int iSockAttrOn = 1;
    
    if ((fd = socket(addr->sin_family, SOCK_STREAM, 0)) < 0){
		printf("socket err:%s\n", strerror(errno));
		return -1;
	}
    
    //set the port reuse
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &iSockAttrOn, sizeof(iSockAttrOn) ) < 0)
		return -1;
    //ignore the sigpipe on socket
    if (setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &iSockAttrOn, sizeof(iSockAttrOn) ) < 0)
		return -1;

    
	if (connect(fd, (struct sockaddr *)addr, sockLen) < 0){
		printf("connect err:%s\n", strerror(errno));
		return -1;
	}
    
	memset((void *)addr, 0, sockLen);
	getsockname(fd, (struct sockaddr *)addr, &sockLen);
	printf("connect OK, 本机IP:%s, Port:%d\n", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
    

    
    return fd;
}

static int _beginClientSelect(int sd){
    fd_set fdset;
    struct timeval timeout = {5, 0};
    FD_ZERO(&fdset);
    FD_SET(sd, &fdset);
    
    return select(sd+1, &fdset, NULL, NULL, &timeout);
}

static int _SLF_MagageServerMsg(_SL_select_client_context_ref context){
    
    int ret;
    char szBuf[remote_call_max_buff_len];
    memset(szBuf, 0, remote_call_max_buff_len);
    
    
    /*suspend the thread until the context is ready*/
    sem_wait(context->msgSem);
    
    
    while (1) {
        ret = _beginClientSelect(context->cliFd);
        
        if(ret<0){
            printf("select err:%s\n", strerror(errno));
            exit(-1);
        }
        else if(ret == 0){ //time out
        }
        else{
            memset(szBuf, 0, remote_call_max_buff_len);
            
            ret = _ISF_Read(context->cliFd, szBuf, sizeof(RemoteMsg), NULL);
            yx_core_encrypt_randomdict_decrypt(context->encryption, (unsigned char*)szBuf, (unsigned char*)szBuf, sizeof(RemoteMsg));
            if(ret == 0){
                RemoteMsgRef msgRecv = (RemoteMsgRef)szBuf;
                msgRecv->nLen = ntohl(msgRecv->nLen);
                if(msgRecv->nLen > 0)
                    ret = _ISF_Read(context->cliFd, szBuf+sizeof(RemoteMsg), msgRecv->nLen, 0);
                    yx_core_encrypt_randomdict_decrypt(context->encryption, (unsigned char*)(szBuf+sizeof(RemoteMsg)), (unsigned char*)(szBuf+sizeof(RemoteMsg)), msgRecv->nLen);
            }
            
            //0 means read success
            //others means the client error.(EPIPE means the client is closed)
            if(ret != 0)
            {
                if(ret == EPIPE)
                    printf("recv server close!\n");
                else
                    printf("recv err:%s\n", strerror(ret));
                
                if(NULL != context->callback_connectionClosedHandleFun){
                    context->callback_connectionClosedHandleFun();
                }
                
                close(context->cliFd);
                break;
            }
            
//            ret = (int)recv(context->cliFd, szBuf, remote_call_max_buff_len, 0);
//            if(ret < 0){ //TODO
//                printf("recv err:%s\n", strerror(errno));
//                close(context->cliFd);
//                break;
//            }
//            else if(ret == 0){ //TODO
//                printf("recv server close!\n");
//                close(context->cliFd);
//                break;
//            }
            else{
                RemoteMsgRef msgRecv = (RemoteMsgRef)szBuf;
//                msgRecv->nLen = ntohl(msgRecv->nLen);
                
                if(strcmp(msgRecv->szCmd, SF_RemoteMsg_bye) == 0){
                    close(context->cliFd);
                    context->cliFd = -1;
                    printf("server say bye\n");
                    /*exit the "while" loop*/
                    break;
                }
                else if(strcmp(msgRecv->szCmd, SF_RemoteMsg_data) == 0 && msgRecv->nLen > 0)
                {
                    
                    cliJobMsgRef jobMsg = NULL;
                    if(NULL != (jobMsg = (cliJobMsgRef)yx_allocator_alloc(context->allocator, sizeof(cliJobMsg))))
                    {
                        jobMsg->buf = NULL;
                        jobMsg->nLen = msgRecv->nLen;
                        jobMsg->callback_msgHandleFun = context->callback_msgHandleFun;
                        jobMsg->allocator = context->allocator;

 //                       if(jobMsg->nLen > 0){
                            if(NULL != (jobMsg->buf = yx_allocator_alloc(context->allocator, jobMsg->nLen))){
                                memcpy(jobMsg->buf, msgRecv->szData, jobMsg->nLen);
                                
#ifdef DEBUG
                                int ret = 0;
                                if(-1 == (ret = yx_core_thread_addTask(context->threadPool, _job_msg, (void*)jobMsg, 0))){
                                    printf("an error had occurred while adding a task");
                                }
                                else if(-2 == ret){
                                    //you know...
                                    YX_ASSERT(0);
                                }
                                

#else
                                
                                /*
                                 Note :disable the mempool behavior monitoring in release version
                                 When you are not the correct use of the remote-module,
                                 it may cause low performance or terrible accident.
                                 
                                 So,
                                 
                                 ---You should made a sufficient testing in debug Version.
                                 
                                 what's more
                                 
                                 ---A flexible thread is need.
                                 
                                 */
                                if(-1 == yx_core_thread_addTask(context->threadPool, _job_msg, (void*)jobMsg, 1)){
                                    printf("an error had occurred while adding a task");
                                }
#endif
                            }
                            else{
                                printf("failed to alloc the message");
                                yx_allocator_free(context->allocator, jobMsg);
                            }
 //                       }
                    } //if(NULL != (jobMsg = (cliJobMsgRef)MALLOC(sizeof(cliJobMsg))))
                    
                    
                } //else if(strcmp(msgRecv->szCmd, SF_RemoteMsg_data) == 0)
                else
                {
                    //unknow command
                }
            }
        } //if(ret<0){
    } //while (1) {
    
    
    //tell the function will be exit
    //the waiting thread should call the pthread_join function to collection the resource
    sem_post(context->msgSem);
    
    return 0;
}


static int _askServerToCloseConnection(_SL_select_client_context_ref context){
    
    char szBuf[remote_call_max_buff_len];
    
    RemoteMsg msg;
    strcpy(msg.szCmd, "WANT_EXIT");
    msg.nLen = 0;
    msg.nLen = htonl(msg.nLen);
    
    
    yx_core_encrypt_randomdict_encrypt(context->encryption, (unsigned char*)(msg.szData), (unsigned char*)(msg.szData), 0);
    yx_core_encrypt_randomdict_encrypt(context->encryption, (unsigned char*)(&msg), (unsigned char*)(&msg), sizeof(RemoteMsg));
    
    memset(szBuf, 0, remote_call_max_buff_len);
    memcpy(szBuf, &msg, sizeof(msg)+0);
    
    pthread_mutex_lock(&(context->mutex));
    _ISF_Write(context->cliFd, szBuf, sizeof(msg)+0, NULL);
    pthread_mutex_unlock(&(context->mutex));
    //send(context->cliFd, szBuf, sizeof(msg)+0, 0);
    
    
    return 0;
}


/*****************************************************/
/*job operation functions*/
/*****************************************************/


//job - this function is using for handle the socket msg in memory pool
static void _job_msg(void* msgPtr){
    cliJobMsgRef jobMsg = (cliJobMsgRef)msgPtr;
    
    if(NULL == jobMsg)
        return;
    
    //#pragma warnring 1
    //jobMsg->callBack((HSLSocket)(jobMsg->));
    jobMsg->callback_msgHandleFun(jobMsg->buf, jobMsg->nLen);
    
    
    //:~ TODO need refactoring
    //this is ugly
    //this buff is alloced by _handleMsg fun.
    if(NULL != jobMsg->buf)
        yx_allocator_free(jobMsg->allocator, jobMsg->buf);
    
    //:~ TODO need refactoring
    //this is ugly
    //this buff is alloced by _handleMsg fun.
    if(NULL != jobMsg)
        yx_allocator_free(jobMsg->allocator, jobMsg);
}


















