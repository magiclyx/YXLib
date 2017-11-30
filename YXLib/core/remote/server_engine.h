//
//  RemoteCallEngine.h
//  TestSelectServer
//
//  Created by Yuxi Liu on 10/16/12.
//  Copyright (c) 2012 Yuxi Liu. All rights reserved.
//

//:~ TODO Need to reconstruct

#ifndef TestSelectServer_RemoteCallEngine_h
#define TestSelectServer_RemoteCallEngine_h

#include "../yx_core_global.h"


#ifdef __cplusplus
extern "C"{
#endif

    
/*
 remove all repetitive comment in the implemention file.
 
 Because I want to refactoring this module in future.
 */
    


#ifdef DEBUG
#define SL_DEBUG 1
#endif

typedef void* HSLSelectServer;
typedef void* HSLSocket;

typedef void (*HSLServerNewConnectionHandleFun)(int connectionID, const char* ipAddress, int port);
typedef void(*HSLServerMsgHandleFun)(int connectionID, const void* buf, int size);
typedef void (*HSLServerConnectionClosedHandleFun)(int connectionID, const char* ipAddress, int port);


HSLSelectServer setupServer(yx_allocator allocator, int port, HSLServerMsgHandleFun callback_msgHandle, HSLServerNewConnectionHandleFun calback_newConnection, HSLServerConnectionClosedHandleFun callback_connectionClosed);
void shutdownServer(HSLSelectServer* handleRef, yx_bool tryToWait);

//0 means success
//EPIPE means the socket closed
//other value means other error. :)
int sendMsg(HSLSelectServer handle, int connectionID, void* buf, size_t size, size_t *bytesWritten);

#ifdef __cplusplus
}
#endif

    
#endif
