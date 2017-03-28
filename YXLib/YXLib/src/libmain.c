//
//  libmain.c
//  YXLib
//
//  Created by LiuYuxi on 15/10/10.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#include "yx_core_headers.h"
#include "yx_core_hash_murmurHash.h"
#include "yx_core_hash_cityhash.h"
#include "yx_core_hash_spookyHash.h"
#include "yx_core_hash.h"

#include "yx_macosx_event.h"


#include "yx_core_event_timmer.h"

//int main(int argc, const char * argv[])
//{
//    yx_core_setup(argc, argv);
//    
//    yx_core_eventloop mainloop;
//    
//    yx_core_eventloop_init(yx_get_persistent_allocator(), &mainloop);
//    
//    
//    yx_core_eventloop_run(&mainloop);
//
//    
//    yx_core_shutdown();
//    
//    return 0;
//}


//void calback(yx_os_event_info_ref event_info)
//{
//    printf("abcdefg\n");
//}

void callback(yx_core_event_timer_ref event, yx_long ident, yx_value data)
{
    printf("abcdefg\n");
}



int main(int argc, const char * argv[])
{
    
    yx_core_setup(argc, argv);
    
    
    yx_core_event_timer timer_event;
    yx_core_event_timer_init(&timer_event,
                             yx_get_allocator(),
                             0.5,
                             1,
                             yx_core_event_timer_level_background,
                             yx_true,
                             0,
                             callback);
    
    yx_core_event_timer_start(&timer_event);
    
    
    
    yx_core_eventloop_globalrun();
    
    
    yx_core_shutdown();
    
    
    
    return 0;
}








//
//
//int main()
//{
//    yx_os_tick start;
//    yx_os_tick end;
//    yx_real timeUsed = 0.0f;
//    
//    yx_uint64 val;
//    
//    
//    const char* a = "moioiojhvfeokok";
//    size_t len = strlen(a);
//
//    
//    
//    //yx_core_murmurhash_default_seed
//    
//    start = yx_os_ticket_now();
//    for (int i=0; i < 100000; i++)
//    {
////        val = rawhash(a, yx_core_murmurhash_default_seed);
//    }
//    end = yx_os_ticket_now();
//    
//    timeUsed = yx_os_ticket_toNanoSecond(end - start);
//    
//    printf("time = %lf\n", timeUsed);
//    
//    
//    
//    
//    
//    start = yx_os_ticket_now();
//    for (int i=0; i < 100000; i++)
//    {
////        val = MurmurHash64A(a, strlen(a), yx_core_murmurhash_default_seed);
//        val = yx_core_murmurhash64(a, len);
//    }
//    end = yx_os_ticket_now();
//    
//    timeUsed = yx_os_ticket_toNanoSecond(end - start);
//    
//    printf("time = %lf\n", timeUsed);
//    
//    
//    
//    
//    start = yx_os_ticket_now();
//    for (int i=0; i < 100000; i++)
//    {
//        val = yx_core_spookyhash64(a, len, 16);
////        SpookyHash::Hash64(a, len, 16);
//    }
//    end = yx_os_ticket_now();
//    
//    timeUsed = yx_os_ticket_toNanoSecond(end - start);
//    
//    printf("time = %lf\n", timeUsed);
//
//    
//    
//    
//    
//    start = yx_os_ticket_now();
//    for (int i=0; i < 100000; i++)
//    {
//        val = yx_core_cityhash64(a, len);
//    }
//    end = yx_os_ticket_now();
//    
//    timeUsed = yx_os_ticket_toNanoSecond(end - start);
//    
//    printf("time = %lf\n", timeUsed);
//
//    
//    
//    //0x21573BE98F273
//
////    
////    unsigned char str[4]={1,0,0,0};
////    unsigned int* i = (unsigned int*) str;
////    
////    if(*i == 1)
////        printf("Little_endian\n");
////    else if(*i == 1000000)
////        printf("Big_endian\n");
//    return 0;
//}
