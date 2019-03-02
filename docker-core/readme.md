[TOC]
# docker内核知识 

## namespace隔离资源
> linux内核提供6中namespace隔离系统调用

| namespace | 系统调用参数 | 隔离内容 |
| --- | --- | ---- |
| UTS | CLONE_NEWUTS | 主机与域名 |
| IPC | CLONE_NEWIPC | 信号量, 消息队列和内存共享 |
| PID | CLONE_NEWPID | 进程编号 |
| Network | CLONE_NEWNET | 网络设备, 网络栈, 端口等 |
| Mount | CLONE_NEWNS | 挂载点 |
| User | CLONE_NEWUSER | 用户和用户组 |

### 进行namespace API操作的四种方式
* clone()在创建进程的同时, 创建namespace`int clone(int (*child_func)(void *), void *child_stack, int flags, void *args)`
* 查看 `/proc/[pid]/ns`文件
> 如果两个进程的namespace编号一样, 那么就是 同一个namespace下面, /proc/[pid]/ns设置这些link的另外作用, 上述的link被打开, 文件的描述符fd存在, 就算namespace下面的全部进程都结束, namespace还会存在, 其他的进程可以加进来.
* 通过`setns()` 加入一个已经存在namespace
> 在docker中, 想要在已经运行的容器执行命令, 需要系方法. setns调用,  进程从原来的namespace加入一个已经存在的namespace, 为了不影响加入的namespace进程的调度者, 在`setns`执行后,使用`clone`, 创建子进程执行命令, 让原来的进程结束.
* 通过`unshare`在原来的进程隔离, 不启用新的进程.

### UTS namespace
```c
#define _GNU_SOURCE 

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)

static char child_stack[STACK_SIZE];

char* const child_args[] = {"/bin/bash", NULL};

int child_main(void *args) {
    printf("在子进程中!\n");
    sethostname("NewHostName", 12);
    execv(child_args[0], child_args);
    return 1;
}

// UTS命名空间
int main() {
    printf("程序开始: \n");
    int child_pid = clone(child_main, child_stack+STACK_SIZE, CLONE_NEWUTS|SIGCHLD, NULL);
    waitpid(child_pid, NULL, 0);
    printf("程序退出\n");
    return 0;
}
```

### IPC namespace
```c
#define _GNU_SOURCE 

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)

static char child_stack[STACK_SIZE];

char* const child_args[] = {"/bin/bash", NULL};

int child_main(void *args) {
    printf("在子进程中!\n");
    sethostname("Newhostname", 12);
    execv(child_args[0], child_args);
    return 1;
}

/*
    ipcmk -Q //创建一个message queue
    ipcs -q  //查询message queue
    进入子进程后, 在查询, 找不到目标queue.
    进程通信隔离
*/
int main() {
    printf("程序开始: \n");
    int child_pid = clone(child_main, child_stack+STACK_SIZE, CLONE_NEWIPC|CLONE_NEWUTS|SIGCHLD, NULL);
    waitpid(child_pid, NULL, 0);
    printf("程序退出\n");
    return 0;
}
```

* bash只在登陆的时候读取一次uts


### PID namespace
```c
#define _GNU_SOURCE 

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)

static char child_stack[STACK_SIZE];

char* const child_args[] = {"/bin/bash", NULL};

int child_main(void *args) {
    printf("在子进程中!\n");
    sethostname("Newhostname", 12);
    execv(child_args[0], child_args);
    return 1;
}

/*
    ipcmk -Q //创建一个message queue
    ipcs -q  //查询message queue
    进入子进程后, 在查询, 找不到目标queue.
    进程通信隔离

    echo $$ 在主进程看一下, 执行代码后在echo $$会发现1.
*/
int main() {
    printf("程序开始: \n");
    int child_pid = clone(child_main, child_stack+STACK_SIZE, CLONE_NEWPID|CLONE_NEWIPC|CLONE_NEWUTS|SIGCHLD, NULL);
    waitpid(child_pid, NULL, 0);
    printf("程序退出\n");
    return 0;
}
```

* 以树的形式管理pid namespace
* 每个pid namespace第一个进程都是init, 起特殊作用. (管理孤儿)
* namespace中的进程, 不能kill或者ptrace父进程或兄弟进程.
* init进程可以屏蔽信号, 在函数中不处理信号, 就直接屏蔽, `防止init被误杀`. 父进程发送的信号, 不是`SIGKILL`, `SIGSTOP`, 都可以忽略.