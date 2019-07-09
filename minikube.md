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