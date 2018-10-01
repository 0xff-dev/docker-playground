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
>
4. docker logs 查看当前job的日志
5. docker start/stop/restart job\_name Start/Stop/Restart one or more running containers
6. 
## 小目标
1. 打一个hadoop及其乱七八糟的的docker
2. 打一个python dj docker
3. Anubis docker
