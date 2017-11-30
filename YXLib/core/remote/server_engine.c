//
//  RemoteCallEngine.c
//  TestSelectServer
//
//  Created by Yuxi Liu on 10/16/12.
//  Copyright (c) 2012 Yuxi Liu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include <unistd.h>
#include <errno.h>

#include <pthread.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <signal.h> //for SL_DEBUG mode

/*the header about the socket*/
#include <sys/socket.h> /* socket bind listen connect accept send recv */
#include <arpa/inet.h>  /* htons ntohs htonl ntohl inet_addr inet_ntoa */
#include <netinet/in.h> /* sockaddr_in */

//#include "cmMem.h"
//#include "../../debug/libDebug.h"

#include "../thread/threadpool.h"
#include "server_engine.h"

#include "../encryption/yx_core_encrypt_randomdict.h"
#include "common_engine.h"
#include "../debug/yx_core_assert.h"

#include "../hash/yx_core_hash.h"
#include "../container/thread/yx_core_threadsafe_hashtable.h"



#define _SL_SERVER_MSGTHREAD_SEM_FILE_FORMAT "/tmp/%dserver.semb"

#define SLSERVERHANDLE2IMPL(h) ((_SL_select_server_context_ref)h)



//control message protocol
typedef struct _ControlMsg{
    char szCmd[16];/* message command
                      //-> pipe command
                      EXIT -- the server is exit
                    */
}ControlMsg, *ControlMsgRef;


//:~ TODO using the hash table to instead the linked list. 
//a socket node in socket linked list
typedef struct _SocketNode{
    int socket;
    struct sockaddr_in addr;
    struct _SocketNode* pNext;
}SocketNode, *SocketNodeRef;



typedef struct _jobMsg{
    void* buf; //buff
    int nLen; //buff len
    int connectionID;
    
    char ipAddress[19];
    int port;
    
    yx_allocator allocator;
    
    yx_core_tshashtable_ref hashtable_ref;
    
    //msg handle fun pointer
    HSLServerNewConnectionHandleFun callback_newConFun;
    HSLServerMsgHandleFun callback_msgHandleFun;
    HSLServerConnectionClosedHandleFun callback_conClosedFun;
}JobMsg, *JobMsgRef;



typedef struct _JobRecord{
    pthread_mutex_t mutex;
    int connectionID;
}JobRecord, *JobRecordRef;



typedef struct ___select_server_context_{
    
    yx_allocator allocator;

    SocketNodeRef head; 
    
    SocketNodeRef ctrl;
    int fdCtrl[2];
    
    pthread_t msgThreadID;
    sem_t* msgSem;
    
    yx_core_threadpool_ref threadPool;
    
//    HTSH_Tble hashTbl;
    yx_core_threadsafe_hashtable hashTable;

    
    HSLServerNewConnectionHandleFun callback_newConFun;
    HSLServerMsgHandleFun callback_msgHandleFun;
    HSLServerConnectionClosedHandleFun callback_conClosedFun;
    
    
    yx_core_encrypt_randomdict encryption;
    
}_SL_select_server_context, *_SL_select_server_context_ref;



SocketNodeRef createSocketNode(yx_allocator allocator, int socket, struct sockaddr_in *pAddr);
void deleteSocketList(yx_allocator allocator, SocketNodeRef* headRef);
void addNodeToListEnd(SocketNodeRef head, SocketNodeRef node);
SocketNodeRef deleteNodeBySocketID(yx_allocator allocator, SocketNodeRef head, int socket);
SocketNodeRef nodeFromSocketID(SocketNodeRef head, int socket);
int maxSocketNumFromList(SocketNodeRef head);



int _exitServer(_SL_select_server_context_ref context);


int _initServer(int type, const struct sockaddr *addr, socklen_t sockLen, int maxQueueLen);
int _initIPV4TCPStreamServerAddress(struct sockaddr_in* addr, int port);

static int _SLF_MagageClientMsg(_SL_select_server_context_ref context);

int _beginServerSelect(_SL_select_server_context_ref context, fd_set* fdset);
int _handleNewClient(_SL_select_server_context_ref context, SocketNodeRef node);
int _handleClientErr(_SL_select_server_context_ref context, SocketNodeRef node);
int _handleClientClosed(_SL_select_server_context_ref context, SocketNodeRef node);
int _handleMsg(_SL_select_server_context_ref context, SocketNodeRef node, RemoteMsgRef msgRef);


static void _job_new(void* msgPtr);
static void _job_msg(void* msgPtr);
static void _job_closed(void* msgPtr);


HSLSelectServer setupServer(yx_allocator allocator, int port, HSLServerMsgHandleFun callback_msgHandle, HSLServerNewConnectionHandleFun calback_newConnection, HSLServerConnectionClosedHandleFun callback_connectionClosed){

    SocketNodeRef head = NULL; 
    SocketNodeRef ctrl = NULL; 
    _SL_select_server_context_ref context = NULL;
    int srvFd = -1; 
    
    int fdCtrl[2] = {-1, -1}; 
    
    struct sockaddr_in srvAddr;
    socklen_t sockLen = sizeof(struct sockaddr);
    
    pthread_t msgThreadID = NULL; 
    
    sem_t* msgSem = SEM_FAILED; 
    
    sig_t pipeSet = NULL; 
    
    yx_core_threadpool_ref threadPool = NULL; 
    
    yx_core_encrypt_randomdict encryption = NULL;
    
    
    if (NULL == allocator)
        allocator = yx_get_allocator();

    pipeSet = signal(SIGPIPE, SIG_IGN);
    if (pipeSet == SIG_ERR)
        goto errout;
    
    
    _initIPV4TCPStreamServerAddress(&srvAddr, port);
    
    if((srvFd = _initServer(SOCK_STREAM, (struct sockaddr *)&srvAddr, sockLen, remote_call_max_queue_len)) < 0)
        goto errout;
    
    
    if(NULL == (context = (_SL_select_server_context_ref)yx_allocator_alloc(allocator, sizeof(_SL_select_server_context))))
        goto errout;
    
    
    context->allocator = allocator;
    
    
    if(NULL == (head = createSocketNode(context->allocator, srvFd, &srvAddr)))
        goto errout;
    

    if(NULL == (threadPool = yx_core_threadpool_create(context->allocator, remote_call_max_thread_in_pool)))
        goto errout;
    
    
    if(-1 == pipe(fdCtrl))
        goto errout;
    
    
    if(NULL == (ctrl = createSocketNode(context->allocator, fdCtrl[0], 0)))
        goto errout;
    addNodeToListEnd(head, ctrl);
    
    
    yx_core_tshashtable_init(context->allocator, &(context->hashTable), remote_call_max_hashTbl_len);
    
    
    
    /*init the encryption*/
    unsigned int key = yx_core_encrypt_randomdict_seedKeyFromString("360_rpc_module", port);
    encryption = yx_core_encrypt_randomdict_create(context->allocator, key, 13);
    if(NULL == encryption)
        goto errout;
    context->encryption = encryption;
    
    
    
    
    char pszSemPath[1024];
    sprintf(pszSemPath, _SL_SERVER_MSGTHREAD_SEM_FILE_FORMAT, (int)getpid());
    sem_unlink(pszSemPath);
    if(SEM_FAILED == (msgSem = sem_open(pszSemPath, O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH, 0)))
        goto errout;
    
    
    context->msgSem = msgSem; 
    

    if(0 != (pthread_create(&msgThreadID, NULL, (void *(*)(void *))_SLF_MagageClientMsg, (void*)context)))
        goto errout;
    

    context->head = head;
    context->threadPool = threadPool;
    memcpy(context->fdCtrl, fdCtrl, sizeof(int)*2);
    context->ctrl = ctrl;
    context->msgThreadID = msgThreadID;
    context->callback_msgHandleFun = callback_msgHandle;
    context->callback_newConFun = calback_newConnection;
    context->callback_conClosedFun = callback_connectionClosed;
    
    /*run the msg thread*/
    sem_post(context->msgSem);
    
    return (HSLSelectServer)context;
    
    
errout:
    
    if(NULL != msgThreadID)
        pthread_cancel(msgThreadID);
    
    if(NULL != threadPool)
        yx_core_theadpool_destroy(&threadPool, 1);
    
    
    if(srvFd > 0)
        close(srvFd);
    
    
    if(NULL != encryption)
        yx_core_encrypt_randomdict_destroy(&encryption);
    
    
    if(-1 != fdCtrl[0])
        close(fdCtrl[0]);
    if(-1 != fdCtrl[1])
        close(fdCtrl[1]);
    
    
    yx_core_tshashtable_recycle(&(context->hashTable));
    

    if(NULL != head)
        yx_allocator_free(allocator, head);
    
    
    if(NULL != ctrl)
        yx_allocator_free(allocator, ctrl);
    
    
    if(SEM_FAILED != msgSem)
        sem_close(msgSem);
//        sem_destroy(msgSem);
    
    
    if(NULL != context)
        yx_allocator_free(allocator, context);
    
    
    return NULL;
    

}

void shutdownServer(HSLSelectServer* handleRef, yx_bool tryToWait){
    
    if(NULL == handleRef || NULL == *handleRef)
        return;
    
    _SL_select_server_context_ref context = SLSERVERHANDLE2IMPL(*handleRef);
    *handleRef = NULL;

    _exitServer(context);
    double exittimes = 0;
    int semFlag = 0;
    while (yx_true == tryToWait  &&  0 != (semFlag = sem_trywait(context->msgSem)) && exittimes<1000 ){
        sleep(0);
        exittimes += 1;
    }
    
    if(semFlag != 0)
        pthread_cancel(context->msgThreadID);
    
    pthread_join(context->msgThreadID, NULL);
    
    
    if(NULL != context->threadPool)
        yx_core_theadpool_destroy(&(context->threadPool), yx_true==tryToWait? 1 : 0);
    
    
    yx_core_tshashtable_recycle(&(context->hashTable));
    
    
    if(NULL != context->encryption)
        yx_core_encrypt_randomdict_destroy(&(context->encryption));
    
    
    SocketNodeRef p = context->head;
    while (NULL != p) {
        if(p->socket > 0)
            close(p->socket);
        p = p->pNext;
    }
    
    deleteSocketList(context->allocator, &(context->head));
    
    sem_close(context->msgSem);
    
    yx_allocator_free(context->allocator, context);
    

    
    printf("server shutdown!\n");
}


int sendMsg(HSLSelectServer handle, int connectionID, void* buf, size_t size, size_t *bytesWritten)
{
    
    if(NULL == handle)
        return -1;
    
    if(NULL == buf)
        return -1;
    
    if(size <= 0)
        return -1;
    
    _SL_select_server_context_ref context = SLSERVERHANDLE2IMPL(handle);
    
    
    int rtVal = -1;
    JobRecordRef jobRecord;
    if((jobRecord = (JobRecordRef)yx_core_tshashtable_get(&(context->hashTable), connectionID, (yx_value)NULL)) != NULL)
    {
        RemoteMsgRef msg = NULL;
        if(NULL != (msg = (RemoteMsgRef)yx_allocator_alloc(context->allocator, sizeof(RemoteMsg) + size)))
        {
            msg->nLen = htonl((int)size);
            strcpy(msg->szCmd, SF_RemoteMsg_data);
            memcpy(msg->szData, buf, size);
            
            
            yx_core_encrypt_randomdict_encrypt(context->encryption, (unsigned char*)(msg->szData), (unsigned char*)(msg->szData), size);
            yx_core_encrypt_randomdict_encrypt(context->encryption, (unsigned char*)msg, (unsigned char*)msg, sizeof(RemoteMsg));
            
            pthread_mutex_lock(&(jobRecord->mutex));
            rtVal = _ISF_Write(connectionID, (void*)msg, sizeof(RemoteMsg)+size, bytesWritten);
            pthread_mutex_unlock(&(jobRecord->mutex));
            
            
            if(0 != rtVal)
            {
                printf("error(%d)=%s\n", rtVal,strerror(rtVal));
            }
            
            
            yx_allocator_free(context->allocator, msg);
        }
    }
    
    return rtVal;
}




int _exitServer(_SL_select_server_context_ref context){
    
    //close(context->fdCtrl[1]);
    ControlMsg msg;
    strcpy(msg.szCmd, SF_PipeMsg_Exit);
    _ISF_Write(context->fdCtrl[1], &msg, sizeof(msg), NULL);
    
    return 0;
}




int _initIPV4TCPStreamServerAddress(struct sockaddr_in* addr, int port){
    memset((void*)addr, 0, sizeof(struct sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = htonl(INADDR_LOOPBACK);//INADDR_LOOPBACK//innet_addr("127.0.0.1");//INADDR_ANY;//inet_addr("127.0.0.1");//INADDR_ANY;// inet_addr("0.0.0.0");
	addr->sin_port = htons(port);
    
    return 0;
}

int _initServer(int type, const struct sockaddr *addr, socklen_t sockLen, int maxQueueLen){
    int fd;
	int err = 0, iSockAttrOn = 1;
	

	if ((fd = socket(addr->sa_family, type, 0)) < 0){
		return -1;
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &iSockAttrOn, sizeof(iSockAttrOn) ) < 0){
		err = errno;
		goto errout;
	}

	if (bind(fd, addr, sockLen) < 0){
		err = errno;
		goto errout;
	}

	if (SOCK_STREAM == type || SOCK_SEQPACKET == type){
		if (listen(fd, maxQueueLen) < 0) {
			err = errno;
			goto errout;
		}
	}
	return fd;
errout:
	close(fd);
	errno = err;
	return -1;
}

int _beginServerSelect(_SL_select_server_context_ref context, fd_set* fdset){
    
    SocketNodeRef node;
    int ret;
    

    FD_ZERO(fdset);
    node = context->head;
    while(NULL != node){
        FD_SET(node->socket, fdset);
        node = node->pNext;
    }
    

    ret = select(maxSocketNumFromList(context->head) + 1, fdset, NULL, NULL, NULL);
    
    if(ret < 0){//select error
        printf("select err=%s!\n", strerror(errno));
        while (context->head != NULL){
            node = context->head;
            context->head = context->head->pNext;
            close(node->socket);
            yx_allocator_free(context->allocator, node);
        }
        
        return -1;
    }
    else if(0 == ret){

    }
    
    return 0;
}


int _handleMsg(_SL_select_server_context_ref context, SocketNodeRef node, RemoteMsgRef msgRef){
    //:~ COMMENT OUT
    char szBuf[remote_call_max_buff_len];
    memset(szBuf, 0, remote_call_max_buff_len);
    RemoteMsgRef backMsgRef = NULL;

    
    if(strcmp(msgRef->szCmd, SF_RemoteMsg_wantExit) == 0){
        backMsgRef = (RemoteMsgRef)yx_allocator_alloc(context->allocator, sizeof(RemoteMsg));
        backMsgRef->nLen = htonl(0);
        strcpy(backMsgRef->szCmd, SF_RemoteMsg_bye);
    }
    else if(strcmp(msgRef->szCmd, SF_RemoteMsg_data) == 0){
        
        JobMsgRef jobMsg = NULL;
        if(NULL != (jobMsg = (JobMsgRef)yx_allocator_alloc(context->allocator, sizeof(JobMsg))))
        {
            jobMsg->buf = NULL;
            jobMsg->nLen = msgRef->nLen;
            jobMsg->connectionID = node->socket;
            jobMsg->hashtable_ref = NULL;
            jobMsg->callback_msgHandleFun = context->callback_msgHandleFun;
            jobMsg->callback_newConFun = NULL;
            jobMsg->callback_conClosedFun = NULL;
            jobMsg->allocator = context->allocator;
            if(jobMsg->nLen > 0){
                
                if(NULL != (jobMsg->buf = yx_allocator_alloc(context->allocator, jobMsg->nLen))){
                    memcpy(jobMsg->buf, msgRef->szData, jobMsg->nLen);
                    
#ifdef DEBUG
                    int ret = 0;
                    if(-1 == (ret = yx_core_thread_addTask(context->threadPool, _job_msg, (void*)jobMsg, 0))){
                        printf("an error had occurred while adding a task\n");
                    }
                    else if(-2 == ret){
                        // you know..
                        YX_ASSERT(0);
                    }
#else
                    /*
                     Note :disable the mempool behavior monitoring in release version
                     When you are not the correct use of the remote-module,
                     it may cause low performance or have a terrible accident.
                     
                     So,
                     
                     ---You should made a sufficient testing in debug Version.
                     
                     what's more
                     
                     ---A flexible thread is need.
                     
                     */
                    if(-1 == yx_core_thread_addTask(context->threadPool, _job_msg, (void*)jobMsg, 1)){
                        printf("an error had occurred while adding a task\n");
                    }
#endif
                }
                else{
                    printf("failed to alloc the message\n");
                }
            }
        }
        
    }
    else{
        printf("unknow msg");
    }
    
    
    
    if(NULL != backMsgRef){
        
        int len = backMsgRef->nLen;
        backMsgRef->nLen = htonl(backMsgRef->nLen);
        
        yx_core_encrypt_randomdict_encrypt(context->encryption, (unsigned char*)(backMsgRef->szData), (unsigned char*)(backMsgRef->szData), len);
        yx_core_encrypt_randomdict_encrypt(context->encryption, (unsigned char*)backMsgRef, (unsigned char*)backMsgRef, sizeof(RemoteMsg));
        
        memset(szBuf, 0, remote_call_max_buff_len);
        memcpy(szBuf, backMsgRef, sizeof(RemoteMsg) + len);
        
        JobRecordRef jobRecord;
        if((jobRecord = (JobRecordRef)yx_core_tshashtable_get(&(context->hashTable), node->socket, (yx_value)NULL)) != NULL)
        {
            pthread_mutex_lock(&(jobRecord->mutex));
            _ISF_Write(node->socket, szBuf, sizeof(RemoteMsg) + len, 0);
            pthread_mutex_unlock(&(jobRecord->mutex));
        }
        
        yx_allocator_free(context->allocator, backMsgRef);
    }
    
    return 0;
}



int _handleNewClient(_SL_select_server_context_ref context, SocketNodeRef node){
    socklen_t sockLen = sizeof(struct sockaddr);
    struct sockaddr_in cliAddr;
    int cliFd;
    
    
    memset((void *)&cliAddr, 0 , sockLen);
    cliFd = accept(node->socket, (struct sockaddr*)&cliAddr, &sockLen);
    if (cliFd < 0){
        printf("accept err=%s!\n", strerror(errno));
        while (context->head != NULL){
            node = context->head;
            context->head = context->head->pNext;
            close(node->socket);
            yx_allocator_free(context->allocator, node);
        }
        return -1;
    }
    
    int iSockAttrOn = 1;
    if (setsockopt(cliFd, SOL_SOCKET, SO_NOSIGPIPE, &iSockAttrOn, sizeof(iSockAttrOn) ) < 0)
        printf("can not set the SIGPIPE sign\n");
        
        
    printf("Client connect:ip=%s, port=%d \n", inet_ntoa(cliAddr.sin_addr),
           ntohs(cliAddr.sin_port));
    addNodeToListEnd(context->head, createSocketNode(context->allocator, cliFd, &cliAddr));
    
    
    JobMsgRef jobMsg = (JobMsgRef)yx_allocator_alloc(context->allocator, sizeof(JobMsg));
    jobMsg->buf = NULL;
    jobMsg->nLen = -1;
    jobMsg->connectionID = cliFd;
    jobMsg->hashtable_ref = &(context->hashTable);
    jobMsg->callback_msgHandleFun = NULL;
    jobMsg->callback_newConFun = context->callback_newConFun;
    jobMsg->callback_conClosedFun = NULL;
    strncpy(jobMsg->ipAddress, inet_ntoa(cliAddr.sin_addr), 13);
    jobMsg->port = ntohs(cliAddr.sin_port);
    jobMsg->allocator = context->allocator;
    
#ifdef SL_DEBUG
    int ret = 0;
    if(-1 == (ret = yx_core_thread_addTask(context->threadPool, _job_new, (void*)jobMsg, 0))){
        printf("an error had occurred while adding a task");
    }
    
    if(-2 == ret){
        printf("Waiting for a thread! (If this information frequently appear, you need to increment the thread in thread pool)");
        if(-1 == yx_core_thread_addTask(context->threadPool, _job_new, (void*)jobMsg, 1)){
            printf("an error had occurred while adding a task");
        }
    }
#else
    if(-1 == yx_core_thread_addTask(context->threadPool, _job_new, (void*)jobMsg, 1)){
        printf("an error had occurred while adding a task");
    }
#endif
    
    return 0;
}


int _handleClientErr(_SL_select_server_context_ref context, SocketNodeRef node){
    printf("recv Client err=%s, ip=%s, port=%d!\n", strerror(errno),
           inet_ntoa(node->addr.sin_addr), ntohs(node->addr.sin_port));
    close(node->socket);
    context->head =  deleteNodeBySocketID(context->allocator, context->head, node->socket);
    
    return 0;
}

int _handleClientClosed(_SL_select_server_context_ref context, SocketNodeRef node){
    
    JobMsgRef jobMsg = (JobMsgRef)yx_allocator_alloc(context->allocator, sizeof(JobMsg));
    jobMsg->buf = NULL;
    jobMsg->nLen = -1;
    jobMsg->connectionID = node->socket;
    jobMsg->hashtable_ref = &(context->hashTable);
    jobMsg->callback_msgHandleFun = NULL;
    jobMsg->callback_newConFun = NULL;
    jobMsg->callback_conClosedFun = context->callback_conClosedFun;
    strncpy(jobMsg->ipAddress, inet_ntoa(node->addr.sin_addr), 13);
    jobMsg->port = ntohs(node->addr.sin_port);
    jobMsg->allocator = context->allocator;
    
#ifdef SL_DEBUG
    int ret = 0;
    if(-1 == (ret = yx_core_thread_addTask(context->threadPool, _job_closed, (void*)jobMsg, 0))){
        printf("an error had occurred while adding a task");
    }
    
    if(-2 == ret){
        printf("Waiting for a thread! (If this information frequently appear, you need to increment the thread num in thread pool)");
        if(-1 == yx_core_thread_addTask(context->threadPool, _job_closed, (void*)jobMsg, 1)){
            printf("an error had occurred while adding a task");
        }
    }
#else
    if(-1 == yx_core_thread_addTask(context->threadPool, _job_closed, (void*)jobMsg, 1)){
        printf("an error had occurred while adding a task");
    }
#endif
    
    
    close(node->socket);
    context->head =  deleteNodeBySocketID(context->allocator, context->head, node->socket);
    
    return 0;
}

static int _SLF_MagageClientMsg(_SL_select_server_context_ref context){
    
    
    yx_bool fRunningFlag = yx_true; 
    char szBuf[remote_call_max_buff_len];
    memset(szBuf, 0, remote_call_max_buff_len);
    
    
    sem_wait(context->msgSem);
    
    
    
    fd_set rdset;
    SocketNodeRef node = NULL;
    int ret;
    while (fRunningFlag == yx_true) {
        
        _beginServerSelect(context, &rdset);
        
        for(node=context->head; node!=NULL; node=node->pNext){
            if(FD_ISSET(node->socket, &rdset) == 0)
                continue;
            
            if(node == context->head){ 
                if(_handleNewClient(context, node) != 0)
                    return -1;
            }
            else if(node == context->ctrl){ 
                memset(szBuf, 0, remote_call_max_buff_len);
                ret = _ISF_Read(context->fdCtrl[0], szBuf, sizeof(ControlMsg), NULL);
                ControlMsgRef msgRecv = (ControlMsgRef)szBuf; 
                    
                /*handle the control msg*/
                if(0!=ret || strcmp(msgRecv->szCmd, SF_PipeMsg_Exit) == 0){ 
                    fRunningFlag = yx_false;
                    break;
                }
            }
            else{
                memset(szBuf, 0, remote_call_max_buff_len);
                ret = _ISF_Read(node->socket, szBuf, sizeof(RemoteMsg), 0);
                yx_core_encrypt_randomdict_decrypt(context->encryption, (unsigned char*)szBuf, (unsigned char*)szBuf, sizeof(RemoteMsg));
                if(ret == 0){
                    RemoteMsgRef msgRecv = (RemoteMsgRef)szBuf;
                    msgRecv->nLen = ntohl(msgRecv->nLen);
                    if( msgRecv->nLen > 0  &&  msgRecv->nLen < (remote_call_max_buff_len - sizeof(RemoteMsg)) ){
                        ret = _ISF_Read(node->socket, szBuf+sizeof(RemoteMsg), msgRecv->nLen, 0);
                        yx_core_encrypt_randomdict_decrypt(context->encryption, (unsigned char*)(szBuf+sizeof(RemoteMsg)), (unsigned char*)(szBuf+sizeof(RemoteMsg)), msgRecv->nLen);
                    }
                    else{
                        ret = -1;
                    }
                }

                if(ret != 0)
                {
                    
                    SocketNodeRef badNode = node;
                    node = node->pNext;
                    _handleClientClosed(context, badNode);
                    if(NULL == node)
                        break;
                }
                else
                {
                    
                    RemoteMsgRef msgRecv = (RemoteMsgRef)szBuf;
                    msgRecv->nLen = msgRecv->nLen;
                    
                    _handleMsg(context, node, msgRecv);
                    
                    
                } //if(ret < 0){ //client error
            } //if(node == head)
        } //for(node=head; node!=NULL; node=node->pNext){
    } //while (1) {
    

    sem_post(context->msgSem);
    
    return 0;
}




SocketNodeRef createSocketNode(yx_allocator allocator, int socket, struct sockaddr_in *pAddr){
    
    SocketNodeRef p = NULL;
    
	if ((p = (SocketNodeRef)yx_allocator_alloc(allocator, sizeof(SocketNode))) != NULL){
		p->socket = socket;
        
        /*select can use for all kinds of the file id, so my socket node must not only support the socket file id*/
        if(NULL != pAddr)
            memcpy(&(p->addr), pAddr, sizeof(struct sockaddr_in));
		p->pNext = NULL;
	}
	return p;
}

void deleteSocketList(yx_allocator allocator, SocketNodeRef* headRef){
    SocketNodeRef p;
    while (*headRef != NULL) {
        p = *headRef;
        *headRef = (*headRef)->pNext;
        
        yx_allocator_free(allocator, p);
    }
}


void addNodeToListEnd(SocketNodeRef head, SocketNodeRef node){
    SocketNodeRef p = head;
	while (p->pNext != NULL){
		p = p->pNext;
	}
	p->pNext = node;
}


SocketNodeRef deleteNodeBySocketID(yx_allocator allocator, SocketNodeRef head, int socket){
    
    SocketNodeRef p = head;
    SocketNodeRef pPrevious = p;
	while (p != NULL){
		if (p->socket == socket){
			if (p != pPrevious){
				pPrevious->pNext = p->pNext;
			}else{
				head = p->pNext;
			}
            
            yx_allocator_free(allocator, p);
			break;
		}
		pPrevious = p;
		p = p->pNext;
	}
    
    
	return head;
}


SocketNodeRef nodeFromSocketID(SocketNodeRef head, int socket){
    SocketNodeRef p = head;
	while (p != NULL){
		if (p->socket == socket){
			return p;
		}
		p = p->pNext;
	}
	return NULL;
}


int maxSocketNumFromList(SocketNodeRef head){
    SocketNodeRef p = head;
	int maxsock = -1;
	while (p != NULL){
		maxsock = maxsock > p->socket ? maxsock : p->socket;
		p = p->pNext;
	}
	return maxsock;
}




static void _job_msg(void* msgPtr){
    JobMsgRef jobMsg = (JobMsgRef)msgPtr;
    
    if(NULL == jobMsg)
        return;
    
    
    jobMsg->callback_msgHandleFun(jobMsg->connectionID, jobMsg->buf, jobMsg->nLen);
    
    if(NULL != jobMsg->buf)
        yx_allocator_free(jobMsg->allocator, jobMsg->buf);
    
    if(NULL != jobMsg)
        yx_allocator_free(jobMsg->allocator, jobMsg);
}


static void _job_new(void* msgPtr){
    JobMsgRef jobMsg = (JobMsgRef)msgPtr;
    
    if(NULL == jobMsg)
        return;
    
    
    
    JobRecordRef jobRecord = (JobRecordRef)yx_allocator_alloc(jobMsg->allocator, sizeof(JobRecord));
    pthread_mutex_init(&(jobRecord->mutex), NULL);
    jobRecord->connectionID = jobMsg->connectionID;
    yx_core_tshashtable_set(jobMsg->hashtable_ref, jobMsg->connectionID, (yx_value)(jobRecord));
    
    
    jobMsg->callback_newConFun(jobMsg->connectionID, jobMsg->ipAddress, jobMsg->port);
    
    
    if(NULL != jobMsg->buf)
        yx_allocator_free(jobMsg->allocator, jobMsg->buf);

    if(NULL != jobMsg)
        yx_allocator_free(jobMsg->allocator, jobMsg);
}


static void _job_closed(void* msgPtr)
{
    JobMsgRef jobMsg = (JobMsgRef)msgPtr;
    
    if(NULL == jobMsg)
        return;
    
    JobRecordRef jobRecord = NULL;
    if((jobRecord = (JobRecordRef)yx_core_tshashtable_get(jobMsg->hashtable_ref, jobMsg->connectionID, (yx_value)(&jobRecord))) != NULL)
    {
        yx_core_tshashtable_remove(jobMsg->hashtable_ref, jobMsg->connectionID);
        
        //destroy the mutex
        while (EBUSY == pthread_mutex_destroy(&(jobRecord->mutex)));
        
        yx_allocator_free(jobMsg->allocator, jobRecord);
        
        jobMsg->callback_conClosedFun(jobMsg->connectionID, jobMsg->ipAddress, jobMsg->port);
    }
    else
    {
        printf("can not find a socket ID in hash table when a client closed");
    }
    
    
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




