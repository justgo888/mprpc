#include <iostream>
#include "mprpcchannel.h"
#include "user.pb.h"
#include "mprpcapplication.h"
#include "mprpccontroller.h"

int main(int argc, char **argv)
{
    //框架初始化
    MprpcApplication::getInstance().Init(argc, argv);
    MprpcController controller;
    //初始化代理类
    justgo::UserServiceRpc_Stub stub(new MrpcChannel());
    //调用请求
    justgo::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    //响应接收
    justgo::LoginResponse response;
    //调用服务，实际调用的是CallMethod()
    stub.Login(nullptr, &request, &response, nullptr);

    // 一次rpc调用完成，读调用的结果
    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    else{
        if(0 == response.result().errcode())
        {
            std::cout << "rpc login response success:" << response.success() << std::endl;
        }
        else{
            std::cout << "rpc login response failed:" << response.result().errmsg() << std::endl;
        }
    }
        
    return 0;
}