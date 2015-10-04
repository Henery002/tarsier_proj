#ifndef CORE_COMMON_H_
#define CORE_COMMON_H_

#include "Core/Std.h"

#define BOOST_ENABLE_ASSERT_HANDLER

#include "boost/assert.hpp"
#include "boost/static_assert.hpp"

#include "boost/shared_ptr.hpp"

#include "boost/thread.hpp"

#include "boost/date_time/posix_time/posix_time.hpp"

// 平台类型
#define PLATFORM_LINUX 1
#define PLATFORM_WINDOWS 2

// Linux
#ifdef linux
#	define TARGET_PLATFORM PLATFORM_LINUX
#endif // Linux

// Windows
#ifdef _WIN32
#	define TARGET_PLATFORM PLATFORM_WINDOWS
#endif // _WIN32

#if TARGET_PLATFORM == PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN

// 平台头文件
#include <Windows.h>

#endif // TARGET_PLATFORM

// 是否生成为静态库
#define CORE_STATIC 1

// 导出宏
#if TARGET_PLATFORM == PLATFORM_WINDOWS
#   ifdef CORE_STATIC
#	    define CORE_API
#   else
#	    ifdef CORE_EXPORTS
#		    define CORE_API __declspec(dllexport)
#	    else
#		    define CORE_API __declspec(dllimport)
#	    endif // CORE_EXPORTS
#   endif // CORE_STATIC
#else
#	define CORE_API
#endif // TARGET_PLATFORM

// 可移植的 snprintf
#ifdef _MSC_VER
#   ifndef snprintf
#       define snprintf _snprintf
#   endif // snprintf
#endif // _MSC_VER

// 调试宏
#ifdef _MSC_VER
#   ifdef _DEBUG
#       define DEBUG 1
#   endif // _DEBUG
#endif // _MSC_VER

// 获得执行文件所在目录
const std::string& GetExeDir();

#if TARGET_PLATFORM == PLATFORM_WINDOWS

// 获得进程 Id
typedef int pid_t;
#define getpid() GetCurrentProcessId()

// 把进程挂起一段时间, 单位: 微秒(千分之一毫秒)
void usleep(unsigned long usec);

// 把进程挂起一段时间, 单位: 秒
unsigned int sleep(unsigned int seconds);

// 获得错误字符串
boost::shared_ptr<std::string> GetStrError(DWORD* errorCode = NULL);

#elif TARGET_PLATFORM == PLATFORM_LINUX

// 获得错误字符串
boost::shared_ptr<std::string> GetStrError(int* errorCode = NULL);

#endif // TARGET_PLATFORM

#endif // CORE_COMMON_H_
