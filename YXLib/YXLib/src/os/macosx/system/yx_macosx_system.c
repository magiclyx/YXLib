//
//  yx_macosx_system.c
//  YXLib
//
//  Created by Yuxi Liu on 9/28/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#include "yx_macosx_system.h"

#include "../environment/yx_macosx_environment.h"
#include "../allocator/yx_macosx_allocator.h"


#pragma mark maxpathlen
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#define PATH_MAX_GUESS YX_OS_MAXPATH_LEN
#define SUSV3 200112L

static yx_long __yx_os_system_max_path = 0;

static yx_long __yx_os_system_guess_maxpathlen(void)
{
    if((__yx_os_system_max_path = pathconf(YX_OS_PATH_ROOT, _PC_PATH_MAX)) < 0)
    {
        __yx_os_system_max_path = PATH_MAX_GUESS;
        
        if (0 != yx_os_err_getlasterr()) {
            //:~ TODO log info
            printf("pathconf error for _PC_PATH_MAX");
        }
    }
    else
    {
        __yx_os_system_max_path++;
    }
    
    if(sysconf(_SC_VERSION) < SUSV3)
        __yx_os_system_max_path = __yx_os_system_max_path + 1;
    
    return __yx_os_system_max_path;
}


yx_long yx_os_system_maxpathlen(void) //get the real max path length
{
    if (0 == __yx_os_system_max_path)
    {
        __yx_os_system_guess_maxpathlen();
    }
    
    return __yx_os_system_max_path;
}


#pragma mark general
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
const yx_char* yx_os_system_machineInfo(void)
{
    static yx_char machine[15+1] = "";
    
    if (machine[0] == '\0')
    {
        yx_size len = 15;
        if (yx_unlikely( 0 != sysctlbyname("hw.machine", machine, &len, NULL, 0) ))
            return NULL;
        
        machine[15] = '\0';
    }
    
    
    return machine;
}


const yx_char* yx_os_system_modelInfo(void)
{
    static yx_char model[31+1] = "";
    
    if (model[0] == '\0')
    {
        yx_size len = 31;
        if (yx_unlikely( 0 != sysctlbyname("hw.model", model, &len, NULL, 0) ))
            return NULL;
        
        model[31] = '\0';
    }
    
    
    return model;

}

int yx_os_system_physicalcpuNum(void)
{
   /*加电时物理核心数*/
    
    static yx_int physicalcpu_num = -1;
    
    if (-1 == physicalcpu_num)
    {
        yx_size len = sizeof(physicalcpu_num);
        if (yx_unlikely( 0 != sysctlbyname("hw.physicalcpu", &physicalcpu_num, &len, NULL, 0) ))
            return -1;
    }
    
    return physicalcpu_num;
}


int yx_os_system_physicalcpuMaxNum(void)
{
    /*可引导最大物理核心数*/
    
    static yx_int physicalcpu_max_num = -1;
    
    if (-1 == physicalcpu_max_num)
    {
        yx_size len = sizeof(physicalcpu_max_num);
        if (yx_unlikely( 0 != sysctlbyname("hw.physicalcpu_max", &physicalcpu_max_num, &len, NULL, 0) ))
            return -1;
    }
    
    return physicalcpu_max_num;

}

int yx_os_system_logicalcpuNum(void)
{
    
    /*加电时逻辑核心数*/
    
    static yx_int logicalcpu_num = -1;
    
    if (-1 == logicalcpu_num)
    {
        yx_size len = sizeof(logicalcpu_num);
        if (yx_unlikely( 0 != sysctlbyname("hw.logicalcpu", &logicalcpu_num, &len, NULL, 0) ))
            return -1;
    }
    
    return logicalcpu_num;
}


int yx_os_system_logicalcpuMaxNum(void)
{
    
    /*当前可引导最大逻辑核心数*/
    
    static yx_int logicalcpu_max_num = -1;
    
    if (-1 == logicalcpu_max_num)
    {
        yx_size len = sizeof(logicalcpu_max_num);
        if (yx_unlikely( 0 != sysctlbyname("hw.logicalcpu_max", &logicalcpu_max_num, &len, NULL, 0) ))
            return -1;
    }
    
    return logicalcpu_max_num;
}

yx_int yx_os_system_cpufrequency(void)
{
    int mib[2] = {CTL_HW, HW_CPU_FREQ};
    yx_uint32 freq = (yx_uint32)-1;
    
    if ((yx_uint32)-1 == freq)
    {
        yx_size len = sizeof(freq);
        if (yx_unlikely(0 != sysctl(mib, 2, &freq, &len, NULL, 0)))
            return (yx_int)-1;
    }
    
    return (yx_int)freq;
}

const yx_char* yx_os_system_cpubrand(void)
{
    static yx_char brandinfo[63+1] = "";
    
    if (brandinfo[0] == '\0')
    {
        yx_size len = 31;
        if (yx_unlikely( 0 != sysctlbyname("machdep.cpu.brand_string", brandinfo, &len, NULL, 0) ))
            return NULL;
        
        brandinfo[31] = '\0';
    }
    
    
    return brandinfo;

}

int yx_os_system_cpucachelinesize(void)
{
    int mib[2] = {CTL_HW, HW_CACHELINE};
    yx_uint32 cachelinesize = (yx_uint32)-1;
    
    if ((yx_uint32)-1 == cachelinesize)
    {
        yx_size len = sizeof(cachelinesize);
        if (yx_unlikely(0 != sysctl(mib, 2, &cachelinesize, &len, NULL, 0)))
            return (yx_int)-1;
    }
    
    return (yx_int)cachelinesize;

}
int yx_os_system_cpucachelinesize_L1_I(void)
{
    int mib[2] = {CTL_HW, HW_L1ICACHESIZE};
    yx_uint32 cachelinesize = (yx_uint32)-1;
    
    if ((yx_uint32)-1 == cachelinesize)
    {
        yx_size len = sizeof(cachelinesize);
        if (yx_unlikely(0 != sysctl(mib, 2, &cachelinesize, &len, NULL, 0)))
            return (yx_int)-1;
    }
    
    return (yx_int)cachelinesize;

}

int yx_os_system_cpucachelinesize_L1_D(void)
{
    int mib[2] = {CTL_HW, HW_L1DCACHESIZE};
    yx_uint32 cachelinesize = (yx_uint32)-1;
    
    if ((yx_uint32)-1 == cachelinesize)
    {
        yx_size len = sizeof(cachelinesize);
        if (yx_unlikely(0 != sysctl(mib, 2, &cachelinesize, &len, NULL, 0)))
            return (yx_int)-1;
    }
    
    return (yx_int)cachelinesize;

}



int yx_os_system_cpucachelinesize_L2(void)
{
    int mib[2] = {CTL_HW, HW_L2CACHESIZE};
    yx_uint32 cachelinesize = (yx_uint32)-1;
    
    if ((yx_uint32)-1 == cachelinesize)
    {
        yx_size len = sizeof(cachelinesize);
        if (yx_unlikely(0 != sysctl(mib, 2, &cachelinesize, &len, NULL, 0)))
            return (yx_int)-1;
    }
    
    return (yx_int)cachelinesize;
}

int yx_os_system_cpucachelinesize_L3(void)
{
    int mib[2] = {CTL_HW, HW_L3CACHESIZE};
    yx_uint32 cachelinesize = (yx_uint32)-1;
    
    if ((yx_uint32)-1 == cachelinesize)
    {
        yx_size len = sizeof(cachelinesize);
        if (yx_unlikely(0 != sysctl(mib, 2, &cachelinesize, &len, NULL, 0)))
            return (yx_int)-1;
    }
    
    return (yx_int)cachelinesize;

}




int yx_os_system_byteorder(void)
{
    static yx_int byteorder = -1;
    
    if (-1 == byteorder)
    {
        yx_size len = sizeof(byteorder);
        if (yx_unlikely( 0 != sysctlbyname("hw.byteorder", &byteorder, &len, NULL, 0) ))
            return -1;
    }
    
    return byteorder;
}



yx_int32 yx_os_system_memsize32(void)
{
    static yx_int32 memsize = -1;
    
    if (-1 == memsize)
    {
        yx_size len = sizeof(memsize);
        if (yx_unlikely( 0 != sysctlbyname("hw.pysmem", &memsize, &len, NULL, 0) ))
            return -1;
    }
    
    return memsize;

}


yx_ulonglong yx_os_system_memsize(void)
{
    static yx_int64 memsize = -1;
    
    if (-1 == memsize)
    {
        yx_size len = sizeof(memsize);
        if (yx_unlikely( 0 != sysctlbyname("hw.memsize", &memsize, &len, NULL, 0) ))
            return (yx_ulonglong)-1;
    }
    
    return (yx_ulonglong)memsize;

}

yx_ulonglong yx_os_system_usermem(void)
{
    static yx_int non_kernel_mem = -1;
    
    if (-1 == non_kernel_mem)
    {
        yx_size len = sizeof(non_kernel_mem);
        if (yx_unlikely( 0 != sysctlbyname("hw.usermem", &non_kernel_mem, &len, NULL, 0) ))
            return (yx_ulonglong)-1;
    }
    
    return (yx_ulonglong)non_kernel_mem;
}

yx_ulonglong yx_os_system_memused(void)
{
    vm_size_t page_size;
    mach_port_t mach_port;
    mach_msg_type_number_t count;
    vm_statistics64_data_t vm_stats;
    
    mach_port = mach_host_self();
    count = sizeof(vm_stats) / sizeof(natural_t);
    if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
        KERN_SUCCESS == host_statistics64(mach_port, HOST_VM_INFO,
                                          (host_info64_t)&vm_stats, &count))
    {
        long long used_memory = ((int64_t)vm_stats.active_count +
                                 (int64_t)vm_stats.inactive_count +
                                 (int64_t)vm_stats.wire_count) *  (int64_t)page_size;
        return (yx_ulonglong)used_memory;
    }
    
    
    return 0;
}
yx_ulonglong yx_os_system_memfree(void)
{
    vm_size_t page_size;
    mach_port_t mach_port;
    mach_msg_type_number_t count;
    vm_statistics64_data_t vm_stats;
    
    mach_port = mach_host_self();
    count = sizeof(vm_stats) / sizeof(natural_t);
    if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
        KERN_SUCCESS == host_statistics64(mach_port, HOST_VM_INFO,
                                          (host_info64_t)&vm_stats, &count))
    {
        long long free_memory = (int64_t)vm_stats.free_count * (int64_t)page_size;
        return (yx_ulonglong)free_memory;
    }
    
    
    return (yx_ulonglong)-1;
}

yx_ulonglong yx_os_system_vmemsize(void)
{
    
    /*
     Unlike most Unix-based operating systems, Mac OS X does not use a preallocated swap partition for virtual memory. Instead, it uses all of the available space on the machine’s boot partition
     
     So, if you want to know how much virtual memory is still available, you need to get the size of the root partition. You can do that like this:
     */
    
    struct statfs stats;
    if (yx_unlikely( 0 == statfs("/", &stats) ))
        return (yx_ulonglong)-1;
    
    return stats.f_bsize * stats.f_bfree;
}

yx_ulonglong yx_os_system_vmemused(void)
{
    static yx_int32 vmused;
    yx_size len = sizeof(vmused);
    
    if (yx_unlikely( 0 != sysctlbyname("vm.swapusage", &vmused, &len, NULL, 0) ))
        return (yx_ulonglong)-1;
    
    return (yx_ulonglong)vmused;

}

yx_ulonglong yx_os_system_vmemused_curproc(void)
{
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
    
    if (KERN_SUCCESS != task_info(mach_task_self(),
                                  TASK_BASIC_INFO, (task_info_t)&t_info,
                                  &t_info_count))
    {
        return (yx_ulonglong)-1;
    }
    
    return t_info.virtual_size;
}




yx_int yx_os_system_pagesize(void)
{
    static yx_int pagesize = -1;
    
    if (-1 == pagesize)
    {
        yx_size len = sizeof(pagesize);
        if (yx_unlikely( 0 != sysctlbyname("hw.usermem", &pagesize, &len, NULL, 0) ))
            return -1;
        
        /*
         same as the following codes
         
         pagesize = (yx_int)sysconf(_SC_PAGE_SIZE);
         */
        
    }
    
    return pagesize;

}

yx_ulonglong yx_os_system_freediskspace(const yx_char* path)
{
    if (NULL == path)
        path = "/";
    
    struct statfs buf;
    yx_ulonglong freespace;
    freespace = 0;
    if(statfs(path, &buf) >= 0){
        freespace = (yx_ulonglong)buf.f_bsize * buf.f_bfree;
    }
    return freespace;
}

yx_ulonglong yx_os_system_totaldisspace(const yx_char* path)
{
    if (NULL == path)
        path = "/";
    
    struct statfs buf;
    yx_ulonglong totalspace;
    totalspace = 0;
    if(statfs(path, &buf) >= 0){
        totalspace = (yx_ulonglong)buf.f_bsize * buf.f_blocks;
    }
    return totalspace;
}





#pragma mark kernel
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
yx_size yx_os_system_maxarg(void)
{
    static uint64_t maxarg = (yx_size)-1;
    
    if ((yx_size)-1 == maxarg)
    {
        yx_size len = sizeof(maxarg);
        if (yx_unlikely( 0 != sysctlbyname("kern.argmax", &maxarg, &len, NULL, 0) ))
            return (yx_size)-1;
    }
    
    return (yx_size)maxarg;

}


yx_os_time yx_os_system_boottime(void)
{
    struct timeval tm;
    yx_size len = sizeof(tm);
    
    if (yx_unlikely( 0 != sysctlbyname("kern.boottime", &tm, &len, NULL, 0) ))
        return (yx_os_time)-1;
    
    
    
    return ((yx_real)tm.tv_usec / (yx_real)YX_USEC_PER_SEC);
}


yx_long yx_os_system_boottime_sec(void)
{
    struct timeval tm;
    yx_size len = sizeof(tm);
    
    if (yx_unlikely( 0 != sysctlbyname("kern.boottime", &tm, &len, NULL, 0) ))
        return (yx_long)-1;
    
    
    
    return (yx_long)(tm.tv_sec);
}

yx_long yx_os_system_boottime_usec(void)
{
    struct timeval tm;
    yx_size len = sizeof(tm);
    
    if (yx_unlikely( 0 != sysctlbyname("kern.boottime", &tm, &len, NULL, 0) ))
        return (yx_long)-1;
    
    
    
    return ((tm.tv_sec * (yx_long)YX_USEC_PER_SEC) + tm.tv_usec);
}


static struct clockinfo __clickinfo = {-1, -1, -1, -1, -1};
const struct clockinfo* yx_os_system_clockinfo(void)
{
    
    if (-1 == __clickinfo.hz)
    {
        yx_size len = sizeof(__clickinfo);
        if (yx_unlikely( 0 != sysctlbyname("kern.clockrate", &__clickinfo, &len, NULL, 0) ))
            return NULL;
    }
    
    return (const struct clockinfo*)&__clickinfo;
}


yx_int yx_os_system_hostid(void)
{
    static int hostid = -1;
    
    if (-1 == hostid)
    {
        yx_size len = sizeof(hostid);
        if (yx_unlikely( 0 != sysctlbyname("kern.hostid", &hostid, &len, NULL, 0) ))
            return -1;
    }
    
    return hostid;

}

const yx_char* yx_os_system_hostname(void)
{
    static yx_char hostname[63+1] = "";
    
    if (hostname[0] == '\0')
    {
        yx_size len = 63;
        if (yx_unlikely( 0 != sysctlbyname("kern.hostname", hostname, &len, NULL, 0) ))
            return NULL;
        
        hostname[63] = '\0';
    }
    
    
    return hostname;
    
}

yx_int yx_os_system_maxfileopened(void)
{
    static int maxfileopens = -1;
    
    if (-1 == maxfileopens)
    {
        yx_size len = sizeof(maxfileopens);
        if (yx_unlikely( 0 != sysctlbyname("kern.maxfiles", &maxfileopens, &len, NULL, 0) ))
            return -1;
    }
    
    return maxfileopens;
}


yx_int yx_os_system_maxfileopened_perproc(void)
{
    static int maxfileopens_proc = -1;
    
    if (-1 == maxfileopens_proc)
    {
        yx_size len = sizeof(maxfileopens_proc);
        if (yx_unlikely( 0 != sysctlbyname("kern.maxfilesperproc", &maxfileopens_proc, &len, NULL, 0) ))
            return -1;
    }
    
    return maxfileopens_proc;
}


yx_int yx_os_system_maxprocess(void)
{
    static int maxprocess = -1;
    
    if (-1 == maxprocess)
    {
        yx_size len = sizeof(maxprocess);
        if (yx_unlikely( 0 != sysctlbyname("kern.maxfilesperproc", &maxprocess, &len, NULL, 0) ))
            return -1;
    }
    
    return maxprocess;
}


yx_int yx_os_system_maxprocess_peruid(void)
{
    static int maxprocess_uid = -1;
    
    if (-1 == maxprocess_uid)
    {
        yx_size len = sizeof(maxprocess_uid);
        if (yx_unlikely( 0 != sysctlbyname("kern.maxprocperuid", &maxprocess_uid, &len, NULL, 0) ))
            return -1;
    }
    
    return maxprocess_uid;
}


yx_int yx_os_system_maxfileNum(void)
{
    static int maxvnode = -1;
    
    if (-1 == maxvnode)
    {
        yx_size len = sizeof(maxvnode);
        if (yx_unlikely( 0 != sysctlbyname("kern.maxvnodes", &maxvnode, &len, NULL, 0) ))
            return -1;
    }
    
    return maxvnode;
}


const yx_char* yx_os_system_kernerVersion(void)
{
    static yx_char comiletimeinfo[127+1] = "";
    
    if (comiletimeinfo[0] == '\0')
    {
        yx_size len = 127;
        if (yx_unlikely( 0 != sysctlbyname("kern.version", comiletimeinfo, &len, NULL, 0) ))
            return NULL;
        
        comiletimeinfo[127] = '\0';
    }
    
    
    return comiletimeinfo;
}


yx_int yx_os_system_releasedate(void)
{
    int mib[2];
    int releasedate;
    
    mib[0] = CTL_KERN;
    mib[1] = KERN_OSRELDATE;
    yx_size len = sizeof(releasedate);
    
    if (yx_unlikely( 0 != sysctl(mib, 2, &releasedate, &len, NULL, 0) ))
        return -1;
    
    return releasedate;
}


const yx_char* yx_os_release(void)
{
    static yx_char release[127+1] = "";
    
    int mib[2];
    mib[0] = CTL_KERN;
    mib[1] = KERN_OSRELEASE;

    
    if (release[0] == '\0')
    {
        yx_size len = 127;
        if (yx_unlikely( 0 != sysctl(mib, 2, release, &len, NULL, 0) ))
            return NULL;
        
        release[127] = '\0';
    }
    
    
    return release;

}

yx_int yx_os_system_revision(void)
{
    int mib[2];
    int revision;
    
    mib[0] = CTL_KERN;
    mib[1] = KERN_OSREV;
    yx_size len = sizeof(revision);
    
    if (yx_unlikely( 0 != sysctl(mib, 2, &revision, &len, NULL, 0) ))
        return -1;
    
    return revision;
}

const yx_char* yx_os_system_type(void)
{
    static yx_char systemtype[31+1] = "";
    
    int mib[2];
    mib[0] = CTL_KERN;
    mib[1] = KERN_OSRELEASE;
    
    
    if (systemtype[0] == '\0')
    {
        yx_size len = 31;
        if (yx_unlikely( 0 != sysctl(mib, 2, systemtype, &len, NULL, 0) ))
            return NULL;
        
        systemtype[31] = '\0';
    }
    
    
    return systemtype;

}



const yx_char* yx_os_system_searchpath(void)
{
    static yx_char searchpath[4095+1] = "";
    
    int mib[2];
    mib[0] = CTL_USER;
    mib[1] = USER_CS_PATH;
    
    
    if (searchpath[0] == '\0')
    {
        yx_size len = 4095;
        if (yx_unlikely( 0 != sysctl(mib, 2, searchpath, &len, NULL, 0) ))
            return NULL;
        
        searchpath[4095] = '\0';
    }
    
    
    return searchpath;
}


static struct loadavg __loadavg;
const struct loadavg* yx_os_system_loadavg(void)
{
    yx_size len = sizeof(__loadavg);
    
    if (yx_unlikely( 0 !=  sysctlbyname("vm.loadavg", &__loadavg, &len, NULL, 0)))
        return NULL;
    
    return (const struct loadavg*)&__loadavg;
}

























