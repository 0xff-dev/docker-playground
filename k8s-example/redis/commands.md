1. 生成redis-master的pods 
- `kubectl apply -f ./redis-master.yaml` // 生成 redis-master 的pod
- `kubectl get pods` //  查看目前的pods
- `kubectl logs -f redis-xxoo` 

2. 生成redis-master 的service
- `kubectl apply -f ./redis-master-svc.yaml`
- `kubectl get services`

3. 生成redis-slave pods
- `kubectl apply -f ./redis-slave.yaml`
- `kubectl get pods`

4. 生成redis-slave service
- `kubectl apply -f ./redis-slave-svc.yaml`
- `kubectl get services`

5. guestbook frontend
- `kubectl apply -f ./front-end.yaml`
- `kubectl get pods`

6. 创建front-end service
- `kubectl apply -f fron-end-svc.yaml`
- `kubectl get services`

7. front-ends-svc  对外服务
- `kubectl expose deployment front-end --port xx --targetPort xx --type NodePort/LoadBalancer`
