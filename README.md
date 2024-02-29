# chatserver
基于muduo实现的，使用nginx tcp负载均衡、redis、mysql等环境的集群聊天服务器源码

bin：存放服务器和客户端的可执行文件  
build：存放cmake所产生的一系列文件  
include：存放一系列头文件  
src：存放一系列源文件  
thirdparty：存放第三方库文件  

执行方式：  
1.先进入build目录，然后执行以下命令：  
rm -rf *  
cmake ..  
make  

2.然后再进入bin目录  
执行产生后的muduo_server 和 ChatClient可执行文件  
