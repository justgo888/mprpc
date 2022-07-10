#ifndef MPRPCPROVIDER_H
#define MPRPCPROVIDER_H
#include <memory>
#include <unordered_map>
#include <google/protobuf/service.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/InetAddress.h>

class RpcProvider
{
public:
    void NotifyService(google::protobuf::Service* service);

    void Run();

    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message *response);

private:
    muduo::net::EventLoop m_loop; //事件循环
    void OnConnection(const muduo::net::TcpConnectionPtr &);
    void OnMessage(const muduo::net::TcpConnectionPtr &,muduo::net::Buffer *,muduo::Timestamp);
    
    struct ServiceInfo
    {
        google::protobuf::Service* m_service;
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> methodeMap_;
    };

    std::unordered_map<std::string,ServiceInfo> serviceMap_;
    
};

#endif