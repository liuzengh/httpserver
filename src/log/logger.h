#pragma once
#include "BlockingQueue.h"
#include "format.h"
#include <thread>
#include <string>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <unistd.h>
class Logger
{
    public:
        Logger();
        // 不可拷贝
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete; 
        virtual ~Logger();
        static Logger* getInstance();
        void asyncWrite();
        static void threadFunc();

        void write(const std::string& message);
        
    private:
        const char* filePath;
        FILE *fp;
        bool quitting;
        // BlockQueue必须声明在writeThread之前。
        std::unique_ptr<BlockingQueue<std::string>>  blockQueue;
        std::unique_ptr<std::thread> writeThread;
};

/*
template <typename S, typename... Args>
void logfun(const S& format_str, Args&&... args, const char *curFile = __FILE__ , int curLine = __LINE__)
{
    Logger* logger = Logger::getInstance();
    logger->write(format_str, args..., curFile, curLine);
}
*/

// 日志格式：
// 日期        时间      微妙     线程号（无语义意义） 日志级别  正文                  源文件:行号 
// 2021-03-03  22:09:18.611678Z  3102627584         INFO     I'm very good 678913  log_test3.cpp:22
class PrintLog
{
    public:
        PrintLog(const char *fileName, int line, std::string level);
        ~PrintLog()=default;
        template <typename S, typename... Args>
        void logfun(const S& format_str, Args&&... args)
        {
            formatTime(); // 日期 时间
        //  fmt::format_to(out, "{} ", syscall(SYS_gettid)); // 线程号，有系统调用，会慢
            auto id = std::this_thread::get_id();
            fmt::format_to(out, "{} ", (*(uint32_t*)&id));
            fmt::format_to(out, "{} ", level); // 日志级别
            fmt::format_to(out, format_str, args...); // 正文
            fmt::format_to(out, " {}:{}\n", fileName, line); // 源文件名：行号
            Logger* logger = Logger::getInstance();
            logger->write(to_string(out));
            out.clear();
        }
        void formatTime();
    private:
        std::string fileName;
        int line;
        std::string level;
        fmt::memory_buffer out;
       
};


#define LOG_INFO    PrintLog(__FILE__, __LINE__, "INFO").logfun
#define LOG_DEBUG   PrintLog(__FILE__, __LINE__, "DEBUG").logfun
#define LOG_WARN    PrintLog(__FILE__, __LINE__, "WARN").logfun
#define LOG_ERROR   PrintLog(__FILE__, __LINE__, "ERROR").logfun