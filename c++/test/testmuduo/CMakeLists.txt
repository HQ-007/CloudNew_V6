cmake_minimum_required(VERSION 3.0)
project(main)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

set(CMAKE_CXX_STANDARD 11)
# #配置头文件
# include_directories()
# #配置库文件
# link_directories()


#需要编译的源文件列表
set(SRC_LIST ./muduo_server.cpp)


#设置可执行文件最终存储的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)


#把制定路径下的源文件名字放入变量名src_list中
#aux_source_directory(. SRC_LIST)

#生成可执行文件server，有src_list变量所定义的源文件编译而来
add_executable(server ${SRC_LIST})

#表示server这个目标程序，需要muduo_net muduo_base pthread这三个库文件
target_link_libraries(server muduo_net muduo_base pthread)