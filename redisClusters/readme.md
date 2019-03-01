## 使用docker搭建redis的集群, 使用Haporxy做负载均衡. 部署简单web应用
### 默认是root用户的操作

1. Docker安装
>  [MacStable](https://download.docker.com/mac/stable/Docker.dmg)  
>  [Ubuntu](https://docs.docker.com/install/linux/docker-ce/ubuntu/)

2. 了解必要的docker命令
* `docker ps options` 列出本地构建的容器
* `docker attach` 进入容器
* `docker start/stop/restart container` 启动/停止/重启 容器
* `docker run ` 根据一个镜像构造容器, 主要的参数 `--link name:alias` 与那个容器通信　`-v` 数据卷的挂载

3. 拉取必要的镜像
```bash
docker pull ubuntu
docker pull django
docker pull redis
docker pull haproxy
```

4. 构造容器
```bash
# 注意启动顺序
docker run -it --name redis-master redis /bin/bash
docker run -it --name redis-slave1 --link redis-master:master redis /bin/bash
docker run -it --name redis-slave2 --link redis-master:master redis /bin/bash
docker run -it --name app1 --link redis-master:db -v ~/project/django/app1:/usr/src/app1 django /bin/bash
docker run -it --name app2 --link redis-master:db -v ~/project/django/app2:/usr/src/app2 django /bin/bash
dockrt run -it --name HAProxy --link app1:app1 --link app2:app2 -p 6301:6301 -v ~/project/hap:/tmp /bin/bash
```

5. 查看redis的默认数据挂载点
`docker inspect container(name.,id) | grep Source`