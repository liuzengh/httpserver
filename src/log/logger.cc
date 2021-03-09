#include "logger.h"

Logger::Logger():
    filePath("/home/ubuntu/project/Server/WebServerV2/webserver_lzh_v2/log/log.txt"),
    fp(fopen(filePath, "w")), // FIXME:更改文件打开方式的API,用更好的系统调用, 调试时覆盖，后面改为追加"a"
    quitting(false),
    blockQueue(new BlockingQueue<std::string>),
    writeThread(new std::thread(threadFunc))
    {
    }
Logger::~Logger()
{
    quitting = true;
    if(writeThread && writeThread->joinable())
        writeThread->join();
    fmt::print("~Logger\n");
    if(fp)
    { 
        fflush(fp); 
        fclose(fp);
    }
}
Logger* Logger::getInstance(){
    // 局部静态变量实现单例模型
    static Logger instance;
    return &instance;
}

void Logger::asyncWrite()
{
    while(!quitting)
    {
        //std::shared_ptr<std::string> spStr = blockQueue->wait_and_pop();
        std::shared_ptr<std::string> spStr =  blockQueue->wait_and_pop();
     //   fmt::print("消费者：{}", *spStr);
        fputs((*spStr).c_str(), fp);  // FIXME: using fmt instead fputs;
        fflush(fp);
    }
}
void Logger::threadFunc()
{
    Logger::getInstance()->asyncWrite();
}


void Logger::write(const std::string& message)
{
    //fmt::print("生成者: {}", str); // ok
    blockQueue->push(message);
}


PrintLog::PrintLog(const char *fileName, int line, std::string level):
            fileName(fileName), 
            line(line),
            level(level) { };
     
void PrintLog::formatTime(){
    struct timeval now;
    time_t time;
    gettimeofday(&now, NULL);
    time = now.tv_sec;
    struct tm result; 
    localtime_r(&time, &result);  
    fmt::format_to(out, "{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}.{:06d}Z ", 
        result.tm_year + 1900, result.tm_mon + 1, result.tm_mday,
        result.tm_hour, result.tm_min, result.tm_sec,now.tv_usec);
};
  