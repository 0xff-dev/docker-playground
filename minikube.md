## k8s 小案例(minikube)

1. 安装`docker`和`kubectl`
2. 下载minikube
3. 启动minikube `minikube start`
4. `minikube dashboard`
5. 创建pod `kubectl create deployment hello-node --image=gcr.io/hello-minikube-zero-install/hello-node`
6. 查看deploymeny， `kubectl get deployments`
7. 查看pods `kubectl get pod[s <name>]`
8. 查看集群的信息 `kubectl get events`
9. 创建service, 对外开放 `kubectl expose eploymeny hello-node --type=LoadBalancer --port=8080`
10. 使用`kubectl get services`得到services的信息。
11. minikube启动service `minikube service hello-node`
12. 添加`Enable`, `minikube addons list` 列出当前支持的addons
13. 为某个addons添加enable, `minikube addons enable heapster`例子
14. clean pod, `kubectl delete service hello-node, kubectl delete deployment hello-node`
15. 停止 `minikube stop`

## [k8s常用命令](http://docs.kubernetes.org.cn/683.html)


## 组件
- Master
- kube-apiserver 任何资源的请求和调用都通过该组件
- etcd (键值对集群，存储pod的状态)
- kube-controller-manager 运行管理控制器，集群中处理常规任务的后台线程。
- cloud-controller-manager 云控制器管理器与底层的云提供商交互。
- kube-scheduler 监控新创建没有分配到node的pod

## 命名空间
- `kubectl create/delete namespace/s new-namespace` 默认情况下，相同Namespace中的对象将具有相同的访问控制策略
- `default`, `kube-system`这两个不可以删除
- `kubectl get namespaces` 获取命名空间
- 临时设置Request的命名空间 `kubectl --namespace=namespace-name run nginx --image=nginx`
- 大多数Kubernetes资源（例如pod、services、replication controllers或其他）都在某些Namespace中，但Namespace资源本身并不在Namespace中。而低级别资源（如Node和persistentVolumes）不在任何Namespace中。Events是一个例外：它们可能有也可能没有Namespace，具体取决于Events的对象。

## Labels和selector
> `lable` 关联到对象上
```
{
  "labels": {
    "k1": "v1",
    "k2": "v2"
  }
}
```
