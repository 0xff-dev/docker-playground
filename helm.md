### basic command

1. add repo
```
helm add repo repo_name  url
```

2. search repo
```
helm search repo stable # 获得chart包的列表
```

3. find all releases
```
helm ls
```

4. install chart/ uninstall release
```
helm install stable/mysql     # custome define release name helm install release_name stable/chart_name    -f new_values.yaml # rewrite values.yaml
helm uninstall release_name 
```

4. release
```
helm ls
```

6. show chart valur
```
helm show values stable/chart_name #查看chart的value文件

7. show chart detail information
```
helm inspect repo/chart
```

8. 管理release
```
helm list 列出已经部署的release
helm delete release   删除一个release，出于审计需要，历史可查
helm status release   查看release信息，即使是delete的release
helm upgrade 升级某个release
helm rollback release version   release 回滚到指定的发布序列
helm get values release 查看release的配置文件
```

9. 创建一个chart包
```
Chart.yaml文件至少包含 name和version两个字段
定义templates目录，里面包含定义应用的基本资源包括deployment和service，sa等。

在根目录执行  helm install . 完成应用包的安装
helm status releasename 查看安装的信息，以及通过kubectl查看pod，deployment的信息
配置体现在Values.yaml文件，在文件中定义属性的值，然后在k8s资源定义的文件中使用变量
例如
Vaues.yaml
image:
  registory: gcr.io/google-samples/node-hello
  tag: '1.0'

deployment.yaml
spec:
  containers:
  - name: node
    image: {{ .Values.image.registory }}:{{ .Values.image.tag }}
```
10.执行预览而不进行部署
```
helm install . --dry-run --debug
```

11. helm chart与定义对象
```
.Release 是指代一次release的结果对象 .Release.Name
.Chart 配置文件 Chart.yaml
.Files chart目录下的文件
.Values 就是定义的应用启动的相关信息的文件，可以直接在这里引用
在templates目录下已__下划线开头的文件不会认为是k8s资源文件
在一个应用中，重读使用的变量较多，可以通过设置公用的变量来解决这个问题
例子在当前目录的chart文件夹下
template不是一个函数，他的输出都是靠右对齐，除第一行其它行都不会锁进，template不能用在pipeline中
helm提供了include函数，他的输出可以通过pipeline传递给其他的函数

checksum的使用，当一个pod使用configmap，在重建configmap同时重建pod
可以为pod增加一个注解，计算confgimap的256sum值。
```

