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

6. 核心组件
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

- 需要了解知识
1. Master
> Nodes -> Master 每个Node都应该配置集群的公共根证书，以便安全的连接到api-server
> master组件通过非加密的端口与集群api-server通信。这个端口只在localshot报漏
> Master -> Node 从master到node有两条路径第一种是通过apiserver到node的kubelet进程。第二种是通过apiserver的代理功能从apiserver到任何node，pod和service

2. Master组件
   - Apiserver k8s如何接受请求以及如何返回结果给客户端
   > api-server 提供http和https两种rest api形式供使用，用户访问需要经历认证，授权，准入三个部分
   - etcd 主要功能机制
   > k-v 存储机制， 监听机制, k 的过期及续约机制，用于监控和服务发现，原子CAS和CAD，用于分布式锁和leader选举
   > leader选举方法： 初始启动节点处于follow状态，并设定一个eletion time，在这个时间段内没有发现来自leader的heartbeat，发起选举，自己变成candidate状态，向集群中其他followers发送请求，询问自己石是否可以为leader. 超过集群中一半的投票则称为leader，然后收集client的日志数据，向其他的followers同步日志。leader靠发送heartbeat保持地址。每一个新的leader都会比前一任任期+1. 
   > 日志复制
   > 当前leader收到客户端的日志，先把日志追加到本地的log中，然后通过heartbeat把entry同步给其他的followers，followers收到后返回ACK确认，leader收到大多数ack信息后将该日志设置为已提交并追加到本地磁盘中。通知客户端并在下一个heartbeat中，leader将通知所以的followers将日志存储在本地磁盘.
   > 安全性 
   > 安全性是用于保证每个节点都执行相同序列的安全机制，如当某个 Follower 在当前 Leader commit Log 时变得不可用了，稍后可能该 Follower 又会被选举为 Leader，这时新 Leader 可能会用新的 Log 覆盖先前已 committed 的 Log，这就是导致节点执行不同序列；Safety 就是用于保证选举出来的 Leader 一定包含先前 committed Log 的机制. 1⃣️选举安全性：每个任期只能选出一个leader。2⃣️Leader完整性： 指leader日志完整性，当 Log 在任期 Term1 被 Commit 后，那么以后任期 Term2、Term3… 等的 Leader 必须包含该 Log；Raft 在选举阶段就使用 Term 的判断用于保证完整性：当请求投票的该 Candidate 的 Term 较大或 Term 相同 Index 更大则投票，否则拒绝该请求。
   > 失效处理
   > 1) Leader 失效：其他没有收到 heartbeat 的节点会发起新的选举，而当 Leader 恢复后由于步进数小会自动成为 follower（日志也会被新 leader 的日志覆盖
   > 2) follower 节点不可用：follower 节点不可用的情况相对容易解决。因为集群中的日志内容始终是从 leader 节点同步的，只要这一节点再次加入集群时重新从 leader 节点处复制日志即可。
   > 3）多个 candidate：冲突后 candidate 将随机选择一个等待间隔（150ms ~ 300ms）再次发起投票，得到集群中半数以上 follower 接受的 candidate 将成为 leader
   - controller manager
   > 有kube-controller-manager和cloud-controller-manager组成，通过api-server架空整个集群状态。
   > kube-controller-manager必须启动的控制器
   > EndpointController, ReplicationController, PodGCController,ReousrceQuotaCOntroller, NamespacesController, ServieAccountController, GarbageCollectorController, DaemonSetController, JobController, DeploymentController, ReplicaSetontroller, HPAController, DisruptionController, StatefulSetController, CronJobController, CSRSigningController, CSRApprovingController, TTLController
   > 默认启动的可选控制器  
   > TokenController, NodeController, ServiceController, RouteController. PVBinderController, AttachDetachController.
   > 默认禁止的控制器
   > BootstrapSignerController, TokenCleanerController
   > `list-watch`原理
   > api-server作为统一入口，任何对数据操作都要经过api-server，list-watch可分为2个，list api和watch api。k8s的informer模块封装list-watch，informer首先通过list api罗列资源，然后调用watch api监听变更事件, 将结果放入到一个FIFO的队列，队列的另一段从协程去除事件，并调用注册的函数处理。watch监听机制的实现则是通过HTTP/1.1的`分块传输编码`
   - Scheduler
   > 监控api-server，将为分配的pod分配到node，有三种方式
   > `nodeSelector`: 只调度到匹配指定label的node上，`nodeAffinity` 功能更丰富的NOde选择器，支持集合操作。`podAffinity`调度到满足条件的Pod所在的 node上。
3. Node组件
   - Kubelet
   > 每个节点上都有一个kubelet服务进程，默认监听10250端口，接受并执行master发来的指令。管理Pod及pod中的容器
   > 向kubelet提供节点上需要运行的pod的清单
   > 文件：启动参数 --config只从的配置目录下的文件(default /etc/kubernetes/manifests),该文件每20秒重新检查一次，可配置
   > HTTP endpoint：启动参数 --manifest-url 设置，每20秒检查一次这个端点
   > api server: 通过api-server监听etcd目录，同步pod清单
   > http server: kubelet监听http请求，并响应简单的api
   - Pod清单管理
   > 向kubelet提供pod清单的方法。
   > 文件，启动参数 --config指定配置文件目录下的文件(默认是/etc/kuberbetes/manifests), 每20秒检查一下
   > HTTP endpoint， 启动参数 --manifest-url 设置每20秒检查以次
   > API  Server 通过api-server监听etcd，同步Pod清单
   > HTTP server kubelet侦听http请求，并响应api以提交新的pod
   - kubelet通过apiserver获取及创建Pod
   > kubelet通过api-server使用`watch`+`list`的方式去监听`/registry/nodes/{当前节点名}`,`/register/pods`将获取到的信息存储到本地。kubelet去监听etcd，所有的pod操作都会被kubelet看到，发现有新的pod绑定到本节点，按照清单创建pod。kubelet读取到监听信息，作如下处理
   1. 为pod创建数据目录
   2. 从 api-server读取该pod清单
   3. 为pod挂在外部卷
   4. 下载pod到secret
   5. 检查在节点上运行的pod，pod没有容器或者Pause容器没有启动，则先停止容器的进程，如果pod中有需要删除的容器，则删除这些容器
   6. 用kuberbetes/pause 镜像为每个容器创建一个Pause容器，Pause容器用来接管其他容器的网络。新建pod都会先创建一个Pause容器。
   
   - 容器运行时
   > 真正管理镜像和容器的生命周期

   - kube-proxy
   > kube-proxy支持以下几种形式的实现
   > userspace: 在用户空间监听一个端口，所有服务通过iptables转发到端口，由内部负载均衡转到实际pod。
   > iptables: 完全以iptables规则实现service的负载均衡，会产生太多的iptables的规则，


4. Autoscaling(HPA Horizontal Pod Autoscaling)
> 根据cpu的使用频率或者应用自定义 metrics 自动扩展Pod的数量
> 为容器配置CPU Request， HPA设置值恰当，70%给容器和应用，预留30%余量，保持Pod和Node健康。保证用户负载均衡。