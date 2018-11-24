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
