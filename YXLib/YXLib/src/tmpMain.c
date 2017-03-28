//
//  tmpMain.c
//  YXLib
//
//  Created by LiuYuxi on 15/8/6.
//  Copyright (c) 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_headers.h"
#include "unittest/yx_unittest_headers.h"
#include "unittest/yx_unittest_unittest.h"

#include "testcase/core/yx_testcase_core_headers.h"


#include "yx_macosx_event.h"

#include "yx_core_headers.h"

#include "yx_core_logger_utility.h"


yx_os_process process;
yx_os_process_ref process2;
int* a = NULL;
pid_t pid = 0;

typedef struct running_data{
    int counter;
    yx_os_eventdriver_ref driverRef;
    yx_bool running;
}running_data;

#define timer_id 0
#define usr_id 0



extern void event_calback(yx_os_event_info_ref event_info);

extern void testfork(yx_os_eventdriver_ref event);

extern void msg_loop(void);




int tmpmain(int argc, const char * argv[])
{
    
//    YX_DEBUGLOG(NULL, "cout %d\n", 2);
//    YX_DEBUGLOG(NULL, "cout %d\n");
    
    yx_core_setup(argc, argv);
    
    
    msg_loop();

    
    yx_core_shutdown();
    return 0;
}


void event_calback(yx_os_event_info_ref event_info)
{
    
    switch (event_info->type)
    {
        case yx_os_event_type_timer:
        {
            
            running_data* pdata =  (running_data*)event_info->user_data;
            yx_os_printf("timer %d \n", pdata->counter);
            
            pdata->counter++;
            
            if (10 == pdata->counter)
            {
                yx_os_eventdriver_send_custom(pdata->driverRef, usr_id, (yx_value)pdata);
            }
        }
            break;
        case yx_os_event_type_user:
        {
            yx_os_printf("user\n");
            fflush(stdout);
            running_data* pdata =  (running_data*)event_info->user_data;
            pdata->running = yx_false;
        }
            break;
        case yx_os_event_type_ioread:
            yx_os_printf("ioread\n");
            fflush(stdout);
            break;
        case yx_os_event_type_iowrite:
            yx_os_printf("iowrite\n");
            fflush(stdout);
            break;
        case yx_os_event_type_process:
            yx_os_printf("process exit notification\n");
            fflush(stdout);
            
            yx_os_process_ref process_r = (yx_os_process_ref)(event_info->user_data);
            printf("pid = %d\n", process_r->process_id);
            yx_os_process_closeHandleIfExit(process_r);

            switch (event_info->subtype) {
                case NOTE_EXIT:
                    break;
                    
                default:
                    break;
            }

            
            break;
            
        default:
            yx_os_printf("uknown\n");
            break;
    }
    
}


void msg_loop()
{
    yx_os_eventdirver driver;
    yx_os_eventdriver_init(yx_stdC_allocator(), &driver, event_calback, 0);
    
    
    running_data data;
    data.counter = 0;
    data.driverRef = &driver;
    data.running = yx_true;
    
    
    testfork(&driver);
    
    
    yx_os_eventdriver_add_timer(&driver, timer_id, 1.0f, 1, yx_true, (yx_value)(&data));
    
    yx_os_eventdriver_add_custom(&driver, usr_id);
    
    
    
    do{
        yx_bool success = yx_os_eventdriver_process(&driver, NULL);
        
        if (yx_false == success)
        {
            yx_os_printf("error\n");
        }
        
    }while (yx_true == data.running);
    
    
    yx_os_eventdriver_recycle(&driver);
}


void testfork(yx_os_eventdriver_ref event)
{
    
    
    yx_os_process_current_init(&process);
    
    yx_bool isChild = yx_false;
    
    
    
    if (yx_unlikely( -1 == yx_os_process_fork(&process, /*"testProcess"*/NULL, &isChild) ))
    {
        yx_os_printf("failed to create process");
    }
    
    
    if (isChild)
    {
        
        yx_os_printf("child process did running\n");
        fflush(stdout);
        
        yx_os_thread_sleep(3);
        
        
        yx_os_printf("child process will shutdown\n");
        fflush(stdout);

        
        yx_os_process_exit_normal();
    }
    else
    {
        yx_os_printf("child process id = %d\n", process.process_id);
        yx_os_eventdriver_add_process(event, yx_os_process_handle(&process), (yx_value)(&process));
    }

}







