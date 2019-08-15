1. `kubectl version`
2. 命令格式: `kubectl <action> <resource>`, `action`一般包括`create, delete,get`,`resource`可以通过`kubectl api-resources`查看
3. `kubectl`通过读取配置文件与集群交互，默认路径`~/.kube/config`
```yml
apiVersion: v1
clusters:
- cluster:
    certificate-authority: /Users/deyuandeng/.minikube/ca.crt
    server: https://192.168.99.100:8443
  name: minikube
contexts:
- context:
    cluster: minikube
    user: minikube
  name: minikube
current-context: minikube
kind: Config
preferences: {}
users:
- name: minikube
  user:
    client-certificate: /Users/deyuandeng/.minikube/apiserver.crt
    client-key: /Users/deyuandeng/.minikube/apiserver.key
```
> 三个顶级概念:`clusters`, `users`, `contexts`

4. k8s Node
> 节点是物理机或者虚拟机，他们组成了k8s的资源池。Master负责资源调度，集群状态控制等。Node负责运行用户应用，承接负载
- 获取所有的节点信息
```
kubectl get nodes
Master节点不会运行用户容器，如果主机数量非常少，可以考虑使用master，前提是给master预留足够的空间
, 不用承载用户的容器的节点status需要加上SchedulingDisabled. kubectl cordon <node name> 命令实现
```
- 获取节点详细信息
```
kubectl describe nodes <node name>
```

5. k8s Namespace
> k8s namespace用来构建虚拟的资源池. 使用ns管理员可以将k8s划分为多个虚拟的区域，不同的项目或者团队使用不同的namespace，达到了共享
> k8s的目的。此外在不同namespace里资源可以取相同的名字，相同ns下的资源不可以相同
```
kubectl create -f file_path 创建一个ns
kubectl get ns 查看命名空间
k8s在启动的说话有一个默认的ns defult，如果在创建资源的说话不指定ns，都是在default

kubectl describe ns <ns name> 命名空间详细信息
```

6. k8s Pod & Deployment
> Pod是运行应用的载体。Pod由多个容器组成，是k8s的最小调度单元，Pod共享底层的资源，由k8s来管理生命周期。一般情况下是由 Deployment来创建Pod，Deployment创建，更新
> 维护其管理的所有的Pods
```
例子
kubectl run nginx --image=nginx_image --replicas=1 -n namespace_name //创建nginx的deployment
kubectl get deployments -n namespace_name//查看创建的deployment
kubectl get pods -n namespace_name // 查看pods -o wide 参数查看Pod运行的主机
kubectl logs pod-name -n namespace_name // 查看pod 的日志信息
// 登陆集群进行操作
minikube ssh // 在执行其他的操作

// 在pods里执行命令
kubectl exec pods_name -n namespace_name -- command (-- 用来区分本地终端命令和容器里执行的命令, 命令只有一个单词的时候可以省略)
kubectl exec pods_name -n namespace_name bash // 容器有终端的情况， 启动终端
```

7. k8s Services
- 提供固定的IP，由于Pod可以随时停止启动，Pod的IP随时变化，`Service` 为Pods提供固定的IP，其他的服务可以通过Service Ip找到提供服务的pods
- 提供负载均衡, Service由多个Pods组成，k8s对组成Service的Pods提供堵在均衡方案。随机访问等。
- 服务发现，集群中其他的服务可以通过service的名字访问后端服务。也可以通过环境变量访问。

- 创建service
```
可以通过kubectl create -f 创建，deployment更快捷的方式
kubectl expose deployment nginx(deployment_name) --port 80 -n namespace
创建一个nginx的service，端口使用80，如果想要外部暴露的非80，使用如下命令
kubectl expose deployment nginx --port 8080 --target-port 80 -n namespace_name
外部8080映射到80端口

kubectl get svc nginx -n namespace_name 查看ns下面的名字=nginx的service的信息

kubectl describe svc nginx -n namespace_name 
Name:              nginx
Namespace:         tutorial
Labels:            run=nginx
Annotations:       <none>
Selector:          run=nginx
Type:              ClusterIP
IP:                10.96.6.136
Port:              <unset>  80/TCP
TargetPort:        80/TCP
Endpoints:         172.17.0.11:80
Session Affinity:  None
Events:            <none>
其中Endpoints 表明Service选中的PodIP:PodPort
```

- NodePort Service
```
上述创建的service只能被集群内部的节点和Pod访问，可以通过NodePort和LoadBalancer两种形式暴露服务
kubectl expose deployment nginx --port 80 --type NodePort -n namespace_name
kubectl get svc nginx -n ns 查看暴露的端口
```

8. k8s Label
> Service通过`selector & label`选取他所管理的Pod，Deployment也是通过`selector & label`管理Pod，r如果让service 选择与deployment相同的pod，那么需要将service的selector与deployment相同label可以在创建的时候添加，也可以在运行时添加或修改。运行时修改会影响集群的状态，selector和label的结构会发生改变。

- Label operations
> kubectl支持对资源的labels进行管理，通过-l选项查看近具有某个label的资源
```
kubectl get pods -l run=nginx -n namespace_name // 获取有run=nginx label的pods
kubectl get svc -l run=nginx -n namespace_name  // 获取svc的selector为run=nginx 的svc
给pod增加label
kubectl label pod pod_name label_name -n namespace
```
