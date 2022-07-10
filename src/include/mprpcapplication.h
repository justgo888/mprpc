#ifndef MPRPCAPPLICATION_H
#define MPRPCAPPLICATION_H
#include "mprpcconfig.h"
class MprpcApplication
{
public:
    void Init(int argc, char **argv);
    static MprpcApplication& getInstance();
    static MprpcConfig& getConfig();
private:
    static MprpcConfig m_rpcconfig;
};

#endif