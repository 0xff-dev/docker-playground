k8s 启动流程

# kube-apiserver
1. 认证
```
每次收到请求，api-server都会通过令牌链进行认证，知道某个成功
- x509处理程序 将验证http请求是否由CA根证书签名的TLS密钥编码的
- bearer token将验证--toke-auth-file参考提供的token文件是否存在 
- 基本认证处理程序确保http请求的基本认证凭证与本地的状态匹配
```
2. 授权
```
kube-apiserver通过设置启动参数 authorization_mode组合一系列的授权这，所有的授权者都拒绝，那么请求将会停止
目前主持的几种授权方法
- webhook 与集群外的服务交互
- ABAC 执行静态文件汇总定义的策略
- RBAC 使用rbac.authorization.k8s.io API Group实现授权决策，允许管理员通过API动态配置策略
- Node 确保kubelet只能访问自己节点上的资源.
```