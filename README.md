# docker-playground


# 知识点
### 基本名词
* 镜像
* 容器&nbsp;&nbsp;&nbsp;&nbsp;从镜像中构建容器，等同 从快照里构建虚拟机
* 数据卷&nbsp;&nbsp;&nbsp;&nbsp;不受容器生命周期的限制，进行数据持久化,在容器内占用空间, 实际保存在容器之外. 容器之间共享数据.
* 链接&nbsp;&nbsp;&nbsp;&nbsp;创建一个新的容器的时候，引用别的容器，被引用的容器获得别名，则两个容器链接在一起

### Docker 组件
* docker client 支持用户与docker daemon通信
* docker Daemon 运行在主机上，处理服务请求
* docker index 中央registry，支持拥有共有和私有访问权限的Docker容器镜像的备份

### Docker要素
* Docker Containers 负责程序的运行, 包括操作系统，用户添加的文件以及元素数
* Docker images 只读模板，运行docker的容器
* Dockerfile 文件的指令集，如何自动构建docker镜像

### 基本的Docker命令
1. docker info
2. docker pull image
3. docker run xx
> -d 后台运行
4. docker logs 查看当前job的日志
5. docker start/stop/restart/rm job\_name Start/Stop/Restart/reomove one or more running containers
6. docker commit $container imgae_name 将容器保存为镜像 [a-z0-9]
7. docker search/history image_name 查找镜像/查看镜像的历史版本
8. docker build [options] PATH|URL
> --rm=true   构建成功后，移除所有中间容器
> --no-cache=true 不使用缓存
9. docker attach container 与运行中的容器进行交互 
10. docker diff 列出容器内发生变化的文件目录,
11. docker events  打印指定时间内的容器实时系统事件
12. docker import/export 导入/导出
13. docker cp container:file_path hostpath
14. docker login 登录
15. docker inspect 收集容器和镜像的底层信息(容器实例的ip，端口绑定列表，特定端口映射搜索, 收集配置信息)
16. docker kill  发送SIGKILL 停止容器的主进程
17. docker rmi 删除一个或者多个镜像
18. docker wait 
19. docker load/save  从tar文件在如镜像, 保存镜像到tar文件

### 学写Dockerfile
1. Dockerfile以FROM 命令开始，指定使用的基础镜像
  * FROM <image_name>   FROM ubuntu  
  * MAINTAINER <auther_name>  设置该镜像的作者  
  * RUN <command>    在shell/exec环境下执行的命令  
  * ADD <src>  <destination>  复制文件指令, 两个参数<source>这个可以是url或者启动配置上下文中的一个文件， <destination>容器内的路径  
  * CMD 提供容器的默认执行命令, Dockerfile只允许使用一次CMD, 多个CMD只执行最后一个.三种格式CMD ["executable", "param1", "param2"]  CMD ["param1", "param2"] CMD command param1 param2  
  * EXPOSE 指定人哦更年期在运行时候，监听的端口
  * EXTRYPOINT(只允许有一个) 给容器配置一个可以执行的命令, 每次使用镜像创建容器的时候，一个特定的应用程序被设置为默认的程序，该镜像在被调用时候，仅能运行指定的应用程序.
  * WORKDIR RUN, CMD, ENTRYPOINT,等命令的工作目录.
  * ENV <key> <value>设置环境变量, 使用键值对, 
  * USER <uid> 镜像正在运行设置一个UID
  * VOLUME 授权访问从容器内到主机上的目录  VOLUME ['/data']
2. 写Dockerfile注意事项
  * 保持常见的指令, MAINTAINER, 以及从上之下更新Dockerfile命令
  * 构建镜像时，使用可以理解的标签
  * 避免在Dockerfile中映射公有的端口
  * CMD,ENTRYPOINT 使用数组语法

## 小目标
1. 打一个hadoop及其乱七八糟的的docker
2. 打一个python dj docker
3. Anubis docker
