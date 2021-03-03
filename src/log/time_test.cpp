#include <chrono>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include "logger.h"
#include <unistd.h>
#include <sys/syscall.h>
int main()
{
    struct timeval now = {0, 0};
    gettimeofday(&now, nullptr);
    time_t tSec = now.tv_sec;
    struct tm result;

    localtime_r(&tSec, &result);
//    printf("%04d-%02d-%02d %02d:%02d:%02d.%06ldZ", result.tm_year + 1900, result.tm_mon + 1, result.tm_mday
//                                        ,result.tm_hour, result.tm_min, result.tm_sec
//                                        ,now.tv_usec);
    fmt::print("{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}.{:06d}Z", result.tm_year + 1900, result.tm_mon + 1, result.tm_mday
                                        ,result.tm_hour, result.tm_min, result.tm_sec
                                        ,now.tv_usec);
    

    return 0;
}