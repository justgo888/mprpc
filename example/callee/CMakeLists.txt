set(SRC_LIST userservice.cc ../user.pb.cc)
#rpc服务的发布者provider
add_executable(provider ${SRC_LIST})
target_link_libraries(provider mprpc protobuf)