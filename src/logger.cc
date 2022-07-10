#include "logger.h"
#include "thread"
#include <time.h>
#include <iostream>

//获取日志单例
Logger& Logger::getInstance()
{
    static Logger logger;
    return logger;
}
//设置日志级别
void Logger::setLevel(LogLevel level)
{
    logLevel_ = level;
}
//写日志
void Logger::log(std::string msg)
{
    lockqueue_.Push(msg);
}

Logger::Logger()
{
    //启动专门的写日志的线程
    std::thread WriteLogTask([&]()
                             {
        for(;;)
        {
            //获取当前日期，然后取日志信息，写入相应的文件中 a+
            time_t now = time(nullptr);
            tm* nowtime = localtime(&now);

            //组织日志文件名称
            char file_name[128]={0};
            sprintf(file_name,"%d-%d-%d-log.txt",nowtime->tm_year+1900,nowtime->tm_mon+1,nowtime->tm_mday);

            //以a+的形式打开文件
            FILE* pf = fopen(file_name,"a+");
            if(pf == nullptr)
            {
                std::cout << "logger file : " << file_name << " open error!" << std::endl;
                exit(EXIT_FAILURE);
            }
            std::string msg = lockqueue_.pop();
            char time_buf[128]  = {0};
            sprintf(time_buf,"%d:%d:%d =>[%s]",nowtime->tm_hour,nowtime->tm_min
            ,nowtime->tm_sec,(logLevel_ == INFO ? "info":"error"));

            msg.insert(0,time_buf);
            msg.append("\n");

            fputs(msg.c_str(),pf);
            fclose(pf);
        } });
        //设置线程分离，守护线程！！！！！
        WriteLogTask.detach();
}