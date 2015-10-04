#ifndef APP_COMMON_H_
#define APP_COMMON_H_

#include "Core/Core.h"
#include "Net/Net.h"
#include "App/AppProto.pb.h"

#include "boost/program_options.hpp"

// 是否生成为静态库
#define APP_STATIC 1

// 导出宏
#if TARGET_PLATFORM == PLATFORM_WINDOWS
#   ifdef APP_STATIC
#	    define APP_API
#   else
#	    ifdef APP_EXPORTS
#		    define APP_API __declspec(dllexport)
#	    else
#		    define APP_API __declspec(dllimport)
#	    endif // APP_EXPORTS
#   endif // APP_STATIC
#else
#	define APP_API
#endif // TARGET_PLATFORM

// 默认的空闲计数
#define APP_DEFAULT_IDLE_COUNT 10

// 默认的空闲挂起, 毫秒
#define APP_DEFAULT_IDLE_SLEEP 10

// 默认后门端口
#define APP_DEFAULT_BACKDOOR_PORT 30000

// 信号类型
enum SIGNAL_TYPE
{
    SIGNAL_INVALID = -1,
    SIGNAL_STOP, // 停止
    SIGNAL_RELOAD, // 重载配置
    NUM_SIGNAL_TYPE
};

#endif // APP_COMMON_H_
