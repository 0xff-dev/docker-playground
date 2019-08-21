# 第一阶段

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

9. k8s Deployment Operations
> 水平扩展和更新应用
> 通过调整Deployment的副本数量，将Pod的数量调整为4个，
```
kubectl scale deployment nginx --replicas=4 -n nameapsce 扩展,在扩展完成后，services会感知到pod，自动将endpointe扩展为4个
```

> 更新应用, k8s回启动一个新的pod，同时会停止一个老的pod，service会停止向老的pod导流
```
kubectl set image deployments nginx nginx=image_path -n namespace

kubectl rollout status deployment nginx -n namespace // 查询deployment的部署情况,
// 如果想回到前一个版本
kubectl rollout undo deploymennt nginx -n namespace // 将nginx返回到上一个版本
```

10. yaml/json 文件
> yaml文件分为五个部分，`apiVersionn`, `metadata`, `spec`, `status`, `kind`, 其中apiVersion表明当前k8s api的分组，kind表明当前操作的资源类型
> metadata是资源的元数据，对于美中资源都是固定的，例如资源的名字，namespace，label等。spec是用户对资源的说明书，对资源的各种配置信息，status是当前资源的状态，
> k8s 会尽最大努力使得spec和status匹配，
```yml
apiVersion: v1
kind: Pod
metadata:
  creationTimestamp: "2019-06-28T10:10:10Z"
  generateName: ngxin-12345
  labels: 
  name: ngxin-12345
  owneReferences:
  resourceVersion: "28590"
  selfLink: "/api/v1/namespaces/ns/pods/nginx-12345"
  uid: "1dffsdfdlkdf"
spec:
  containers:
  dnsPloicy: ClusterFirst
  enableServiceLinks: true
  nodeName: minikube
  priority: 0
  restartPolicy: Always
  schedulerName: default-scheduler
status:
  conditions:
  containerStatuses:
  hostIP: 10.10.10.10
  phase: Running
  podIP: 172.17.0.8
  qosClass: Brustable
  startTime: "2019-06-28T10:10:10Z"
```

11. yaml学习
> 通过-o yaml获取已经部署的资源的yaml文件
```
kubectl get pods pod_name -n namespace -o yaml
kubectl get svc svc_name nginx -n ns -o yaml
```

12. 从yaml创建资源
```
kubectl create -f yaml_path -n ns

// 更新yaml文件，并返回给k8s
kubectl apply -f yaml_path -n ns
```

13. k8s events
> k8s events 显示了集群中所有的事件，不同于资源，events是由k8s系统组件创建的，用来提示用户集群发生的各种事件，默认情况下集群事件由TTL，超过TTL事件会被删除
```
kubectl get events -n ns
```

14. Pod 生命周期
- Pod Phase
- Pod Condition
- Restart Ploicy
- Container probes

> `Restart Policy` 值Pod内容执行出错或者执行完毕是否要进行重启
> `Container probes` 分为两种，`LivenessProbe`和`ReadinessProbe`. `Liveness`检查应用是否依然健康无措，有错误则k8s会根据policy重启或者仅更新状态. `Readiness`
> 检查应用是否可以对外提供服务. 若检查不通过，则会把pod从资源池删除


15. configMap & Secret
> `ConfgiMap`是用来管理配置信息的资源类型，可以单独创建ConfigMap，在将ConfigMap挂载到Pod，实现配置和应用分离. ConfgiMap可以通过yaml来创建
```
kubectl create confgimap game-config --from-file=file_path --from-file=file_path -n ns

kubectl get configmap game-config -o wide -n ns

kubectl describe game-config -n ns

// 查看配置文件
kubectl get configmap game-config -n ns -o yaml
```
> secret 使用和ConfigMap相似，这是内容加密


# 第二阶段
1. k8s执行流程
> 通过Kubectl提交一个创建RC的请求，该请求通过APIServer被写入etcd中，此时Controller Manager通过
> API Server的监听资源变化的接口监听到这个RC事件，分析之后，发现当前集群中还没有它所对应的Pod实例，
> 于是根据RC里的Pod模板定义生成一个Pod对象，通过APIServer写入etcd，接下来，此事件被Scheduler发现，
> 它立即执行一个复杂的调度流程，为这个新Pod选定一个落户的Node，然后通过API Server讲这一结果写入到etcd中，
> 随后，目标Node上运行的Kubelet进程通过APIServer监测到这个“新生的”Pod，并按照它的定义，启动该Pod并
> 任劳任怨地负责它的下半生，直到Pod的生命结束。随后，我们通过Kubectl提交一个新的映射到该Pod的
> Service的创建请求，ControllerManager会通过Label标签查询到相关联的Pod实例，然后生成Service的
> Endpoints信息，并通过APIServer写入到etcd中，接下来，所有Node上运行的Proxy进程通过APIServer查询并监听
> Service对象与其对应的Endpoints信息，建立一个软件方式的负载均衡器来实现Service访问到后端Pod的流量转发功能。
> 在集群中`Master`节点由4个进程`api-server`, `controller manager`, `etcd`, `scheduler`
>APIServer: APIServer负责对外提供RESTful的Kubernetes API服务，
> 它是系统管理指令的统一入口，任何对资源进行增删改查的操作都要交给
> APIServer处理后再提交给etcd。如架构图中所示，kubectl（Kubernetes提供的客户端工具，
> 该工具内部就是对Kubernetes API的调用）是直接和APIServer交互的。

> schedule: scheduler的职责很明确，就是负责调度pod到合适的Node上。如果把scheduler看成一个黑匣子，
> 那么它的输入是pod和由多个Node组成的列表，输出是Pod和一个Node的绑定，即将这个pod部署到这个Node上。
> Kubernetes目前提供了调度算法，但是同样也保留了接口，用户可以根据自己的需求定义自己的调度算法。
> controller manager: 如果说APIServer做的是“前台”的工作的话，那controller manager就是负责“后台”的。
> 每个资源一般都对应有一个控制器，而controller manager就是负责管理这些控制器的。比如我们通过APIServer
> 创建一个pod，当这个pod创建成功后，APIServer的任务就算完成了。而后面保证Pod的状态始终和我们预期的一样
> 的重任就由controller manager去保证了。

> etcd: etcd是一个高可用的键值存储系统，Kubernetes使用它来存储各个资源的状态，从而实现了Restful的API。

> Node

>每个Node节点主要由三个模块组成：kubelet、kube-proxy、runtime。

> runtime。runtime指的是容器运行环境，目前Kubernetes支持docker和rkt两种容器。

> kube-proxy。该模块实现了Kubernetes中的服务发现和反向代理功能。反向代理方面：kube-proxy支持TCP和UDP连接转发，默认基于Round
> Robin算法将客户端流量转发到与service对应的一组后端pod。服务发现方面，
> kube-proxy使用etcd的watch机制，监控集群中service和endpoint对象数据的动态变化，并且维护一个service到endpoint的映射关系，
> 从而保证了后端pod的IP变化不会对访问者造成影响。另外kube-proxy还支持session affinity。

> kubelet。Kubelet是Master在每个Node节点上面的agent，是Node节点上面最重要的模块，它负责维护和管理该Node上面的所有容器，
> 但是如果容器不是通过Kubernetes创建的，它并不会管理。本质上，它负责使Pod得运行状态与期望的状态一致。

2. Pod的生命周期
> `Replication Conntroller (RC)` 是k8s的另一个核心概念应用托管在k8s后，RC要保证任何时间
> k8s中有指定数量的Pod在运行, 此外还有滚动升级，升级回滚等
> Pod的声明周期包括：通过模版定义，分配到一个Node上运行，就不会离开这个Node，直到被删除。Pod失败，会被k8s清理，重建后的Pod ID会发生变化，k8s中Pod的迁移实际是指在新的Node上重建Pod

3. Pod通信
```
同一个Node内，通过Veth连在同一个docker0网桥上，他们的IP是从docker0网桥上自动获取的，他们和网桥本身IP3在同一个网段

不同NOde的pod通信
对docker0的IP地址统一规划，对Pod的IP地址做统一的规划。

Pod到service的通信
Service的虚拟IP通过每个Node上的kube-proxy映射到不同的pod上，目前只支持轮训
```

4. Label 增删改
```
kubectl label pod pod_name role=backend // 添加label
kubectl label pod pod_name role- // 删除label
kubectl lable pod pod_name role=abc -overwrite // 更改一个label的值 
```

5. 创建pod的流程
```
1. 用户通过REST API 创建Pod
2. apiserver将其写入etcd中
```

6. questions
- Master 与 Node通信
> Nodes -> Master 每个Node都应该配置集群的公共根证书，以便安全的连接到api-server
> master组件通过非加密的端口与集群api-server通信。这个端口只在localshot报漏
> Master -> Node 从master到node有两条路径第一种是通过apiserver到node的kubelet进程。第二种是通过apiserver的代理功能从apiserver到任何node，pod和service

7. 核心组件
- etcd
```
主要功能:
基于k-v的存储
监听机制
k的过期及续约机制
原子CAS, CAD  用于分布式锁和leader的选举
```

- api-server
```
提供集群管理的REST API接口，包括认证授权，数据校验以及集群状态变更
提供其他模块之间数据交互和通信的枢纽， 其他模块通过api-server查询或者修改数据，只有ai-server去操作etcd

访问控制
1. 认证
开启TLS，所有的请求都需要先认证，k8s有多种认证机制，支持同时开启多个认证插件，认证成功用户名会传入授权模块，认证失败则返回401

2. 授权
认证之后到授权模块，k8s支持多种授权机制，支持开启多个授权插件，授权成功，怎会发送请求到准入控制模块，否则返回403

3. 准入控制

```

- scheduler
```
scheduler是调度pod到node节点上，该模块监听api-server，查询还未分配到node的pod
分配考虑的因素
1. 公平调度
2. 资源利用效率
3. Qos
4. affinity 和 anti-affinity
5. 数据本地化
6. 内部负载干扰
7. deadlines

指定Pod运行在指定的Node节点上
1. 给node打标签，在daemonset中设置
kubectl lable nodes node-01 disktype=ssd
spec:
  nodeSelector:
    disktype: ssd

2. nodeAffinity
nodeAffinity 目前支持两种 requiredDuringSchedulingIgnoreDurlingExecution,
preferredDurlingfSchedulingIgnoreDurlingExecution分别代表满足条件和优选条件

apiVersion: v1
kind: Pod
metadata:
  name: with-node-affinity
spec:
  affinity:
    nodeAffinity:
      requiredDuringSchedulingIgnoredDuringExecution:
        nodeSelectorTerms:
        - matchExpressions:
          - key: kubernetes.io/e2e-az-name
            operator: In
            values:
            - e2e-az1
            - e2e-az2
      preferredDuringSchedulingIgnoredDuringExecution:
      - weight: 1
        preference:
          matchExpressions:
          - key: another-node-label-key
            operator: In
            values:
            - another-node-label-value
  containers:
  - name: with-node-affinity
    image: gcr.io/google_containers/pause:2.0

表示调度到带有标签kubernetes.io/e2e-az-name 并且值为e2e-az1火e2e-az2的Node上，并且优选带有标签another-node-label-key=node-label-value


3. podAffinity, podAntiAffinity
如果一个"Node 所在的空间至少包含一个带有标签security=s1并且运行中的pod", 那么可以调度
不能调度到 "包含至少一个带有security并且运行中的Pod"的node上

4. Taints和tolerations
用于保证pod不被调度到不合适的node上，taint应用于node上，tolerations应用于pod上


5. 优先级调度
v1.11默认开启，其他版本配置
apiserver --feature-gates=PodPriority-true和 --runtime-config=scheduling.k8s.io/v1alpha1=true
scheduler 配置 --feature-gates=PodPriority=true

调度过程
分为predicate(过滤掉不符合条件的节点)和priority(优先级排序，选择优选级较高的节点)阶段
```

- controller-manager
```
controller 由kube-controller-manager和cloud-controller-manager组成，通过api-server监控集群的状态

Metrics
controller manager metrics提供了内部逻辑的性能度量，默认监听在10252端口，通过
http://localhost:10252/metrics 访问

高可用
启动时设置--leader-elect=true,controller manager会使用多节点选主的方式选择主节点，主节点调用StartControllers() 启动所有控制器，其他节点只是执行选取算法

高性能
```