#include <iostream>
#include <string>
#include "user.pb.h"
#include <memory>
#include <mprpcapplication.h>
#include <mprpcprovider.h>

// UserService继承自在proto文件中定义的UserServiceRpc类
//
class UserService : public justgo::UserServiceRpc
{
public:
    /*
    caller远程调用Login方法callee实际要执行的方法
    callee获得函数名和参数从而调用这个函数
    */
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "Login success!" << std::endl;
        std::cout << "name:" << name << "pwd:" << pwd << std::endl;
        return true;
    }

    /*
    rpc远程调用的过程：
    caller -> 函数名加函数参数 -> protobuf序列化 -> muduo发送给callee
    callee -> muduo接收到数据 -> protobuf反序列化 -> callee获得函数名和参数
    */

    //重写UserServiceRpc的虚函数
    void Login(google::protobuf::RpcController *controller,
               const ::justgo::LoginRequest *request,
               ::justgo::LoginResponse *response,
               ::google::protobuf::Closure *done)
    {
        //先从request中获得函数参数
        std::string name = request->name();
        std::string pwd = request->pwd();

        //执行需要调用的方法
        bool login_result = Login(name, pwd);

        if (login_result)
        {
            std::cout<<"1111qqq"<<std::endl;
            //组织调用成功的响应消息
            justgo::ResultCode *code = response->mutable_result(); //创建一个指针指向response中的ResultCode,从而设置errcode和errmsg
            code->set_errcode(0);                                  //成功返回0
            code->set_errmsg("");                                  //返回空
            response->set_success(true);                           //返回true
        }
        else
        {
            //组织调用失败的响应消息
            justgo::ResultCode *code = response->mutable_result(); //创建一个指针指向response中的ResultCode,从而设置errcode和errmsg
            code->set_errcode(-1);                                 //失败返回-1
            code->set_errmsg("login fail");                        //返回失败信息
            response->set_success(false);                          //返回false
        }
        //执行回调操作，进行response的序列化和网络发送
        done->Run();
    }
};


int main(int argc,char** argv)
{
    //已经构建好了UserService类，那么这个rpc框架应该怎么使用呢？

    //第一步：初始化框架
    MprpcApplication::getInstance().Init(argc,argv);

    //第二步：发布callee提供的支持远程调用的方法
    RpcProvider provider;//网络服务的类对象
    provider.NotifyService(new UserService());

    //第三步：阻塞等待caller的调用消息
    provider.Run();

    return 0;
}