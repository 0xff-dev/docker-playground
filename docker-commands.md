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
