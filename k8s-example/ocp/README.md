这几个文件主要是处理针对权限过小的情况，无法使用hostPath的问题，正常在k8s集群中设置好psp即可，
当使用`opc`集群的时候需要设置`scc`权限(见scc.yaml), 这些权限可以被分配到ns下的某个`service account`
