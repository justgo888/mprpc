#include "mprpcprovider.h"
#include "mprpcapplication.h"
#include "logger.h"
#include "zookeeperutil.h"
#include "rpcheader.pb.h"
#include <functional>
#include <string>
#include <google/protobuf/descriptor.h>

void RpcProvider::NotifyService(google::protobuf::Service* service)
{
    //获取服务描述指针
    const google::protobuf::ServiceDescriptor *serviceDesc = service->GetDescriptor();
    //获取服务名称
    std::string service_name = serviceDesc->name();
    //统计service中的方法数量
    int method_count = serviceDesc->method_count();
    LOG_INFO("service_name:%s", service_name.c_str());
    //存储服务信息
    struct ServiceInfo service_info;
    for (int i = 0; i < method_count; i++)
    {
        //当前方法的描述信息
        const google::protobuf::MethodDescriptor *methodDesc = serviceDesc->method(i);
        //方法名称
        std::string method_name = methodDesc->name();
        // method存入表中
        service_info.methodeMap_.insert({method_name, methodDesc});

        LOG_INFO("method_name:%s", method_name.c_str());
    }
    service_info.m_service = service;
    // service存入表中
    serviceMap_.insert({service_name, service_info});
}

void RpcProvider::Run()
{
    //获取ip地址和端口
    std::string ip = MprpcApplication::getConfig().QueryValue("rpcserverip");
    uint32_t port = atoi(MprpcApplication::getConfig().QueryValue("rpcserverport").c_str());
    muduo::net::InetAddress addr(ip, port);

    //填写InetAddress信息
    muduo::net::TcpServer server(&m_loop, addr, "rpcProvider");

    //绑定连接事件和读写事件的回调函数
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    //设置muduo线程库的线程数量
    server.setThreadNum(4);

    // 把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
    ZkClient zkCli;
    zkCli.Start();
    // service_name为永久性节点    method_name为临时性节点
    for (auto &sp : serviceMap_) 
    {
        // /service_name   /UserServiceRpc
        std::string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0);
        for (auto &mp : sp.second.methodeMap_)
        {
            // /service_name/method_name   /UserServiceRpc/Login 存储当前这个rpc服务节点主机的ip和port
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            // ZOO_EPHEMERAL表示znode是一个临时性节点
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    //调试显示信息
    std::cout << "server start at ip:" << ip << " port:" << port << std::endl;

    //开启服务和循环
    server.start();
    m_loop.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        conn->shutdown();
    }
}
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp time)
{
    //转换为字符流
    std::cout<<"1111"<<std::endl;
    std::string recv_buf = buf->retrieveAllAsString();
    uint32_t head_size = 0;
    recv_buf.copy((char *)&head_size, 4, 0);
    //根据
    std::string rpc_header_str = recv_buf.substr(4, head_size);
    mprpc::RpcHeader rpcheader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (rpcheader.ParseFromString(rpc_header_str))
    {
        service_name = rpcheader.service_name();
        method_name = rpcheader.method_name();
        args_size = rpcheader.args_size();
    }

    std::string args_str = recv_buf.substr(4 + head_size, args_size);

    // 打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << head_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "============================================" << std::endl;

    //获取service对象和method对象
    auto it = serviceMap_.find(service_name);
    if(it == serviceMap_.end())
    {
        std::cout<<service_name<<"not exist!"<<std::endl;
        return;
    }

    auto mit = it->second.methodeMap_.find(method_name);
    if(mit == it->second.methodeMap_.end())
    {
        std::cout<<service_name<<":"<<method_name<<"not exist!"<<std::endl;
        return;
    }
    google::protobuf::Service* service = it->second.m_service;
    const google::protobuf::MethodDescriptor* method = mit->second;

    //生成rpc调用的请求和响应
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str))
    {
        std::cout << "request parse error, content:" << args_str << std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();
    //给下面的method方法的调用提供一个回调函数closure
    google::protobuf::Closure* done = google::protobuf::NewCallback<RpcProvider,const muduo::net::TcpConnectionPtr& ,google::protobuf::Message*>(this,&RpcProvider::SendRpcResponse,conn, response);
    //在框架上根据远端rpc请求，调用当前rpc节点上发布的方法
    //new UserService().Login(controller, request, response, done)
    service->CallMethod(method,nullptr,request,response,done);
}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message *response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str))
    {
        conn->send(response_str);
    }
    else{
        std::cout<<"response serialize fail"<<std::endl;
    }
    //模仿http,由rpcprovider主动断开连接
    conn->shutdown();
}
