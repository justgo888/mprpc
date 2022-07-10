#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>
#include <mprpcconfig.h>
MprpcConfig MprpcApplication::m_rpcconfig;
void MprpcApplication::Init(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "format: command -i <configfile>" << std::endl;
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string config_file;
    while ((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            std::cout << "format: command -i <configfile>" << std::endl;
            exit(EXIT_FAILURE);
        case ':':
            std::cout << "format: command -i <configfile>" << std::endl;
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    // 开始加载配置文件了 rpcserver_ip=  rpcserver_port   zookeeper_ip=  zookepper_port=
    m_rpcconfig.LoadConfigFile(config_file.c_str());

    //std::cout << "rpcserverip:" << m_rpcconfig.QueryValue("rpcserverip") << std::endl;
    //std::cout << "rpcserverport:" << m_rpcconfig.QueryValue("rpcserverport") << std::endl;
    //std::cout << "zookeeperip:" << m_rpcconfig.QueryValue("zookeeperip") << std::endl;
    //std::cout << "zookeeperport:" << m_rpcconfig.QueryValue("zookeeperport") << std::endl;
}

MprpcConfig& MprpcApplication::getConfig()
{
    return m_rpcconfig;
}

MprpcApplication& MprpcApplication::getInstance()
{
    static MprpcApplication app;
    return app;
}
