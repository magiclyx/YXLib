//
//  yx_unittest_unittest.c
//  YXLib
//
//  Created by Yuxi Liu on 9/8/14.
//  Copyright (c) 2014 Yuxi Liu. All rights reserved.
//

#include "yx_unittest_unittest.h"
#include "yx_unittest_list.h"

static const unsigned int _yx_unittest_buffer_len = 1024;

static const yx_os_tick _yx_unittest_time_limited_none = -1.0f;


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 context declare
 */

typedef enum __yx_test_context_type{
    yx_test_context_test,
    yx_test_context_case,
    yx_test_context_group,
}yx_test_context_type;



typedef struct __yx_test_ctx{
    yx_test_context_type ctx_type;
    
    yx_allocator allocator;
    
    _yx_ut_list group_list; // a list of test group
    yx_uint32 repeat_times; //how many times will the case repeats, default 1 times
    
    yx_uint32 group_num;  // total group num
    yx_uint32 totle_num;  // total case num
    yx_uint32 failed_num; // failed case num
    
    yx_os_file stdout_file;  //will buckup the stdout here
    yx_os_file report_file;    //will write the ut report here
    
    yx_char root_path[YX_MAX_PATH_LEN+1]; //a path for report
}yx_test_ctx, *yx_test_ctx_ref;


typedef struct __yx_test_group{
    yx_test_context_type ctx_type;
    
    yx_allocator allocator;
    
    yx_test_ctx_ref owner_test;  //point to the yx_test_ctx. if not add to a test, NULL is set
    
    _yx_ut_list case_list;    //a list of thest case
    yx_char group_name[_yx_unittest_buffer_len+1]; //the group name
    
    yx_uint32 totle_num;  //total case num in group
    yx_uint32 failed_num; //failed case num in group
    
    yx_uint32 repeat_times; //repeat times. if not indicated, use the one set in yx_text_ctx
}yx_test_group, *yx_test_group_ref;


typedef struct __yx_test_case{
    yx_test_context_type ctx_type;
    
    yx_test_group_ref owner_group; //point to the yx_test_group.
    
    yx_ut_testcase test_fun; //call-back func
    yx_os_time time_limit; //time limit in test
    yx_char case_name[_yx_unittest_buffer_len+1]; //the case name
    
    yx_result result; //test result
    
    /*use when test failed*/
    yx_char exp[_yx_unittest_buffer_len+1];  //expression
    yx_char msg[_yx_unittest_buffer_len+1]; //err msg
    yx_char file[_yx_unittest_buffer_len+1]; //file name
    yx_uint32 line; //line
}yx_test_case, *yx_test_case_ref;


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 private declare
 */

//log fun
static void _yx_ut_report(yx_test_ctx_ref test_context_ptr, yx_vaparam(const yx_char* yx_restrict format)) yx_vafun_declare yx_printf_fmtcheck(2, 3);

//release context(group, case) in list
static void _yx_ut_release_obj_context(_yx_ut_list list, yx_handle handle, yx_index index);
//destroy a test case
static void _yx_ut_destroy_case(yx_test_case_ref* test_case_context_ptr);

//run the cse
static yx_bool _yx_ut_run_groups(_yx_ut_list list, yx_handle handle, yx_index index);
static yx_bool _yx_ut_run_case(_yx_ut_list list, yx_handle handle, yx_index index);

//report the detail error info
static yx_bool _yx_ut_report_groups(_yx_ut_list list, yx_handle handle, yx_index index);
static yx_bool _yx_ut_report_case(_yx_ut_list list, yx_handle handle, yx_index index);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


yx_unittest yx_ut_create(yx_allocator allocator){
    //:~ TODO not completed
    yx_test_ctx_ref context = NULL;
    
    
    if (NULL == allocator) {
        allocator = yx_get_allocator();
    }
    
    /*create context*/
    context = (yx_test_ctx_ref)yx_allocator_alloc(allocator, sizeof(yx_test_ctx));
    if (NULL == context) {
        goto errout;
    }
    
    yx_os_memzero(context, sizeof(yx_test_ctx));
    
    
    /*create allocator*/
    context->allocator = allocator;
    
    /*set context type*/
    context->ctx_type = yx_test_context_test;
    
    
    /*create group list*/
    context->group_list = _yx_ut_list_create(allocator);
    if (NULL == context->group_list) {
        goto errout;
    }
    
    
    /*num of case or groups*/
    context->group_num = 0;
    context->totle_num = 0;
    context->failed_num = 0;
    
    
    /*repeat times, default 1*/
    context->repeat_times = 1;
    
    
    /*output path*/
    yx_os_date* pdate = yx_os_date_now(yx_false);
    yx_os_snprintf(context->root_path, YX_MAX_PATH_LEN , "%s/%02d:%02d:%02d_%04d-%02d-%02d"
                   , YX_UT_OUTPUT_FOLDER
                   , yx_os_date_hour(pdate)
                   , yx_os_date_minute(pdate)
                   , yx_os_date_second(pdate)
                   , yx_os_date_year(pdate)
                   , yx_os_date_month(pdate)
                   , yx_os_date_month(pdate));
    
    
    /*create output folder*/
    if(0 != yx_os_dir_create(context->root_path, YX_OS_FILE_AU_EXECUTE)){
        
        /*if the folder exist, and can read, write. it is not an error*/
        if (YX_ERROR_NOTEXIST == yx_os_err_getlasterr()) {
            if (0 != yx_os_path_realusr_isWritable(context->root_path)) {
                goto errout;
            }
        }
        else{
            goto errout;
        }
    }
    
    /*buckup the stdout */
    context->stdout_file =  yx_os_file_duplicate(yx_os_file_stdout);
    if (yx_result_failed == context->stdout_file)
        goto errout;
    
    /*redirect the output file*/
    yx_char output_path[YX_MAX_PATH_LEN+1];
    yx_os_strncpy(output_path, context->root_path, YX_MAX_PATH_LEN);
    yx_os_strncat(output_path, "/output", YX_MAX_PATH_LEN);
    int outputfile = yx_os_file_createForUpdate(output_path, 0);
    yx_os_file_redirect_stout(outputfile);
    yx_os_file_close(outputfile);
    
    
    //report_file
    yx_char report_path[YX_MAX_PATH_LEN+1];
    yx_os_strncpy(report_path, context->root_path, YX_MAX_PATH_LEN);
    yx_os_strncat(report_path, "/report", YX_MAX_PATH_LEN);
    context->report_file = yx_os_file_createForUpdate(report_path, 0);
    
    
    return (yx_unittest)context;
    
    
errout:
    
    if (NULL != context) {
        
        if (NULL != context->group_list) {
            _yx_ut_list_destroy(&(context->group_list), _yx_ut_release_obj_context);
        }
        
        if (context->stdout_file > 0) {
            
            yx_os_file_redirect_stout(context->stdout_file);
            
            yx_os_file_close(context->stdout_file);
        }
        
        if (context->report_file > 0) {
            yx_os_file_close(context->report_file);
        }
        
        
        yx_allocator_free(allocator, context);
    }
    
    return NULL;
}


void yx_ut_destroy(yx_unittest* test_handle_ptr){
    
    do {
        
        if (NULL == test_handle_ptr)
            break;
        
        yx_test_ctx_ref context = (yx_test_ctx_ref)(*test_handle_ptr);
        *test_handle_ptr = NULL;
        
        if (NULL == context)
            break;
        
        
        yx_allocator allocator = context->allocator;
        
        if (NULL != context->group_list){
            _yx_ut_list_destroy(&(context->group_list), _yx_ut_release_obj_context);
        }
        
        
        if (context->stdout_file > 0) {
            
            yx_os_file_flush(yx_os_file_stdout); //flush posix file handle (for fwrite)
            fflush(stdout); //flush ansi C FILE (for printf)
            
            yx_os_file_redirect_stout(context->stdout_file);
            yx_os_file_close(context->stdout_file);
        }
        
        if (context->report_file > 0) {
            yx_os_file_close(context->report_file);
        }
        
        
        yx_allocator_free(allocator, context);
        
    } while (0);
    
}

void yx_ut_add_group(yx_unittest ut, yx_unittest_group group){
    
    yx_test_ctx_ref context = (yx_test_ctx_ref)ut;
    YX_ASSERT(NULL != context);
    
    yx_test_group_ref group_context = (yx_test_group_ref)group;
    YX_ASSERT(NULL == group_context->owner_test); //make shure the group is a fresh new group
    
    
    /*some config should inheriting from the test*/
    if (0 == group_context->repeat_times) {
        group_context->repeat_times = context->repeat_times;
    }
    
    /*set test owner*/
    group_context->owner_test = context;
    
    /*insert the group*/
    _yx_ut_list_insert_back(context->group_list, group);
    
    /*increate the total case num in test context*/
    context->totle_num += group_context->totle_num;
    /*increate the total group num in test context*/
    context->group_num++;
}

void yx_ut_set_repeat_times(yx_unittest ut, yx_uint times){
    yx_test_ctx_ref context = (yx_test_ctx_ref)ut;
    context->repeat_times = times;
}

void yx_ut_run(yx_unittest ut){
    yx_test_ctx_ref context = (yx_test_ctx_ref)ut;
    YX_ASSERT(NULL != context);
    
    YX_ASSERT(NULL != context->group_list);
    
    
    _yx_ut_report(context, "############################################################\n");
    _yx_ut_report(context, "#  yxlib unittest\n");
    _yx_ut_report(context, "#\n");
    _yx_ut_report(context, "#  report path:%s\n", context->root_path);
    _yx_ut_report(context, "#  group num:%d\n", context->group_num);
    _yx_ut_report(context, "#  case num:%d\n", context->totle_num);
    _yx_ut_report(context, "############################################################\n");
    _yx_ut_report(context, "\n\n\n");
    
    _yx_ut_list_walk(context->group_list, _yx_ut_run_groups);
    
    
    if (0 != context->failed_num) {
        _yx_ut_report(context, "############################################################\n");
        _yx_ut_report(context, "#  detail info\n");
        _yx_ut_report(context, "#  number failed case:%d\n", context->failed_num);
        _yx_ut_report(context, "############################################################\n");
        _yx_ut_report(context, "\n\n\n");
        _yx_ut_list_walk(context->group_list, _yx_ut_report_groups);
    }
    
}


/*yx_test_case*/
yx_unittest_group yx_ut_create_group(yx_allocator allocator, const yx_char* name){
    
    
    if (NULL == allocator) {
        allocator = yx_get_allocator();
    }
    
    
    yx_test_group_ref testgroup = (yx_test_group_ref)yx_allocator_alloc(allocator, sizeof(yx_test_group));
    if (NULL == testgroup)
        goto errout;
    
    testgroup->allocator = allocator;
    
    testgroup->owner_test = NULL;
    
    testgroup->ctx_type = yx_test_context_group;
    
    if (NULL != name){
        yx_os_strncpy(testgroup->group_name, name, _yx_unittest_buffer_len);
    }
    else{
        yx_os_strempty(testgroup->group_name);
    }
    
    testgroup->case_list = _yx_ut_list_create(NULL);
    if (NULL == testgroup->case_list)
        goto errout;
    
    
    testgroup->failed_num = 0;
    testgroup->totle_num = 0;
    
    testgroup->repeat_times = 0;
    
    
    
    return testgroup;
    
errout:
    if (NULL != testgroup) {
        
        if (NULL != testgroup->case_list) {
            _yx_ut_list_destroy(&(testgroup->case_list), _yx_ut_release_obj_context);
        }
        
        yx_allocator_free(allocator, testgroup);
    }
    
    return NULL;
}
void yx_ut_destroy_group(yx_unittest_group* test_group_ptr){
    
    
    do {
        
        if (NULL == test_group_ptr)
            break;
        
        yx_test_group_ref testgroup = (yx_test_group_ref)(*test_group_ptr);
        *test_group_ptr = NULL;
        
        if (NULL == testgroup)
            break;
        
        
        yx_allocator allocator = testgroup->allocator;
        
        if (NULL != testgroup->case_list) {
            _yx_ut_list_destroy(&(testgroup->case_list), _yx_ut_release_obj_context);
        }
        
        yx_allocator_free(allocator, testgroup);
        
    } while (0);
    
}

void yx_ut_group_add_case(yx_unittest_group group, const yx_char* case_name, yx_ut_testcase case_fun){
    
    yx_test_group_ref group_context = (yx_test_group_ref)group;
    
    
    yx_test_case_ref testcase = (yx_test_case_ref)yx_allocator_alloc(group_context->allocator, sizeof(yx_test_case));
    YX_ASSERT(NULL != testcase);
    
    testcase->owner_group = group_context;
    
    testcase->ctx_type = yx_test_context_case;
    
    if (NULL != case_name){
        yx_os_strncpy(testcase->case_name, case_name, _yx_unittest_buffer_len);
    }
    else{
        yx_os_strempty(testcase->case_name);
    }
    
    
    testcase->time_limit = _yx_unittest_time_limited_none;
    
    testcase->test_fun = case_fun;
    testcase->result = yx_result_unknown;
    testcase->line = YX_MAX_UINT32;
    yx_os_strempty(testcase->file);
    yx_os_strempty(testcase->exp);
    yx_os_strempty(testcase->msg);
    
    YX_ASSERT(NULL != group_context->case_list);
    _yx_ut_list_insert_back(group_context->case_list, testcase);
    group_context->totle_num++;
    if (NULL != group_context->owner_test) {
        group_context->owner_test->totle_num++;
    }
}

void yx_ut_group_set_repeat_times(yx_unittest_group group, yx_uint times){
    yx_test_group_ref group_context = (yx_test_group_ref)group;
    if (NULL != group_context) {
        group_context->repeat_times = times;
    }
}


void yx_ut_case_error(yx_unittest_case test_case, const char* file, yx_uint line, const yx_char* msg, yx_char* expr){
    
    if (NULL == test_case)
        return;
    
    yx_test_case_ref case_context = (yx_test_case_ref)test_case;
    
    case_context->result = yx_result_failed;
    
    case_context->line = line;
    
    if (NULL != file)
        yx_os_strncpy(case_context->file, file, _yx_unittest_buffer_len);
    
    if (NULL != msg)
        yx_os_strncpy(case_context->msg, msg, _yx_unittest_buffer_len);
    
    if (NULL != expr) {
        yx_os_strncpy(case_context->exp, expr, _yx_unittest_buffer_len);
    }
    
}

void yx_ut_case_set_time_limit(yx_unittest_case test_case, yx_os_time limit){
    yx_test_case_ref case_context = (yx_test_case_ref)test_case;
    case_context->time_limit = limit;
}

/*utility*/
void yx_ut_printf(yx_unittest ut, yx_vaparam(char* format)) yx_vafun_declare
{
    yx_test_ctx_ref ut_context = (yx_test_ctx_ref)ut;
    
    yx_valist ap;
    char* fmt;
    FILE* cfile = yx_os_create_CFile(ut_context->stdout_file, "w+");
    
    yx_vastart(ap, format);
    fmt = yx_vaarg_first(ap, char*, format);
    vfprintf(cfile, fmt, ap);
    vprintf(fmt, ap);
    yx_vaend(ap);
    
    fclose(cfile);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
static void _yx_ut_report(yx_test_ctx_ref test_context_ptr, yx_vaparam(const yx_char* yx_restrict format)) yx_vafun_declare
{
    yx_valist valist;
    const yx_char* format_string;
    
    yx_char buff[_yx_unittest_buffer_len+1];
    
    yx_vastart(valist, format);
    format_string = yx_vaarg_first(valist, const yx_char*, format);
    yx_os_vsnprintf(buff, _yx_unittest_buffer_len, format_string, valist);
    buff[_yx_unittest_buffer_len] = '\0';
    yx_vaend(valist);
    
    yx_os_file_write(test_context_ptr->stdout_file, buff, yx_os_strlen(buff) * sizeof(yx_char));
    yx_os_file_write(test_context_ptr->report_file, buff, yx_os_strlen(buff) * sizeof(yx_char));
    yx_os_file_flush(test_context_ptr->stdout_file);
    yx_os_file_flush(test_context_ptr->report_file);
};

static void _yx_ut_release_obj_context(_yx_ut_list list, yx_handle handle, yx_index index)
{
    yx_unused(list);
    yx_unused(index);
    
    yx_test_context_type* type = (yx_test_context_type*)(handle);
    
    switch (*type) {
        case yx_test_context_group:
            //yx_ut_destroy_group(&handle); //the group is alloced outside.
            break;
        case yx_test_context_case:
            _yx_ut_destroy_case((yx_test_case_ref*)&handle);
            break;
            
        default:
            break;
    }
}


static void _yx_ut_destroy_case(yx_test_case_ref* test_case_context_ptr){
    
    do {
        if (NULL == test_case_context_ptr)
            break;
        
        yx_test_case* testcase_context = *test_case_context_ptr;
        *test_case_context_ptr = NULL;
        if (NULL == testcase_context)
            break;
        
        
        yx_allocator allocator = testcase_context->owner_group->allocator;
        yx_allocator_free(allocator, testcase_context);
        
    } while (0);
    
}


static yx_bool _yx_ut_run_groups(_yx_ut_list list, yx_handle handle, yx_index index)
{
    yx_unused(list);
    yx_unused(index);
    
    yx_test_group_ref group_context = (yx_test_group_ref)handle;
    YX_ASSERT(NULL != group_context);
    YX_ASSERT(yx_test_context_group == group_context->ctx_type);
    
    _yx_ut_report(group_context->owner_test, "------------------------------------------------------------\n");
    _yx_ut_report(group_context->owner_test, "%s\n", group_context->group_name);
    _yx_ut_report(group_context->owner_test, "------------------------------------------------------------\n");
    
    YX_ASSERT(NULL != group_context->case_list);
    _yx_ut_list_walk(group_context->case_list, _yx_ut_run_case);
    _yx_ut_report(group_context->owner_test, "\n\n\n");
    
    return yx_true;
}


static yx_bool _yx_ut_run_case(_yx_ut_list list, yx_handle handle, yx_index index)
{
    yx_unused(list);
    yx_unused(index);
    
    yx_test_case_ref case_context = (yx_test_case_ref)handle;
    YX_ASSERT(NULL != case_context);
    YX_ASSERT(yx_test_context_case == case_context->ctx_type);
    
    YX_ASSERT(NULL != case_context->owner_group);
    yx_test_group_ref group_context = case_context->owner_group;
    
    /*printf the case name*/
    _yx_ut_report(case_context->owner_group->owner_test, "%s ...", case_context->case_name);
    
    
    /*run case in several times*/
    YX_ASSERT(NULL != case_context->test_fun);
    yx_os_tick total_tick = 0; //use to calculate the average time
    for (yx_uint i = 0;  i < group_context->repeat_times;  i++) {
        
        /*init the test result*/
        case_context->result = yx_result_success;
        yx_os_strncpy(case_context->exp, "unknown", _yx_unittest_buffer_len);
        yx_os_strncpy(case_context->file, "unknown", _yx_unittest_buffer_len);
        yx_os_strempty(case_context->msg);
        case_context->line = 0;
        case_context->time_limit = _yx_unittest_time_limited_none;
        
        /*run the test*/
        yx_os_tick start = yx_os_ticket_now();
        case_context->test_fun(case_context);
        yx_os_tick end = yx_os_ticket_now();
        
        /*if failed, stop it*/
        if (yx_result_success != case_context->result) {
            break;
        }
        
        total_tick += (end - start);
        
        /*if defined a time limit, check it*/
        if (case_context->time_limit > 0.0f) {
            
            yx_real nsec_used = yx_os_ticket_toNanoSecond(end-start);
            if (nsec_used > case_context->time_limit)
            {
                /*
                 out of time, trate it as error and stop testing
                 */
                
                case_context->result = yx_result_failed;
                
                yx_os_snprintf(case_context->msg, _yx_unittest_buffer_len, "time out. (limited:%lf, used:%lf)", case_context->time_limit, nsec_used);
                
                break;
            }
            
        }
        
        
    }
    
    
    if (yx_result_success != case_context->result)
    {
        /*increated the failed num*/
        case_context->owner_group->failed_num++;
        case_context->owner_group->owner_test->failed_num++;
        
        /*print the "failed"*/
        _yx_ut_report(case_context->owner_group->owner_test, "failed\n");
    }
    else
    {
        /*print the "done"*/
        yx_os_tick average_tick = (yx_os_tick)(total_tick / group_context->repeat_times);
        _yx_ut_report(case_context->owner_group->owner_test, "done  (%lf)\n", yx_os_ticket_toNanoSecond(average_tick));
    }
    
    
    return yx_true;
}



static yx_bool _yx_ut_report_groups(_yx_ut_list list, yx_handle handle, yx_index index)
{
    yx_unused(list);
    yx_unused(index);
    
    yx_test_group_ref group_context = (yx_test_group_ref)handle;
    YX_ASSERT(NULL != group_context);
    YX_ASSERT(yx_test_context_group == group_context->ctx_type);
    
    YX_ASSERT(NULL != group_context->case_list);
    _yx_ut_list_walk(group_context->case_list, _yx_ut_report_case);
    
    return yx_true;

}
static yx_bool _yx_ut_report_case(_yx_ut_list list, yx_handle handle, yx_index index){
    yx_unused(list);
    yx_unused(index);
    
    yx_test_case_ref case_context = (yx_test_case_ref)handle;
    YX_ASSERT(NULL != case_context);
    YX_ASSERT(yx_test_context_case == case_context->ctx_type);
    
    YX_ASSERT(NULL != case_context->owner_group);
    yx_test_group_ref group_context = case_context->owner_group;
    
    
    if (yx_result_success != case_context->result) {
        
        _yx_ut_report(group_context->owner_test, "------------------------------\n");
        _yx_ut_report(case_context->owner_group->owner_test, "%s:%s\n", group_context->group_name, case_context->case_name);
        _yx_ut_report(case_context->owner_group->owner_test, "\tfile:%s\n", case_context->file);
        _yx_ut_report(case_context->owner_group->owner_test, "\tline:%d\n", case_context->line);
        _yx_ut_report(case_context->owner_group->owner_test, "\texpr:%s\n", case_context->exp);
        _yx_ut_report(case_context->owner_group->owner_test, "\tinfo:%s\n", case_context->msg);
        _yx_ut_report(case_context->owner_group->owner_test, "\n\n\n");

    }
    
    
    return yx_true;

}
