#mprpc属于liuyu构建的rpc通信框架

#项目结构：
bin:可执行文件
build:项目编译文件
lib:项目库文件
src:源文件
test:测试代码
example:框架使用示例
CMakeLists.txt 顶层的cmake文件
autobuild.sh 项目一键编译脚本

#功能描述
使用google开源的protobuf实现rpc框架的数据序列化和反序列化功能，包括rpc方法参数的打包和解析；
使用开源的muduo网络库(陈硕)构建框架的网络部分，实现rpc服务主机的寻找、rpc调用请求的发起和rpc调用结果的响应；
使用zookeeper服务配置中心实现rpc框架的服务发现功能。