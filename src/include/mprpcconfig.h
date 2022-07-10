#ifndef MPRPCCONFIG_H
#define MPRPCCONFIG_H
/*
读取配置文件信息的类对象
*/
#include <string>
#include <unordered_map>

class MprpcConfig
{
public:
    //负责解析配置文件信息
    void LoadConfigFile(const char* configfile);
    //查询对应的配置项信息
    std::string QueryValue(const std::string& key) const;
private:
    std::unordered_map<std::string,std::string> configMap_;//存储配置信息的map表
    //删除string中前后的空格
    void DelBlank(std::string& src_buf);
};


#endif