//
//  common_engine.h
//  clilib
//
//  Created by Yuxi Liu on 1/6/13.
//
//

#ifndef clilib_common_engine_h
#define clilib_common_engine_h

//#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif
  
    
//:~ [v]
    
/*
 
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 !!!!!!!!!!!!!!!!!!warning !!!!!!!!!!!!!!!!!!!!!!
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 
 not implemented
 and
 there are 19 bugs in remote call modules!!!!!!!!(1 bug and 18 "todo")
 I've aready filed some of the the bug on the websit of "taskWorkShop-Yuxi"
 */


extern const int remote_call_max_buff_len;
extern const int remote_call_max_thread_in_pool;
extern const int remote_call_max_queue_len;
extern const int remote_call_max_hashTbl_len;


extern const char* SF_RemoteMsg_wantExit;
extern const char* SF_RemoteMsg_bye;
extern const char* SF_RemoteMsg_data;
extern const char* SF_RemoteMsg_largeData;

extern const char* SF_PipeMsg_Exit;



//:~ TODO merge all the ctl section to RemoteMsg from yxLib.
/*the current struction can not distribute the msg to subProcess*/
/*With the increase of fetures in 360Safe, there will be more and more the child process is started. This is not what I want*/
    
//socket message protocol
typedef struct _RemoteMsg{
    char szCmd[16];/* message command
                    
                    //-> socket command
                    WANT_EXIT -- want to exit
                    BYE  -- feedback of the "WANT_EXIT" msg
                    DATA -- data
                    LARGE_DATA
                    */
    
    
    int nLen; 
    char szData[0];
}RemoteMsg, *RemoteMsgRef;






int _ISF_Write(int fd, const void *buf, size_t bufSize, size_t *bytesWritten);
int _ISF_Read(int fd, void *buf, size_t bufSize, size_t *bytesRead);


#ifdef __cplusplus
}
#endif



#endif
