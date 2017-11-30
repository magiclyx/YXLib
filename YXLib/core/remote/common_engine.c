//
//  2.m
//  clilib
//
//  Created by Yuxi Liu on 1/6/13.
//
//


#include <stdio.h>
#include <assert.h>

#include <unistd.h>
#include <errno.h>




#import "common_engine.h"


//:~ TODO Split the packet
//:~ TODO using yx_pool here, for a elastic memory
const int remote_call_max_buff_len = 1024*100;
//

const int remote_call_max_thread_in_pool = 10; 
const int remote_call_max_queue_len = 10;
const int remote_call_max_hashTbl_len = 100; 


const char* SF_RemoteMsg_wantExit = "WANT_EXIT";
const char* SF_RemoteMsg_bye = "BYE";
const char* SF_RemoteMsg_data = "DATA";
const char* SF_RemoteMsg_largeData = "LARGE_DATA";

const char* SF_PipeMsg_Exit = "EXIT";





int _ISF_Write(int fd, const void *buf, size_t bufSize, size_t *bytesWritten){
    int 	err;
	char *	cursor;
	size_t	bytesLeft;
	ssize_t bytesThisTime;
	
	
#if SL_DEBUG
    {
        int					junk;
        struct stat			sb;
        struct sigaction	currentSignalState;
        int					val;
        socklen_t			valLen;
        
        junk = fstat(fd, &sb);
        //ssert(junk == 0);
        
        if ( S_ISFIFO(sb.st_mode) || S_ISSOCK(sb.st_mode) ) {
            junk = sigaction(SIGPIPE, NULL, &currentSignalState);
            assert(junk == 0);
            
            valLen = sizeof(val);
            junk = getsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &val, &valLen);
            assert(junk == 0);
            assert(valLen == sizeof(val));
            
            
            assert( (currentSignalState.sa_handler == SIG_IGN) || (val == 1) );
        }
    }
#endif //SL_DEBUG
    
    
    
	err = 0;
	bytesLeft = bufSize;
	cursor = (char *) buf;
	while ( (err == 0) && (bytesLeft != 0) ) {
		bytesThisTime = write(fd, cursor, bytesLeft);
		if (bytesThisTime > 0) {
			cursor    += bytesThisTime;
			bytesLeft -= bytesThisTime;
		} else if (bytesThisTime == 0) {
			assert(0);
			err = EPIPE;
		} else {
			assert(bytesThisTime == -1);
			
			err = errno;
			assert(err != 0);
			if (err == EINTR) {
				err = 0;		// let's loop again
			}
		}
	}
	if (bytesWritten != NULL) {
		*bytesWritten = bufSize - bytesLeft;
	}
    
	
	return err;
}



int _ISF_Read(int fd, void *buf, size_t bufSize, size_t *bytesRead)
{
	int 	err;
	char *	cursor;
	size_t	bytesLeft;
	ssize_t bytesThisTime;
    
    // Pre-conditions
    
	assert(fd >= 0);
	assert(buf != NULL);
    // bufSize may be 0
    // bytesRead may be NULL
	
    // char* charBuff = malloc(bufSize*sizeof(char));
    
	err = 0;
	bytesLeft = bufSize;
	cursor = (char *) buf;
	while ( (err == 0) && (bytesLeft != 0) ) {
		bytesThisTime = read(fd, cursor, bytesLeft);
		if (bytesThisTime > 0) {
			cursor    += bytesThisTime;
			bytesLeft -= bytesThisTime;
		} else if (bytesThisTime == 0) {
			err = EPIPE;
		} else {
			assert(bytesThisTime == -1);
			
			err = errno;
			assert(err != 0);
			if (err == EINTR) {
				err = 0;		// let's loop again
			}
		}
	}
	if (bytesRead != NULL) {
		*bytesRead = bufSize - bytesLeft;
	}
    
	
	return err;
}