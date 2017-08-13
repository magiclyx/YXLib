//
//  main.m
//  test
//
//  Created by Yuxi Liu on 2017/8/12.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "yx_macosx_spinlock_raw.h"
#import "yx_macosx_spinlock.h"
#import <pthread.h>
#import "yx_macosx_atomic.h"

typedef union ticketlock2 ticketlock2;


int main(int argc, const char * argv[]) {
    
    
    @autoreleasepool {
        
        
        NSMutableArray* arr = [[NSMutableArray alloc] initWithCapacity:1000];
    
        dispatch_semaphore_t _semaphore;
        _semaphore = dispatch_semaphore_create(0);
        
        __block pthread_mutex_t _mutex;
        pthread_mutex_init(&_mutex, NULL);

        
        __block yx_os_spinlock spinlock = YX_OS_SPINLOCK_INIT;
        
        __block volatile int value = 0;
        
        void (^add)() = ^{
            
            dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);
            [NSThread sleepForTimeInterval:1];
            
            for(int i=0; i<1000; i++){
                //pthread_mutex_lock(&_mutex);
                yx_os_spin_lock(&spinlock);
                [arr addObject:@(value++)];
                //NSLog(@"%d", value++);
                yx_os_spin_unlock(&spinlock);
                //pthread_mutex_unlock(&_mutex);
            }
        };
        
        
        dispatch_queue_t queue = dispatch_queue_create("com.xxx.safequeue", DISPATCH_QUEUE_CONCURRENT);
        for (int i=0; i<1; i++) {
            dispatch_async(queue, add);
        }
        
        for (int i=0; i<1; i++) {
            dispatch_semaphore_signal(_semaphore);
        }
        
        NSDate* start = [NSDate date];
        dispatch_barrier_sync(queue, ^{});
        
        NSDate* end = [NSDate date];
        
        NSTimeInterval time = [end timeIntervalSinceDate:start];
        NSLog(@"%lf", time);
    
        
        pthread_mutex_destroy(&_mutex);

//        NSLog(@"%@", arr);
    }

    
    return 0;
}
