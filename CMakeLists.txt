cmake_minimum_required(VERSION 3.5) #3.5以下版本将会在未来被移除

#项目名称，根据自身来定
project(muduo_server) 

#配置编译选项
#在原有CMAKE_CXX_FLAGS变量的值基础上，加上-g，添加调试功能
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)  

# 设置可执行文件最终存储的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) #PROJECT_SOURCE_DIR 是CMakeLists.txt文件所在目录

#配置头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)

#加载子目录,也就是去编译src目录下的CMakeLists.txt
add_subdirectory(src)

#配置库文件搜索路径
#link_directories()

#生成可执行文件,方法1
#add_executable(muduo_server muduo_server.cpp)

#生成可执行文件,方法2
##设置需要编译的源文件列表
# set(SRC_LIST ./muduo_server.cpp)
##可以把 . 指定路径下的所有源文件名字放入变量名SRC_LIST里面
#aux_source_directory(. SRC_LIST)


#用SRC_LIST所指向的cpp文件，生成muduo_server可执行文件
# add_executable(muduo_server ${SRC_LIST})
# muduo_server可执行文件，需要连接muduo_net muduo_base pthread这三个库文件
# target_link_libraries(muduo_server muduo_net muduo_base pthread)


