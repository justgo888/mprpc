set(SRC_LIST 
    mprpcapplication.cc
    mprpcprovider.cc
    mprpcconfig.cc
    rpcheader.pb.cc
    mprpcchannel.cc
    mprpccontroller.cc
    logger.cc
    zookeeperutil.cc) 

add_library(mprpc ${SRC_LIST})
target_link_libraries(mprpc muduo_net muduo_base pthread protobuf zookeeper_mt)