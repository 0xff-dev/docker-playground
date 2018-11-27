### 注意不要多写RUN,层数会很多,多的命令直接在一行写完
### 盲点记录
1. dockerfile 构建分层存储，每一个RUN都是启动一个容器


## 正经操作
1. COPY
> COPY 命令将从上下文环境的**源文件** 复制到新的一层镜像内的**目标路径**,目标路径可以是绝对路径，或者相对WORKDIR的相对路径,  源文件的各种属性都会保留，读，写，执行，文件的变更时间 COPY --chown=username:group files path
```docker
COPY package.json /app
```
2. ADD
> 更高级的复制文件，源文件可以是URL, 并且默认的权限是600，当要把压缩文件解压到目标路径，用ADD, 其他的推荐COPY  
```docker
ADD --chown=usernmae:group files path
```

3. CMD 推荐的格式 CMD ["command", "param", "param", ...] 
> 启动应用程序就是容器应用进程, 容器就是为了主进程存在，主进程退出，容器没有存在的意义，也将退出。所以，在容器里，程序应该以前台的形式启动，而容器本身可以后台启动
```docker
CMD ["nginx", "-g", "daemon off;"]
```

4. ENTRYPOINT (将容器做成命令行的格式)
> 第一种dockfile使用的是CMD形似的命令，在运行容器的时候，在镜像后面跟的是命令，第二种ENTRYPOINT是可以跟参数的, CMD会变成ENTRYPOINT的参数  
> 应用运行前的准备
```dockerfile
FROM ubuntu:16.04
RUN apt-get udpate \
    && apt-get install -y curl \
    && rm -rf /var/lib/apt/lists/*
CMD ["curl", "-s", "https://ip.cn"]


FROM ubuntu:16.04
RUN apt-get update \
    && apt-get install -y curl \
    && rm -rf /var/lib/apt/lists/*
ENTRYPOINT ["curl", "-s", "https://ip.cn"]
```

5. ENV 设置环境变量
> 对版本升级以及一些别的操作有力，方便维护
```dockerfile
ENV UBUNTU_VERSION 16.04
```

6. ARG
> 构建参数和ENV效果一样, ARG构建的环境变量在将来容器运行时，不会存在

7. VOLUME
> 指定匿名卷，保证不响容器的存储型写入大量数据
```dockerfile
VOLUME /data
```
> /data自动挂载为匿名卷, 任何向/data写入的数据都不会进入容器的存储层, 保证了容器存储层的无状态变化

8. EXPOSE
> 容器运行时，暴露的端口，宿主机器和容器的端口映射
```dockerfile
FROM nginx
RUN echo "hello" > /var/lib/nginx/html/index.html
EXPOSE 80

docker container run -p 8000:80 -it imagename
```

9. WORKDIR
> 设定工作目录，写dockerfile不要当成shell脚本来写，每个RUN都启动一个容器  
很可能你操作了，得不到想要的结果， 
错误示范
```dockerfile
WORKDIR /app
RUN cd /app
RUN echo "Hello" > text.txt
```

10. USER <用户名>[:<用户组>]
> 改变当前环境状态，并且影响以后的层， 改变RUN, CMD, ENTRYPOINT的执行者的身份  
> 推荐使用gosu切换特定的用户去执行某些特定任务

11. ONBUILD
> 后面跟随的是其他的指令 RUN, COPY等. 这些命令在构建当前镜像的时候，不会执行，去构建下一级的时候才会执行
```dockerfile
FROM node:slim
RUN mkdir /app
WORKDIR /app
ONBUILD COPY ./.package.json /app
ONBUILD RUN ["npm", "install"]
ONBUILD COPY . /app
CMD ["npm", "start"]

# 次级构建
FROM image-name
```

# 操作容器
1. 启动容器
> 启动容器有两种情况, 用镜像构建一个容器，把停止的容器重新启动
```bash
docker run -it image_name:tag 
-t 为docke分配一个伪终端, -i 容器的标准输入保持打开

docker container start/stop container_id 启动/停止一个存在的容器
```

2. 守护态运行
> 让容器在后台运行 加 -d 参数

3. 进入容器
> docker attach 在stdin中执行exit，容器也会停止  
> docker exec -it container\_id bash 进入容器, 带有一个伪终端

4. 导入导出容器
```bash
docker export container_id > file.tar 将容器快照导出到本地
docker save image_name > file.tar 持久化镜像
从快照导入镜像
docker import URL/filtar image_name
docker load < file.tar

```
> docker import和load 的区别, import 可以重新指定镜像的名字, load不可以

5. 删除容器
```bash
docker container rm <container id>
docker container rm -f <container id> 删除运行中的容器
docker container prune 清除所有终止的容器
```
6. 推向DockerHub
```bash
docker login
docker tag image_name:tag username/image_name:tag
docker logout
```

# 数据卷
* 数据卷 可以在容器之间共享和重用

* 对 数据卷 的修改会立马生效

* 对 数据卷 的更新，不会影响镜像

* 数据卷 默认会一直存在，即使容器被删除

```bash
docker volume create volume_name 创建数据卷
docker volume inspect volume_name 查看数据卷的结构
```
> 启动一个挂在数据卷的容器, target是数据卷的挂在位置
```bash
docker run -d -p \
       --name we \
       --mount source=my-vol,targer=/webapp \
       training/webapp
       python app.py

docker inspect web 查看web容器的信息(包含volume的信息)
docker volume rn my-vol 删除数据卷
docker volume prune 去掉无主的数据卷
```

# docker-compose
> 命令参数说明
1. -f, --file 指定Compose模版文件,默认是docker-compose.yml
2. -p --project-name NAME 指定项目的名字, 默认是所在的目录,作为项目的名字
3. --x-networking 使用 Docker 的可拔插网络后端特性
4. --x-network-driver DRIVER 指定网络后端的驱动，默认为 bridge
5. --verbose 输出更过的信息
6. -v, --version打印版本,并退出

> 命令说明
1. build ```docker-compose build [options] [services]``` (重新构建项目中的服务容器), 可以随时在项目目录下进行build
重构的参数
```bash
--force-rm 删除构建过程的临时容器
--no-cache 构建过程不实用cache(这将家常构建时间)
--pull 尝试pull拉取最新的镜像
```
2. config 验证docker-compose.yml文件格式是否正确
3. down 停止up启动的容器并移除网络
4. exec 进入指定的容器
5. images 列出项目使用的镜像
6. kill 格式为 docker-compose kill [options] [SERVICE...] 通过发送信号,强制停止服务容器
7. psuse 暂停一个服务容器
8. ps 列出当前服务的全部容器
9. push 推送项目的依赖的到Docker仓库
10. restart 重启项目中的服务 -t  重启超时设置
11. rm 删除所有停止状态的服务容器, 建议先执行docker-compose stop, 
参数说明
-f, --force 强制删除
-v 删除容器所挂在的数据卷
12. run 指定服务上执行一个命令, 默认情况下,如果容器有依赖,也会启动 (--no-deps 关闭依赖的自动启动)
```bash
docker-compose run ubuntu curl www.google.com
```

# Compose 模版文件
1. 注意每个服务都必须通过 image 指令指定镜像或 build 指令（需要 Dockerfile）等来自动构建生成镜像。
2. build参数
```yaml
version: "3"
services:
  webapp:
    build: . // 路径是docker-compose.yml或者绝对路径
    //或者用context: folder_dockderfile, dockerfile: dockerfile_name
```
