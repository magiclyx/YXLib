//
//  yx_os_pipe.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/12.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_pipe.h"
#include "../file/yx_macosx_file_redirection.h"

yx_bool yx_os_pipe_init_noname(yx_os_pipe_ref p, yx_os_pipe_type type)
{
    assert(NULL != pipe);
    
    
    if (pipe(p->pfd) < 0)
    {
        p->pfd[0] = p->pfd[1] = -1;
        return yx_false;
    }
    
    p->type = type;
    
    
    switch (type) {
        case yx_os_pipe_readwrite:
            /*do nothing*/
            break;
        case yx_os_pipe_readonly:
            close(p->pfd[0]);
            p->pfd[0] = -1;
            break;
        case yx_os_pipe_writeonly:
            close(p->pfd[1]);
            p->pfd[1] = -1;
            break;
            
        default:
            assert(0);
    }
    
    
    return yx_true;
}

void yx_os_pipe_recycle(yx_os_pipe_ref p)
{
    assert(NULL != p);
    
    if (-1 != p->pfd[0])
        close(p->pfd[0]);
    
    if (-1 != p->pfd[1])
        close(p->pfd[1]);
}



yx_bool yx_os_pipe_bind2stdio(yx_os_pipe_ref p)
{
    switch (p->type)
    {
        case yx_os_pipe_readwrite:
            break;
        case yx_os_pipe_readonly:
            close(p->pfd[0]);
            p->pfd[0] = -1;
            break;
        case yx_os_pipe_writeonly:
            close(p->pfd[1]);
            p->pfd[1] = -1;
            break;
            
        default:
            assert(0);
    }
    
    
    if (-1 != p->pfd[0]  &&  yx_os_pipe_canwrite(p))
    {
        yx_os_file_redirect_stdin(p->pfd[0]);
    }
    
    if (-1 != p->pfd[1]  &&  yx_os_pipe_canread(p))
    {
        yx_os_file_redirect_stout(p->pfd[1]);
    }
    

    return yx_true;
}



yx_int yx_os_pipe_read(yx_os_pipe_ref p, yx_byte *buf, yx_size bufSize, yx_size *bytesRead)
{
    yx_int 	  err;
    yx_byte*  cursor;
    yx_size   bytesLeft;
    ssize_t   bytesThisTime;
    int fd;
    
    if (!yx_os_pipe_canread(p))
        return -1;
    
    fd = yx_os_pipe_readHandle(p);
    
    
    assert(fd >= 0);
    assert(0 != bufSize);
    assert(NULL != buf);
    
    
    err = 0;
    bytesLeft = bufSize;
    cursor = buf;
    while ( (err == 0) && (bytesLeft != 0) )
    {
        bytesThisTime = read(fd, cursor, bytesLeft);
        
        if (bytesThisTime > 0)
        {
            cursor    += bytesThisTime;
            bytesLeft -= (yx_size)bytesThisTime;
        }
        else if (bytesThisTime == 0)
        {
            err = EPIPE;
        }
        else
        {
            assert(bytesThisTime == -1);
            
            err = errno;
            assert(err != 0);
            
            if (err == EINTR)
            {
                err = 0;		// let's loop again
            }
        }
    }
    
    
    
    if (bytesRead != NULL)
        *bytesRead = bufSize - bytesLeft;
    
    
    return err;
}





yx_int yx_os_pipe_write(yx_os_pipe_ref p, yx_byte *buf, yx_size bufSize, yx_size *bytesWritten)
{
    yx_int 	  err;
    yx_byte*  cursor;
    yx_size   bytesLeft;
    ssize_t   bytesThisTime;
    int fd;
    
    
    if (!yx_os_pipe_canwrite(p))
        return -1;
    
    
    fd = yx_os_pipe_readHandle(p);
    
    
    assert(NULL != p);
    assert(fd >= 0);
    assert(0 != bufSize);
    assert(buf != NULL);
    
    
    
    // SIGPIPE occurs when you write to pipe or socket
    // whose other end has been closed.  The default action
    // for SIGPIPE is to terminate the process.  That's
    // probably not what you wanted.  So, in the debug build,
    // we check that you've set the signal action to SIG_IGN
    // (ignore).  Of course, you could be building a program
    // that needs SIGPIPE to work in some special way, in
    // which case you should define BAS_WRITE_CHECK_SIGPIPE
    // to 0 to bypass this check.
    
#if YX_DEBUG
    {
        int					junk;
        struct stat			sb;
        struct sigaction	currentSignalState;
        int					val;
        socklen_t			valLen;
        
        junk = fstat(fd, &sb);
        assert(junk == 0);
        
        if ( S_ISFIFO(sb.st_mode) || S_ISSOCK(sb.st_mode) )
        {
            junk = sigaction(SIGPIPE, NULL, &currentSignalState);
            assert(junk == 0);
            
            valLen = sizeof(val);
            junk = getsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &val, &valLen);
            assert(junk == 0);
            assert(valLen == sizeof(val));
            
            // If you hit this assertion, you need to either disable SIGPIPE in
            // your process or on the specific socket you're writing to.  The
            // standard code for the former is:
            //
            // (void) signal(SIGPIPE, SIG_IGN);
            //
            // You typically add this code to your main function.
            //
            // The standard code for the latter is:
            //
            // static const int kOne = 1;
            // err = setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &kOne, sizeof(kOne));
            //
            // You typically do this just after creating the socket.
            
            assert( (currentSignalState.sa_handler == SIG_IGN) || (val == 1) );
        }
    }
#endif //YX_DEBUG
    
    
    
    err = 0;
    bytesLeft = bufSize;
    cursor = buf;
    while ( (err == 0) && (bytesLeft != 0) )
    {
        bytesThisTime = write(fd, cursor, bytesLeft);
        if (bytesThisTime > 0)
        {
            cursor    += bytesThisTime;
            bytesLeft -= (yx_size)bytesThisTime;
            
        }
        else if (bytesThisTime == 0)
        {
            assert(0);
            err = EPIPE;
        }
        else
        {
            assert(bytesThisTime == -1);
            
            err = errno;
            assert(err != 0);
            
            
            if (err == EINTR)
            {
                err = 0;		// let's loop again
            }
        }
    }
    
    
    
    
    if (bytesWritten != NULL)
        *bytesWritten = bufSize - bytesLeft;
    
    
    
    
    return err;
}








































