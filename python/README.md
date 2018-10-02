# root用户, 非root,加sudo或者切换root

## 使用基本命令

```bash
docker build -t image_name .           # Create image useing this directory's Dockerfile
docker run -p 4000:80 image_name       # Run image_name mapping port 4000 to 80
docker run -d -p 4000:80 image_name    # In detached mode
docker container ls                    # List all 
docker container ls --all              # List all containers, even not running
docker container stop/kill <hash>      # Stop/shutdown speicfied container
docker container rm <hash>             # Remove container

docker tag <image> username/repository:tag  # Tag <image> for upload to registry
docker push username/repository:tag         # Upload tagged to registry
docker pull username/repository:tag         # Pull image
dockrt run username/repository:tag          # Run iamge from a registry
```

## 操作
```bash
docker run -d -p 4000:80 stevenshuang/learn-docker:tag <返回一个容器的id>
curl http://localhost:4000/   会返回Hello World
docker container stop <hash id> 如果不知道, docker container ls找到对应的id
```