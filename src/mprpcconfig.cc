#include "mprpcconfig.h"
#include <iostream>
#include <string>
//负责解析配置文件信息
void MprpcConfig::LoadConfigFile(const char *configfile)
{
    FILE *pf = fopen(configfile, "r");
    if (pf == nullptr)
    {
        std::cout << "cannot open this file: No such file!" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (!feof(pf))
    {
        char buf[512] = {0};
        fgets(buf, 512, pf);
        std::string read_buf(buf);
        if (read_buf.empty() || read_buf[0] == '#')
        {
            continue;
        }
        DelBlank(read_buf);
        int idx = read_buf.find('=');
        if (idx == -1)
        {
            continue;
        }
        std::string key;
        std::string value;

        key = read_buf.substr(0, idx);
        DelBlank(key);
        int endix = read_buf.find('\n');
        value = read_buf.substr(idx + 1, endix - idx - 1);
        DelBlank(value);

        configMap_.insert({key, value});
    }
    fclose(pf);
}
//查询对应的配置项信息
std::string MprpcConfig::QueryValue(const std::string &key) const
{
    auto it = configMap_.find(key);
    if (it == configMap_.end())
    {
        return "";
    }
    return it->second;
}
//删除配置信息中的空格
void MprpcConfig::DelBlank(std::string &src_buf)
{
    int idx = src_buf.find_first_not_of(' ');
    if (idx != -1)
    {
        // 说明字符串前面有空格
        src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }
    // 去掉字符串后面多余的空格
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1)
    {
        // 说明字符串后面有空格
        src_buf = src_buf.substr(0, idx + 1);
    }
}