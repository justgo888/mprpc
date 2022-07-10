#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include "lockqueue.h"

// 定义宏 LOG_INFO("xxx %d %s", 20, "xxxx")
#define LOG_INFO(logmsgformat, ...) \
    do \
    {  \
        Logger &logger = Logger::getInstance(); \
        logger.setLevel(INFO); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.log(c); \
    } while(0) \

#define LOG_ERR(logmsgformat, ...) \
    do \
    {  \
        Logger &logger = Logger::getInstance(); \
        logger.setLevel(ERROR); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.log(c); \
    } while(0) \

//定义日志消息级别
enum LogLevel
{
    INFO,  //普通消息
    ERROR, //错误消息
};

class Logger
{
public:
    //获取日志单例
    static Logger &getInstance();
    //设置日志级别
    void setLevel(LogLevel level);
    //写日志
    void log(std::string msg);

private:
    int logLevel_;                     //记录日志级别
    LockQueue<std::string> lockqueue_; //日志缓冲队列

    Logger();
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
};

#endif