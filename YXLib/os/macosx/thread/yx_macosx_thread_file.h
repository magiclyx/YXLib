//
//  yx_macosx_thread_file.h
//  YXCLib
//
//  Created by Yuxi Liu on 2017/12/3.
//  Copyright © 2017年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_MACOSX_THREAD_FILE_H_
#define _YX_MACOSX_THREAD_FILE_H_

/*注意，这里只是线程安全锁，并不是进程安全锁*/
/*注意，这里是CFile, 也就是FILE* fp */

//锁可以递归调用
#define yx_os_thread_cfile_lock(fp)    flockfile(fp)
#define yx_os_thread_cfile_unlock(fp)  funlockfile(fp)
#define yx_os_thread_cfile_trylock(fo) ftrylockfile(fp)


/*这四个，不进行封装。 忽略当前锁，读或写一个字符*/
//int	 getc_unlocked(FILE *);
//int	 getchar_unlocked(void);
//int	 putc_unlocked(int, FILE *);
//int	 putchar_unlocked(int);

#endif /* _YX_MACOSX_THREAD_FILE_H_ */
