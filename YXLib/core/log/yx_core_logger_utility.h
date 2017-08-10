//
//  yx_core_logger_utility.h
//  YXLib
//
//  Created by LiuYuxi on 15/10/9.
//  Copyright © 2015年 Yuxi Liu. All rights reserved.
//

#ifndef _YX_CORE_LOGGER_UTILITY_H_
#define _YX_CORE_LOGGER_UTILITY_H_

#include "yx_core_logger.h"


#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#define YX_DEBUGLOG(logger, ...) \
yx_core_log(logger, __YXFILE__, sizeof(__YXFILE__), __YXFUNC__, sizeof(__YXFUNC__), __YXLINE__, YX_LOGGER_DEBUG, __VA_ARGS__)

#define YX_INFOLOG(logger, ...) \
yx_core_log(logger, __YXFILE__, sizeof(__YXFILE__), __YXFUNC__, sizeof(__YXFUNC__), __YXLINE__, YX_LOGGER_INFO, __VA_ARGS__)

#define YX_NOTICE(logger, ...) \
yx_core_log(logger, __YXFILE__, sizeof(__YXFILE__), __YXFUNC__, sizeof(__YXFUNC__), __YXLINE__, YX_LOGGER_NOTICE, __VA_ARGS__)

#define YX_WARN(logger, ...) \
yx_core_log(logger, __YXFILE__, sizeof(__YXFILE__), __YXFUNC__, sizeof(__YXFUNC__), __YXLINE__, YX_LOGGER_WARN, __VA_ARGS__)

#define YX_ERROR(logger, ...) \
yx_core_log(logger, __YXFILE__, sizeof(__YXFILE__), __YXFUNC__, sizeof(__YXFUNC__), __YXLINE__, YX_LOGGER_ERROR, __VA_ARGS__)

#define YX_FATAL(logger, ...) \
yx_core_log(logger, __YXFILE__, sizeof(__YXFILE__), __YXFUNC__, sizeof(__YXFUNC__), __YXLINE__, YX_LOGGER_FATAL, __VA_ARGS__)




#elif defined __GNUC__

#define YX_DEBUGLOG(logger, format, args...) \
yx_core_log(logger, __YXFILE__, sizeof(__YXFILE__), __YXFUNC__, sizeof(__YXFUNC__), __YXLINE__, YX_LOGGER_DEBUG, format, ##args)

#define YX_INFOLOG(logger, format, args...) \
yx_core_log(logger, __YXFILE__, sizeof(__YXFILE__), __YXFUNC__, sizeof(__YXFUNC__), __YXLINE__, YX_LOGGER_INFO, format, ##args)

#define YX_NOTICE(logger, format, args...) \
yx_core_log(logger, __YXFILE__, sizeof(__YXFILE__), __YXFUNC__, sizeof(__YXFUNC__), __YXLINE__, YX_LOGGER_NOTICE, format, ##args)

#define YX_WARN(logger, format, args...) \
yx_core_log(logger, __YXFILE__, sizeof(__YXFILE__), __YXFUNC__, sizeof(__YXFUNC__), __YXLINE__, YX_LOGGER_WARN, format, ##args)

#define YX_ERROR(logger, format, args...) \
yx_core_log(logger, __YXFILE__, sizeof(__YXFILE__), __YXFUNC__, sizeof(__YXFUNC__), __YXLINE__, YX_LOGGER_ERROR, format, ##args)

#define YX_FATAL(logger, format, args...) \
yx_core_log(logger, __YXFILE__, sizeof(__YXFILE__), __YXFUNC__, sizeof(__YXFUNC__), __YXLINE__, YX_LOGGER_FATAL, format, ##args)


#endif

#endif /* _YX_CORE_LOGGER_UTILITY_H_ */
