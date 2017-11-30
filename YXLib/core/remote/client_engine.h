//
//  RemoteCallEngine.h
//  TestSelectClient
//
//  Created by Yuxi Liu on 10/17/12.
//  Copyright (c) 2012 Yuxi Liu. All rights reserved.
//

#ifndef TestSelectClient_RemoteCallEngine_h
#define TestSelectClient_RemoteCallEngine_h

//#include "../../cm/cmBasicTypes.h"
#include "../yx_core_global.h"


/*
 remove all repetitive comment in the implemention file.
 
 Because I want to refactoring this module in future.
 */


#ifdef __cplusplus
extern "C"{
#endif



#ifdef DEBUG
#define SL_DEBUG 1
#endif


typedef void* HSLSelectClient;


typedef void(*HSLClientMsgHandleFun)(const void* buf, int size);
typedef void(*HSLClientConnectionClosedHandleFun)();

HSLSelectClient SLFSetupClient(yx_allocator allocator, const char* address, int port, HSLClientMsgHandleFun callback_msgHandle, HSLClientConnectionClosedHandleFun callback_connectionClosed);
void SLFShutDownClient(HSLSelectClient* handRef, yx_bool tryToWait);
int SLFSendData(HSLSelectClient handle, void* buf, size_t size);

#ifdef __cplusplus
}
#endif

    
    
#endif
