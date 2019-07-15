## 概念

- `k8s` 主组件包含三个进程，在集群的某个节点上， 三个进程分别是`kube-apiserver`, `kube-controller-manager`, `kube-scheduler`
- 集群的非`master`节点都运行两个进程 `kubelet 和master进行通信`, `kube-proxy网络代理，将k8s的网路服务代理到每个节点上`,
### PaaS概念
1. `PaaS` 平台服务，将软件研发的平台作为一种服务，以`SaaS`模式提交给用户
2. `laas卖计算机硬件资源，PaaS是卖开发，运行环境，SaaS就是卖软件`

## kubernetes 对象(集群状态的描述或者期望)
基本的kubernetes对象包括:
- Pod (Pod是若干个相关容器的组合，是一个逻辑概念，Pod包含的容器运行在同一个宿主机上，这些容器使用相同的网络命名空间、IP地址和端口，相互之间能通过`localhost`来发现和通信，共享一块存储卷空间。在Kubernetes中创建、调度和管理的最小单位是Pod。一个Pod一般只放一个业务容器和一个用于统一网络管理的网络容器)
- Service(Service是真实应用服务的抽象，定义了Pod的逻辑集合和访问这个Pod集合的策略，Service将代理Pod对外表现为一个单一访问接口，外部不需要了解后端Pod如何运行，这给扩展或维护带来很大的好处，提供了一套简化的服务代理和发现机制)
- Volume(数据卷)
- Namespace(常用来隔离用户，k8s自带的服务一般运行在`kube-system` namespace中)

kubernetes 包含大量的`controller`的高级抽象，控制器基于对象构建，并提供额外的功能和方便的特性
- ReplicaSet
- Deployment(为Pod和ReplicaSet提供了声明式的定义， 主要应用在`定义 Deployment来创建Pod和ReplicaSet`, `滚动升级和回滚应用`,`扩容和缩容`,`暂停和继续Deployment`)
- StatefulSet
- DaemonSet
- Job
- Replication Controller (主要是用来控制管理Pod的副本， 确保任何时候k8s的集群中有指定数量的Pod副本在运行，如果少于指定数量的副本Replication Controller会启动新的Pod副本，多的会杀死。ReplicationController是实现弹性伸缩，滚动升级的核心)

K8s 基本概念
- Label (Label是用于区分Pod、Service、Replication Controller的Key/Value键值对，实际上Kubernetes中的任意API对象都可以通过Label进行标识。每个API对象可以有多个Label，但是每个Label的Key只能对应一个Value。Label是Service和Replication Controller运行的基础，它们都通过Label来关联Pod，相比于强绑定模型，这是一种非常好的松耦合关系。)
- Node (Kubernets属于主从的分布式集群架构，Kubernets Node（简称为Node，早期版本叫做Minion）运行并管理容器。Node作为Kubernetes的操作单元，将用来分配给Pod（或者说容器）进行绑定，Pod最终运行在Node上，Node可以认为是Pod的宿主机)

## K8s对象管理
> 一个k8s对象应该用一种技术去管理，混合使用技术去管理会导致未知的行为。

| Management technique | Operates On | Recommended environment | Suppoorted writers | Learning curve |
| :----: | :----: | :----: | :----: | :----: |
| Impreative command | Live objects | Development projects | 1+ | Lowest |
| Impreative object confuguration | individual files | Production projects | 1 | Moderate |
| Declarative object configuration | Directories files | Producetion projects | 1+ | Higest |
- Imperative commands 命令行,直接在集群中操作活的对象，用户提供参数
> kubectl run nginx --image nginx    kubectl create deployment nginx --image nginx
- Impreative object confuguration
> kubectl create -f nginx.yaml 通过配置文件创建
- Declarative object configuration
>  

## Kubernetes 对象构建讲解
每个`kubernetes`对象的结构描述都包括`spec`, `status`
- spec 有用户提供，描述用户期望的对象的特征及集群状态。
- status 有k8s集群提供和更新，描述k8s对象的实时状态。
在对象的描述文件中，必须包含一下字段
- apiVersion   k8s的api版本
- kind         k8s对象的类型
- metadata     唯一标识该对象的数据源，包括`name`, `UID`, 可选的`namespace`
- spec         描述对象的详细信息，不同的对象`spec`格式不同，可以套其他对象的字段

## k8s常用对象说明
1. Master
集群的控制节点，负责整个集群的控制，k8s的所有命令都发给Master，负责具体的执行过程。
2. Master组件
- kube-apiserver 资源增删改查的入口
- kube-controller-manager 资源对象的总管
- kube-scheduler 负责资源的调度
- etcd Server k8s的所有资源对象都保存在etcd中
3. Node
Node是集群的工作负载节点，默认情况下kubelet会想k8s注册自己，Node被纳入管理范围，kubelet会定时想master汇报情况，包括 操作系统，Docker版本，机器资源情况等。如果长时间不报告，被认为是失联，出发Pod的转移。
4. Node组件
- kubelet Pod的管家，与master通信
- kube-proxy 实现k8s Service的通信与负载平衡机制的重要组件
- Docker 容器的创建与管理
5. Node相关命令
- `kubectl get nodes` 
- `kubectl describe node {node_name}`
6. describe命令的Node信息
- Node基本信息： 名称，标签，创建时间等。
- Node当前的状态， node启动后会进行自检工作，磁盘是否满，内存是否不足，都正常切换为`ready`状态。
- Node的主机地址与主机名
- Node上的资源总量CPu，内存，最大可调度的Pod数量
- Node的可分配资源
- 主机信息， 主机的唯一标识UUID，内核版本，操作系统，k8s版本，kubelet和kube-proxy版本
- 正在运行的Pod的列表及概要信息
- 以分配的资源使用情况，
- NOde的Event信息。
7. Pod
Pod是k8s操作的基本单元，每个pod有一个根容器(Pause容器) pause的状态代表着整个容器组的状态，其他业务容器共享pause的ip，共享Pause挂在的volume，这样简化了同一个pod不同容器之间的网络和文件共享问题。
- k8s集群中，同宿主机的或不同宿主机的pod要能够进行TCP/IP直接通信，因此直接采用虚拟二层网络技术来实现。
- Pod有两种类型，普通类型和静态Pod， 静态Pod不通过k8s调度创建，直接在具体的Node机器上通过具体的文件来启动。普通的pod则是由k8s启动。存放在etcd中。

8. Label
- 是一个键值对
## 例子
1. Deployment 使用，定义nginx
```yaml
apiVersion: extensions/v1beta1
kind: Deployment
metadata:
  name: nginx-deployment
spec:
  replicas: 3
  template:
    metadata:
      labels:
        app: nginx
    spec:
      containers:
      - name: nginx
        image: nginx:1.7.9
        ports:
        - containerPort: 80
扩容: kubectl scale deployment nginx-deployment --replicas 10
如果支持横向pod自动扩容，设置自动扩容: kubectl autoscale deployment nginx-deployment --min=10 --max=15 --cpu-percent=80
更新镜像: kubectl set image deployment/nginx-deployment nginx=nginx:1.9.1
```