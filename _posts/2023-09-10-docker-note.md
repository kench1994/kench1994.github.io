---
layout: post
title:  Docker工作笔记
date:   2023-09-10 17:21:50
categories: docker
tags: docker
---

* content
{:toc}


#### 基础术语
名词 | 释义
| :----- | :-------| 
**寄存服务(registry)** | 负责托管和发布镜像的服务，默认为Docker Hub
**镜像(image)** | 镜像的概念更多偏向于一个环境包，这个环境包可以移动到任意的Docker平台中去运行。你也可以认为镜像与你装载操作系统iso镜像是一个概念
**容器(container)** | 容器是镜像运行时的实体（类似于面向对象中类的实例）。容器可以被创建、启动、停止、删除、暂停等。
**仓库(repository)** | 一组相关镜像（通常是一个应用或服务的不同版本）的集合
**标签(tag)** | 仓库中镜像的识别号，由英文和数字组成（如5.7或stable）

#### Docker系统架构
![docdocker_structureker](./assert/docker_structure.png)
- 图的中央是 **Docker 守护进程**， 它负责容器的创建、 运行和监控， 还负责镜像的构建和储存。Docker 守护进程通过docker daemon命令启动，一般会交由主机的操作系统负责执行。
- **Docker客户端**在图的左边， 它通过HTTP 与Docker 守护进程通信。 
- **Docker寄存服务**负责储存和发布镜像。默认的寄存服务为Docker Hub, 它托管了数以千计的公共镜像, 以及由其负责把关的 “官方” 镜像。 许多组织会搭建自己的寄存服务器，用于储存商业用途和机密的镜像，这样做还可以节省从互联网下载镜像所浪费的时间。

#### Docker安装（以centos7 为例，其他参考[菜鸟教程](https://www.runoob.com/docker/)）
> Docker 支持CentOS 7 64位 及以上版本进行安装。
- 安装所需的软件包。
 ```shell
sudo yum install -y yum-utils \
  device-mapper-persistent-data \
  lvm2 docker-ce docker-ce-cli containerd.io
 ```
- 启动 Docker。
 ```shell
sudo systemctl start docker
 ```
- 或者设定Docker开机启动
 ```shell
sudo systemctl enable docker
 ```

#### 容器状态
容器可以处于以下几种状态之一
- 已创建（created)    容器通过create命令初始化，但未曾启动
- 已退出（exited)     相较于已创建的容器，已退出的容器至少启动过一次
- 重启中（restarting）实际上很少遇见，当docker引擎尝试重启一个启动失败的容器时才会出现
- 运行中（running）   
- 已暂停（paused)


#### Docker 常用命令
> Docker 客户端非常简单，我们可以直接输入 ``docker`` 命令来查看到 Docker 客户端的所有命令选项。
或者输入 ``docker COMMAND --help `` 来获取指定command更详细的帮助。比如：
 ```shell
$ docker images --help

Usage:  docker images [OPTIONS] [REPOSITORY[:TAG]]
List images
Options:
  -a, --all             Show all images (default hides intermediate images)
      --digests         Show digests
  -f, --filter filter   Filter output based on conditions provided
      --format string   Pretty-print images using a Go template
      --no-trunc        Don't truncate output
  -q, --quiet           Only show image IDs
 ```

#### 获取基础镜像
我们可以通过[Docker Hub](https://hub.docker.com/)搜索我们所需要的基础镜像，并拉取到本地，通常拉取docker官方提供的基础镜像。

 ```shell
docker pull mysql
 ```
或者拉去指定版本的镜像
 ```shell
docker pull mysql:5.7
 ```
> 如果不指定```:```后的 ```tag```，直接拉取的就是```latest```的版本(确实存在```latest标签```镜像的时候，否则报错)。
> 很多仓库都会把 ```latest ```标签作为最新的稳定版本镜像别名，但这不过是一个惯例，而非严格标准

#### 启动容器
以下命令使用 mysql 镜像启动一个容器，参数为以命令行模式进入该容器：
 ```shell
docker run \
    -p 3306:3306 \
    -v /opt/mysql/conf:/etc/mysql \
    -v /opt/mysql/logs:/var/log/mysql \
    -v /opt/mysql/data:/var/lib/mysql \
    -e MYSQL_ROOT_PASSWORD=tipray \
    -d mysql:5.7 \
    --rm \
    --network=host \
    --dns=8.8.8.8 \
    --name mysql 
 ```
**参数说明**：
- i: 进入容器
- t: 分配一个终端
- d: 后台运行
- --name hellomysql 设定容器实例名为hellomysql，非必要参数
- mysql:5.7: mysql:5.7 镜像。如果镜像不存在于本地仓库，会先行尝试从docker hub进行拉取。
- -p <IP><宿主机端口>:<容器端口> 映射宿主机端口和容器端口
- -v <宿主机目录>:<容器目录><权限(ro/rw)> 将宿主机目录挂载到容器里，比如将容器内 ```mysql``` 默认数据路径 ```/var/lib/mysql``` 持久化到本地 ```/opt/mysql/data```
- -e 设置容器的环境变量
- --network=host 使用宿主机网路
- --dns=8.8.8.8 设置dns
- --rm命令，让容器在退出时，自动清除挂载的卷
- /bin/bash：放在镜像名后的是命令，表示有个交互式 Shell，因此用的是 /bin/bash。
> 进入容器后要退出终端，直接输入 exit

#### 启停删除容器
查看所有的容器命令如下：
 ```shell
(base) ➜  ~ docker ps -a
CONTAINER ID   IMAGE                COMMAND                  CREATED          STATUS                     PORTS     NAMES
c14b4ce19b12   mysql                "docker-entrypoint.s…"   19 minutes ago   Exited (0) 6 seconds ago             ecstatic_ganguly
1866ec78c313   my/timezone:0.2   "/bin/sh -c 'echo 't…"   2 days ago       Up 2 days                            timezone
 ```
使用 ```docker start ```启动一个已停止的容器：
 ```shell
docker start 1866ec78c313 
 ```
使用 ```docker stop ```停止一个已启动的容器：
 ```shell
docker stop timezone 
 ```
使用```docker rm```删除已停止的容器
 ```shell
docker rm -v timezone #-v参数可以删除与容器关联的卷。
 ```
> docker ```start/stop/restart/rm ```既可以接受```CONTAINER ID```作为参数，也可以接受```CONTAINER NAMES ```

#### 进入容器
如果我们在 ```run ```启动一个容器时不需要进入容器，可以直接通过 -d 参数使容器处于后台deamon模式
 ```shell
docker run -d --name ubuntu-test ubuntu /bin/bash
 ```

在使用 ```-d``` 参数时，容器启动后会进入后台。此时想要进入容器，可以通过```docker exec```指令：
 ```shell
docker exec -it 243c32535da7 /bin/bash
 ```

#### 列出镜像
我们可以使用 ```docker images ```或 ```docker image ls ```来列出本地主机上的镜像。
 ```shell
$ docker images
REPOSITORY           TAG                 IMAGE ID            CREATED             SIZE
redis                latest              5f515359c7f8        5 days ago          183 MB
nginx                latest              05a60462f8ba        5 days ago          181 MB
mongo                3.2                 fe9198c04d62        5 days ago          342 MB
<none>               <none>              00285df0df87        5 days ago          342 MB
ubuntu               18.04               329ed837d508        3 days ago          63.3MB
ubuntu               bionic              329ed837d508        3 days ago          63.3MB
 ```

如果仔细观察，会注意到，这里标识的所占用空间和在 Docker Hub 上看到的镜像大小不同。比如，``ubuntu:18.04 `` 镜像大小，在这里是 63.3MB，但是在  显示的却是 25.47 MB。这是因为 Docker Hub 中显示的体积是压缩后的体积。在镜像下载和上传过程中镜像是保持着压缩状态的，而 ``docker images `` 显示的是镜像下载到本地后展开的大小。
另外一个需要注意的问题是，``docker image ls `` 列表中的镜像体积总和并非是所有镜像实际硬盘消耗。由于 Docker 镜像是多层存储结构，并且可以继承、复用，因此不同镜像可能会因为使用相同的基础镜像，从而拥有共同的层。由于 Docker 使用 Union FS，相同的层只需要保存一份即可，因此实际镜像硬盘占用空间很可能要比这个列表镜像大小的总和要小的多。

我们可以通过 ``docker system df `` 命令来便捷的查看镜像、容器、数据卷所占用的空间。

 ```shell
$ docker system df
​TYPE                TOTAL               ACTIVE              SIZE                RECLAIMABLE
Images              24                  0                   1.992GB             1.992GB (100%)
Containers          1                   0                   62.82MB             62.82MB (100%)
Local Volumes       9                   0                   652.2MB             652.2MB (100%)
Build Cache                                                 0B                  0B
 ```

#### 虚悬镜像
上面的镜像列表中，还可以看到一个特殊的镜像，这个镜像既没有仓库名，也没有标签，均为 ```<none>```：
 ```shell
<none>               <none>              00285df0df87        5 days ago          342 MB
 ```

``docker build ``会导致这种现象。由于新旧镜像同名，旧镜像名称被取消，从而出现仓库名、标签均为 <none> 的镜像。这类无标签镜像也被称为 虚悬镜像(dangling image) ，可以用下面的命令专门显示这类镜像：

 ```shell
$ docker image ls -f dangling=true
REPOSITORY          TAG                 IMAGE ID            CREATED             SIZE
<none>              <none>              00285df0df87        5 days ago          342 MB
 ```

一般来说，虚悬镜像已经失去了存在的价值，是可以随意删除的，可以用下面的命令删除。

 ``` shell
docker image prune
 ```

#### 资源清理
Docker 提供了一个命令，可以清理任何正在悬挂（未标记或与容器关联）的资源（镜像, 容器, 数据卷 和 网络）：
 ```
docker system prune
 ```
若要另外删除任何已停止的容器和所有未使用的镜像（而不仅仅是悬空的镜像），请将标志添加到命令中：-a
 ```
docker system prune -a
 ```

#### 中间层镜像
为了加速镜像构建、重复利用资源，Docker 会利用 中间层镜像。所以在使用一段时间后，可能会看到一些依赖的中间层镜像。默认的 ```docker image ls ```列表中只会显示顶层镜像，如果希望显示包括中间层镜像在内的所有镜像的话，需要加 ```-a ```参数。
 ```shell
docker image ls -a
 ```
这样会看到很多无标签的镜像，与之前的虚悬镜像不同，这些无标签的镜像很多都是中间层镜像，是其它镜像所依赖的镜像。这些无标签镜像不应该删除，否则会导致上层镜像因为依赖丢失而出错。实际上，这些镜像也没必要删除，因为之前说过，相同的层只会存一遍，而这些镜像是别的镜像的依赖，因此并不会因为它们被列出来而多存了一份，无论如何你也会需要它们。只要删除那些依赖它们的镜像后，这些依赖的中间层镜像也会被连带删除。
列出部分镜像

不加任何参数的情况下，```docker image ls ```会列出所有顶层镜像，但是有时候我们只希望列出部分镜像。```docker image ls ```有好几个参数可以帮助做到这个事情。
根据仓库名列出镜像
 ```shell
$ docker image ls ubuntu
REPOSITORY          TAG                 IMAGE ID            CREATED             SIZE
ubuntu              18.04               329ed837d508        3 days ago          63.3MB
ubuntu              bionic              329ed837d508        3 days ago          63.3MB
 ```
列出特定的某个镜像，也就是说指定仓库名和标签
 ```shell
$ docker image ls ubuntu:18.04
REPOSITORY          TAG                 IMAGE ID            CREATED             SIZE
ubuntu              18.04               329ed837d508        3 days ago          63.3MB
 ```
除此以外，docker image ls 还支持强大的过滤器参数 --filter，或者简写 -f。之前我们已经看到了使用过滤器来列出虚悬镜像的用法，它还有更多的用法。比如，我们希望看到在 mongo:3.2 之后建立的镜像，可以用下面的命令：
 ```shell
$ docker image ls -f since=mongo:3.2
REPOSITORY          TAG                 IMAGE ID            CREATED             SIZE
redis               latest              5f515359c7f8        5 days ago          183 MB
nginx               latest              05a60462f8ba        5 days ago          181 MB
 ```
想查看某个位置之前的镜像也可以，只需要把 since 换成 before 即可。
此外，如果镜像构建时，定义了 LABEL，还可以通过 LABEL 来过滤。
 ```shell
$ docker image ls -f label=com.example.version=0.1
...
 ```
#### 用 ID、镜像名、摘要删除镜像
其中，``<镜像>``可以是 ``镜像短ID``、``镜像长ID``、``镜像名 ``或者 ``镜像摘要``。
比如我们有这么一些镜像：
 ```shell
$ docker image ls
REPOSITORY                  TAG                 IMAGE ID            CREATED             SIZE
centos                      latest              0584b3d2cf6d        3 weeks ago         196.5 MB
redis                       alpine              501ad78535f0        3 weeks ago         21.03 MB
docker                      latest              cf693ec9b5c7        3 weeks ago         105.1 MB
nginx                       latest              e43d811ce2f4        5 weeks ago         181.5 MB
 ```
我们可以用镜像的完整 ID，也称为 长 ID，来删除镜像。使用脚本的时候可能会用长 ID，但是人工输入就太累了，所以更多的时候是用 短 ID 来删除镜像。docker image ls 默认列出的就已经是短 ID 了，一般取前3个字符以上，只要足够区分于别的镜像就可以了。
比如这里，如果我们要删除 ``redis:alpine `` 镜像，可以执行：
 ```
$ docker image rm 501
Untagged: redis:alpine
Untagged: redis@sha256:f1ed3708f538b537eb9c2a7dd50dc90a706f7debd7e1196c9264edeea521a86d
Deleted: sha256:501ad78535f015d88872e13fa87a828425117e3d28075d0c117932b05bf189b7
Deleted: sha256:96167737e29ca8e9d74982ef2a0dda76ed7b430da55e321c071f0dbff8c2899b
Deleted: sha256:32770d1dcf835f192cafd6b9263b7b597a1778a403a109e2cc2ee866f74adf23
Deleted: sha256:127227698ad74a5846ff5153475e03439d96d4b1c7f2a449c7a826ef74a2d2fa
Deleted: sha256:1333ecc582459bac54e1437335c0816bc17634e131ea0cc48daa27d32c75eab3
Deleted: sha256:4fc455b921edf9c4aea207c51ab39b10b06540c8b4825ba57b3feed1668fa7c7
 ```
我们也可以用镜像名，也就是 <仓库名>:<标签>，来删除镜像。
 ```
$ docker image rm centos
Untagged: centos:latest
Untagged: centos@sha256:b2f9d1c0ff5f87a4743104d099a3d561002ac500db1b9bfa02a783a46e0d366c
Deleted: sha256:0584b3d2cf6d235ee310cf14b54667d889887b838d3f3d3033acd70fc3c48b8a
Deleted: sha256:97ca462ad9eeae25941546209454496e1d66749d53dfa2ee32bf1faabd239d38
 ```

### 制作镜像
> 创建镜像主要使用 ```Dockerfile ```文件 和 ```docker build ```指令。```Dockerfile ```是一个描述如何创建 ```Docker ```镜像所需步骤的文本文件。

**Dockerfile语法**

|命令|释义|
| :----- | :-------|
FROM | 设置Dockerfile 使用的基础镜像 
LABEL |以键值对的形式给镜像添加一些元数据，比如镜像的作者、文档地址等
COPY | 复制文件至镜像
ADD | 更高级的复制文件指令
ENV | 设置镜像内的环境变量
EXPOSE | 声明容器运行时提供服务的端口，这只是一个声明。帮助镜像使用者理解这个镜像服务的守护端口，以方便配置映射
VOLUME | 指定为数据卷的文件或目录，容器运行时应该尽量保持容器存储层不发生写操作，对于数据库类需要保存动态数据的应用，其数据库文件应该保存于卷(volume)中
USER | 设置任何后续的RUN、CMD 或ENTRYPOINT 指令执行时所用的用户（用户名或UID）
WORKDIR | 对任何后续的RUN、CMD、ENTRYPOINT、ADD 或COPY 指令设置工作目录
RUN | 通常用于安装应用和软件包，build期间执行
CMD | 容器启动时执行指定的指令
ENTRYPOINT | 不会被忽略，一定会被执行，即使运行 docker run时指定了其他命令
HEALTHCHECK | 健康检查

``COPY [--chown=<user>:<group>] ["<源路径1>",... "<目标路径>"] ``
``ADD ``指令和 ``COPY ``的格式和性质基本一致。但是在 COPY 基础上增加了一些功能。
- 比如 ``<源路径> ``可以是一个 URL，这种情况下，Docker 引擎会试图去下载这个链接的文件放到 <目标路径> 去。下载后的文件权限自动设置为 600，如果这并不是想要的权限，那么还需要增加额外的一层 RUN 进行权限调整，另外，如果下载的是个压缩包，需要解压缩，也一样还需要额外的一层 RUN 指令进行解压缩。所以不如直接使用 RUN 指令，然后使用 wget 或者 curl 工具下载，处理权限、解压缩、然后清理无用文件更合理。因此，这个功能其实并不实用，而且不推荐使用。
- 如果 ``<源路径> ``为一个 tar 压缩文件的话，压缩格式为 gzip, bzip2 以及 xz 的情况下，ADD 指令将会自动解压缩这个压缩文件到 <目标路径> 去。
在某些情况下，这个自动解压缩的功能非常有用，比如官方镜像 ubuntu 中：
 ```Dockerfile
FROM scratch
ADD ubuntu-xenial-core-cloudimg-amd64-root.tar.gz /
 ```
- 对于目录而言，```COPY ```和 ```ADD ```命令具有相同的特点：只复制目录中的内容而不包含目录自身
- 如果文件名中含有空格，需要使用双引号将文件名包裹```COPY "test space.txt" /home/opt/Program Files```
- ENTRYPOINT有两种形式，executable与shell，如下：
``` Dockerfile
ENTRYPOINT ["executable"] : executable form
ENTRYPOINT executable : shell form
 ```
- CMD指令也有两种使用方式：
``` Dockerfile
CMD ["command", "param1"] ：exec Form，推荐方式
CMD command param1 ： shell Form
``` 
- HEALTHCHECK 支持下列选项：
--interval=<间隔>：两次健康检查的间隔，默认为 30 秒；
--timeout=<时长>：健康检查命令运行超时时间，如果超过这个时间，本次健康检查就被视为失败，默认 30 秒；
--retries=<次数>：当连续失败指定次数后，则将容器状态视为 unhealthy，默认 3 次。
``` shell
HEALTHCHECK --interval=5s --timeout=3s \
  CMD if [ 2 -eq $(ps -aux | grep tipray | grep -v tipray | wc -l) ]; then exit 0; else exit 1; fi
```
#### 构建步骤
- 创建基础工作文件夹，比如 DokcerImage
- 放入自己的可执行文件和相关环境library
- 创建编写Dockerfile,Dockerfile文件名和后续指令一定要注意大小写相关
 ``` java
DockerImage
├── bin
│   └── hello.sh
└── Dockerfile
 ```

 ``` shell 
$cat hello.sh
 #!/bin/bash
while true; then do
    echo 'Hello Docker!'
    sleep 3s
done
 ```

 ```Dockerfile
$cat Dockerfile 
FROM alpine
LABEL maintainer="huangc" build_date="2022-08-02"

ADD --chown=root:root --chmod=744 bin/ /opt/tipray/docker/

RUN apk add --no-cache tzdata \
    && cp /usr/share/zoneinfo/Asia/Shanghai /etc/localtime \
    && echo "Asia/Shanghai" > /etc/timezone \
    && apk del tzdata

USER root
WORKDIR /opt/tipray/docker/
ENTRYPOINT ["hello.sh"]
 ```
- 执行``docker build ``
``` shell
$ docker build -t tipray/hello:0.1 . 
Sending build context to Docker daemon  5.632kB
Step 1/7 : FROM alpine
 ---> d7d3d98c851f
Step 2/7 : LABEL maintainer="huangc" build_date="2022-08-02"
 ---> Using cache
 ---> 05ea6c781a47
Step 3/7 : ADD --chown=root:root bin/ /opt/tipray/docker/
 ---> Using cache
 ---> 1a844e6a8332
Step 4/7 : RUN apk add --no-cache tzdata     && cp /usr/share/zoneinfo/Asia/Shanghai /etc/localtime     && echo "Asia/Shanghai" > /etc/timezone     && apk del tzdata     && ls -l /
Successfully built e957faa29720
Successfully tagged tipray/hello:0.1
......
 ```
**``docker build``参数说明**：
-t ：指定要创建的目标镜像名[**一般可以按照这样的规范标识，(dockerhub)用户名/镜像名:标签名(tag)**]
. ：Dockerfile 文件所在目录
## 

#### 镜像制作的扩展技巧
- 使用``.dockerignore ``排除环境中不需要的文件，用法和``.gitignore``相同，支持正则表达式
 ```gitignore
.git
*/.git
.svn
.vscode
 ```


- 类似于Git 会保存每一次提交的文件版本，在 Dockerfile 中,每一条指令都会创建一个镜像层，继而会增加整体镜像的大小。尽可能合并指令以减少中间镜像层
``` Dockerfile
Dockerfile
 ...
RUN apk add --no-cache tzdata \
    && cp /usr/share/zoneinfo/Asia/Shanghai /etc/localtime \
    && echo "Asia/Shanghai" > /etc/timezone \
    && apk del tzdata
 ...
 ```

- 复制文件的同时修改元信息```COPY --chmod=755 --chown=normal:normal output/hello /usr/bin/hello```。使用前需要开启 docker 的 buildkit 特性（在 docker build 命令前添加 DOCKER_BUILDKIT=1 即可），目前只支持 --chmod=755 和 --chmod=0755 这种设置方法，不支持 --chmod=+x



- **及时清理不需要的文件**，运行容器时不需要的文件，一定要在创建的同一层清理，否则依然会保留在最终的镜像中。
 ``` Dockerfile
RUN apt-get update \
  && apt-get install -y <PACKAGES> \
  && rm -rf /var/lib/apt/lists/*
 # 官方的 ubuntu/debian 镜像 apt-get 会在安装后自动执行 clean 命令
 ```

- 使用 Docker 自带的 ```docker history ```命令，分析所有镜像层的创建时间、指令以及体积等较为基础的信息

 ```shell
$docker history tipray/hello:0.1
IMAGE          CREATED         CREATED BY                                      SIZE      COMMENT
2bd99827b7b9   2 minutes ago                                                   26.1kB    merge sha256:31869c3d8326ef9d4474835ca7d7f23b34b3a5375e68ccf10676b53e30df8ed9 to sha256:d7d3d98c851ff3a95dbcb70ce09d186c9aaf7e25d48d55c0f99aae360aecfd53
<missing>      2 minutes ago   /bin/sh -c #(nop)  CMD ["/bin/sh" "-c" "ENTR…   0B        
<missing>      2 minutes ago   /bin/sh -c #(nop) WORKDIR /opt/tipray/docker/   0B        
<missing>      2 minutes ago   /bin/sh -c #(nop)  USER root                    0B        
<missing>      2 minutes ago   /bin/sh -c apk add --no-cache tzdata     && …   0B        
<missing>      2 minutes ago   /bin/sh -c #(nop) ADD --chown=root:rootdir:1…   0B        
<missing>      2 minutes ago   /bin/sh -c #(nop)  LABEL maintainer=huangc b…   0B        
<missing>      2 weeks ago     /bin/sh -c #(nop)  CMD ["/bin/sh"]              0B        
<missing>      2 weeks ago     /bin/sh -c #(nop) ADD file:a2648378045615c37…   5.53MB    
```



- 使用第三方工具dive分析镜像层组成，并列出每个镜像层所包含的文件列表，可以很方便地定位到影响镜像体积的构建指令以及具体文件```dive golang:1.16 ```



#### 合并多个镜像层
当构建的层数很多且执行指令较复杂时，可通过 ```docker build``` 命令中添加 ```—squash``` 参数在最终生成镜像时将所有镜像层合并成一层Dockerfile：
``` shell
$ docker build -t squash-image --squash -f Dockerfile . 
$ docker history squash-image
IMAGE          CREATED        CREATED BY                                      SIZE      COMMENT
55ded8881d63   9 hours ago                                                    0B        merge sha256:95f1695b29044522250de1b0c1904aaf8670b991ec1064d086c0c15865051d5d to sha256:e66264b98777e12192600bf9b4d663655c98a090072e1bab49e233d7531d1294
<missing>      11 hours ago   /bin/sh -c #(nop)  ENTRYPOINT ["/bin/ash"]      0B
<missing>      11 hours ago   /bin/sh -c rm -f /resource.tar                  0B
<missing>      11 hours ago   /bin/sh -c touch /resource.tar                  0B
<missing>      11 hours ago   /bin/sh -c #(nop) COPY file:66065d6e23e0bc52…   0B
<missing>      7 weeks ago    /bin/sh -c #(nop)  CMD ["/bin/sh"]              0B
<missing>      7 weeks ago    /bin/sh -c #(nop) ADD file:8e81116368669ed3d…   5.53MB
 ```
这种做法的坏处在于，镜像在保存和分发时是可以复用镜像层的，推送镜像时会跳过镜像仓库已存在的镜像层，拉取镜像时会跳过本地已拉取过的镜像层，而合并成一层后则失去了这种优势。


- 修改镜像时区
[Dockerfile设置默认时区](https://blog.51cto.com/cuiyingfeng/4371774)




- 不要再镜像中存储密钥信息
虽然这可能是做简单的方法，但它意味着任何能够获取镜像的敌人都能获取你的密钥。
通过环境变量传递密钥相对直接写入镜像更优，并非最好的方案，但更容易实现。比如：
``` shell
docker run -it \
    -p 23306:23306 \
    -e MYSQL_ROOT_PASSWORD=tr838408_dlp@2019 \
    --name my-mysql-main.jmfw \
    my/mysql:0.1
 ```
官方mysql ``entrtpoint.sh`` 脚本中会读取 ``MYSQL_ROOT_PASSWORD`` 变量并设置为root用户密码


##
- 从指定的镜像层构建镜像
``` shell
FROM 8eaa4ff06b53
RUN echo "Build form image id" > /etc/buildinfo
RUN echo "8eaa4ff06b53" >> /etc/buildinfo
CMD ["cat", "/etc/buildinfo"]
 ```

- 一个容器对应一个应用，甚至进程
容器本身的设计，就是希望容器和服务/应用能够具备相同的生命周期。
出现故障时开发人员能方便地对该故障容器进行问题排查，而不必对整个系统的各个部分进行排查，这也使得其更具有可移植性和可预测性。
升级程序时能够将影响范围控制再更小的粒度，极大增加应用程序生命周期管理的灵活性，避免在升级某个服务时中断相同容器中的其他进程。

- 让业务进程的pid保持为1：如果业务进程可以在CMD中启动，则使用executable模式；如果必须放在shell脚本中启动，则在业务进程的启动命令前加上exec关键字使其pid保持为1

#### 调试镜像层
> 很多时候，刚接触Docker的用户都会被镜像生成的过程难倒。Dockerfile 中的每个指令执行后都会产生一个新的统像层，而这个镜像层其实可以用来启动容器。 一个新的镜像层的建立，是用上一层的镜像启动容器，然后执行Dockerfile的指令后，把它保存为一个新的镜像。 当Dockerfile指令成功执行后，中间使用过的那个容器会被删掉，除非提供了 ``-rm=false ``或 ``--squash `` 等参数
通过``docker history``查看组成镜像的所有层，当构建失败时，可以把失败前的那个层启动起来。
 ```shell
 ..
 ---> 043b0f212924
Step 7/7 : CMD ENTRYPOINT ["sh", "hello.sh"]
 ---> Running in 1e7bf37d08da
Removing intermediate container 1e7bf37d08da
 ---> 854f279eb679
Successfully built 854f279eb679
Successfully tagged tipray/hello:0.1
/bin/sh: ENTRYPOINT: not found
# 这边是run失败了,查看镜像层
$docker history tipray/hello:0.1
IMAGE          CREATED                  CREATED BY                                      SIZE      COMMENT
854f279eb679   Less than a second ago   /bin/sh -c #(nop)  CMD ["/bin/sh" "-c" "ENTR…   0B        
043b0f212924   Less than a second ago   /bin/sh -c #(nop) WORKDIR /opt/tipray/docker/   0B        
db59f09c9771   1 second ago             /bin/sh -c #(nop)  USER root                    0B        
40caf336ee25   1 second ago             /bin/sh -c apk add --no-cache tzdata     && …   26kB      
2db844931aeb   3 seconds ago            /bin/sh -c #(nop) ADD --chown=root:rootdir:1…   63B       
05ea6c781a47   3 seconds ago            /bin/sh -c #(nop)  LABEL maintainer=huangc b…   0B        
d7d3d98c851f   2 weeks ago              /bin/sh -c #(nop)  CMD ["/bin/sh"]              0B        
<missing>      2 weeks ago              /bin/sh -c #(nop) ADD file:a2648378045615c37…   5.53MB    

#进入entrypoint前的一个镜像层
$ docker run -it --name debug 043b0f212924

#尝试调试
/opt/tipray/docker # ls
hello.sh
/opt/tipray/docker # sh hello.sh
hello.sh: line 1: syntax error: unexpected "then" (expecting "do")
 ```
可以看出，我们通过进入镜像层调试模拟脚本执行发现实际上问题是脚本语法问题。

#### 镜像构建缓存
Docker 为了加快镜像构建的速度, 也会将每一个镜像层缓存下来。Docker 的缓存特性能大大提高工作效率, 可是它的实现却有点笨抽。 你的指令必须满足以下条件，Docker才会使用缓存:
- 上一个指令能够在缓存中找得到，并且
- 缓存中存在一个镜像层，而它的指令与你的指令一模一样，父层也完全相同(即使指令中出现一些无关重要的空格也会使缓存失效)

**此外**，关于COPY和ADD指令，如果它们引用的文件的校验和或元数据发生了变化，那么缓存也将失效。

这意味着，即使那些每次调用的结果可能都不一样的RUN指令，也仍然会被缓存。如果需要下载文件、执行 apt-get update或复制源码库, 请务必注意这一点。

如果需要使缓存失效，可以在执行 ``docker build ``的时候加上 ``--no-cache ``参数

#### 规范开发，制定脚本
``` perl
apprval
    ├── Dockerfile
    ├── init.sh
    ├── in.sh
    ├── pack.sh
    ├── rm.sh
    ├── TRWfe
    ├── Readme.md
    └── VERSION
 ```
``` shell
$ cat init.sh
PRODUCT_SERI="my"
PROD="apprval"
VERSION_PATH=$PWD/VERSION
VERSION=`cat $VERSION_PATH`
TAG="$PRODUCT_SERI/$PROD:$VERSION"

docker build -t $TAG --squash --rm --force-rm . 

docker history $TAG

docker run -d \
    -p 8280:8280 \
    --name $PRDO \
    $TAG
```
``` shell
$cat rm.sh
PRODUCT_SERI="my"
PROD="mysql"
VERSION_PATH=$PWD/VERSION
VERSION=`cat $VERSION_PATH`
REPOSITORY="$PRODUCT_SERI/$PROD"
TAG="$REPOSITORY:$VERSION"

realted_containers=`sudo docker ps -a |grep "$REPOSITORY"`
echo $realted_containers

realted_containers_list=`echo $realted_containers | awk '{print $1}'`
cnt=`echo "$realted_containers_list" | xargs  | awk -F ' ' '{print NF}'`
if [ "$cnt" -gt 0 ]; then
    for id in $realted_containers_list; do
        sudo docker stop $id && sudo docker rm $id
        echo "stop and remove container id $id" 
    done
fi

realted_images=`sudo docker images -a | grep "$REPOSITORY"`
echo $realted_images
realted_images_list=`echo $realted_images | awk '{print $3}'`
cnt=`echo "$realted_images_list" | xargs  | awk -F ' ' '{print NF}'`
if [ "$cnt" -gt 0 ]; then
    for id in $realted_images_list; do
        sudo docker rmi $id
        echo "remove image id $id" 
    done
fi
sudo docker rmi $(sudo docker images --filter "dangling=true" -q --no-trunc)
 ```

#### 导出/导入镜像
- 导出
``` shell
$ cat pack.sh
PRODUCT_SERI="my"
PROD="apprval"
VERSION_PATH=$PWD/VERSION
VERSION=`cat $VERSION_PATH`
TAG="$PRODUCT_SERI/$PROD:$VERSION"

sudo docker save "$TAG" > "../$PRODUCT_SERI-$PROD-$VERSION.tar"
sudo gzip "../$PRODUCT_SERI-$PROD-$VERSION.tar"
sudo chmod a+rw "../$PRODUCT_SERI-$PROD-$VERSION.tar.gz"
sudo rm "../$PRODUCT_SERI-$PROD-$VERSION.tar"
 ```
- 导入
```
docker load -i my/apprval:0.1.tar.gz
```
**更优的实践：通过公司内自建寄存服务（regesity）内部分发镜像**

^
^
###
#### 数据持久化


在Docker中，要想实现数据的持久化，需要将数据从宿主机挂载到容器中。下面列举2种常见的方式：
- ``-v`` 或 ``--volume``：挂载存在于宿主机文件系统上，并由宿主机维护者管理。Docker 之外的应用程序/进程也可以对其进行修改
- ``--mount``：Docker 将为我们管理它们，并且无法在 Docker 外部访问它们（类似于自己的文件系统），可以存储在专用的远程位置（例如，在云中）并将其与系统的其他部分分离，并将其与备份、监控、加密和硬件管理等外部服务集成。

**使用数据卷的最佳场景**
1.在多个容器之间共享数据，多个容器可以同时以只读或者读写的方式挂载同一个数据卷，从而共享数据卷中的数据。
2.当宿主机不能保证一定存在某个目录或一些固定路径的文件时，使用数据卷可以规避这种限制带来的问题。
3.当你想把容器中的数据存储在宿主机之外的地方时，比如远程主机上或云存储上。
4.当你需要把容器数据在不同的宿主机之间备份、恢复或迁移时，数据卷是很好的选择。

**数据卷的简单使用**
- 创建一个数据卷
``` shell
$ docker volume create my-vol
```
- 查看所有数据卷
``` shell
$ docker volume ls
DRIVER              VOLUME NAME
local               my-vol
```
- 启动一个挂载数据卷的容器
在用 ``docker run`` 命令的时候，使用 --mount 标记来将 数据卷 挂载到容器里。在一次 docker run 中可以挂载多个 数据卷。
下面创建一个名为 web 的容器，并加载一个 数据卷 到容器的 /usr/share/nginx/html 目录。
```
$ docker run -d -P \
    --name web \
    # -v my-vol:/usr/share/nginx/html \
    --mount source=my-vol,target=/usr/share/nginx/html \
    nginx:alpine
```
- 删除数据卷
```
$ docker volume rm my-vol
```
数据卷 是被设计用来持久化数据的，它的生命周期独立于容器，Docker 不会在容器被删除后自动删除 数据卷，并且也不存在垃圾回收这样的机制来处理没有任何容器引用的 数据卷。如果需要在删除容器的同时移除数据卷。可以在删除容器的时候使用 docker rm -v 这个命令。
无主的数据卷可能会占据很多空间，要清理请使用以下命令
```
$ docker volume prun
```

#### 容器访问外部网络
容器要想访问外部网络，需要本地系统的转发支持。在Linux 系统中，检查转发是否打开。
``` shell   
$sysctl net.ipv4.ip_forward
net.ipv4.ip_forward = 1
 ```
如果为 0，说明没有开启转发，则需要手动打开。
``` shell
$sysctl -w net.ipv4.ip_forward=1
 ```
如果在启动 Docker 服务的时候设定 --ip-forward=true, Docker 就会自动设定系统的 ip_forward 参数为 1


#### 使用 ``Docker Compose`` 编排镜像
``Docker Compose``旨在迅速建立和运行Docker开发环境。大体上，它使用YAML文件来存储不同容器的配置, 节省开发者重复且容易出错的输入，以及避免了自行开发解决方案的负担。当你的应用开始变得复杂时，它就能发挥所长了。Compose将使我们免于自己维护用于服务编排的脚本，包招启动、 连接、更新和停止容器。``docker-compose.yml：``
``` yml
version: '3'
services:
  my-mysql:
    container_name: my-mysql
    environment:
      - MYSQL_ALLOW_EMPTY_PASSWORD=yes
      - MYSQL_ROOT_PASSWORD=tr838408_dlp@2019
    image:  my/mysql:0.1
    ports:
      - "23306:23306"
    network_mode: "host"

  my-engine:
    container_name: my-engine
    image:  my/engine:1.0
    ports:
      - "20180:20180"
    network_mode: "host"
    depends_on:
      - my-mysql
 ```
**基础指令**
``up``
启动所有在Compose 文件中定义的容器，并且把它们的日志信息汇集一起，通常会使用-d参数使Compose 在后台运行。
``build``
重新建造由 Dockertile 构建的镜像，除非镜像不在在，否则中命令不会技行构建的动作，因此需要更新镜像时便使用这个命令。
``ps``
获取由Compose 管理的容器的状态信息。
``run``
启动一个容器，并运行一个一次性的命令。被连接的容器会同时启动，除非用了-deps参数。
``logs``
汇集由Compose 管理的容器的日志，并以彩色输出。
``down``
停止容器，但不会删除它们。
``rm``
删除已停止的容器。不要忘记使用-v参数来删除任何由 Docker 管理的数据卷

 ``` shell
 (base) ➜  docker docker-compose up  
[+] Running 8/8
 ⠿ Container my-mysqlapp  Created                                                                                             0.1s
 ⠿ Container my-mysql     Created                                                                                             0.1s
 ⠿ Container my-engine    Created                                                                                             0.0s
 ⠿ Container my-apprval   Created                                                                                             0.0s
 ⠿ Container my-bkappsrv  Created                                                                                             0.1s
 ⠿ Container my-datasrv   Created                                                                                             0.1s
 ⠿ Container my-bksrv     Created                                                                                             0.1s
 ⠿ Container my-console   Created                                                                                             0.0s
Attaching to my-apprval, my-bkappsrv, my-bksrv, my-console, my-datasrv, my-engine, my-mysql, my-mysqlapp
my-mysql     | 2022-08-04 14:42:42+08:00 [Note] [Entrypoint]: Entrypoint script for MySQL Server 5.7.39-1.el7 started.
my-mysqlapp  | 2022-08-04 06:42:42+00:00 [Note] [Entrypoint]: Entrypoint script for MySQL Server 5.7.39-1.el7 started.
my-mysql     | 2022-08-04 14:42:42+08:00 [Note] [Entrypoint]: Switching to dedicated user 'mysql'
my-mysqlapp  | 2022-08-04 06:42:42+00:00 [Note] [Entrypoint]: Switching to dedicated user 'mysql'
my-mysql     | 2022-08-04 14:42:42+08:00 [Note] [Entrypoint]: Entrypoint script for MySQL Server 5.7.39-1.el7 started.
my-mysqlapp  | 2022-08-04 06:42:42+00:00 [Note] [Entrypoint]: Entrypoint script for MySQL Server 5.7.39-1.el7 started.
my-mysql     | 2022-08-04 14:42:42+08:00 [Note] [Entrypoint]: Initializing database files
my-mysqlapp  | 2022-08-04 06:42:42+00:00 [Note] [Entrypoint]: Initializing database files
my-mysql     | 2022-08-04T14:42:42.891562+08:00 0 [Warning] option 'skip_name_resolve': boolean value '' wasn't recognized. Set to OFF.
my-mysql     | 2022-08-04T14:42:42.891621+08:00 0 [Warning] TIMESTAMP with implicit DEFAULT value is deprecated. Please use --explicit_defaults_for_timestamp server option (see documentation for more details).
my-mysql     | 2022-08-04T14:42:42.891709+08:00 0 [ERROR] Can't find error-message file '/usr/bin/mysql/share/mysql/errmsg.sys'. Check error-message file location and 'lc-messages-dir' configuration directive.
my-mysqlapp  | 2022-08-04T06:42:42.893645Z 0 [Warning] TIMESTAMP with implicit DEFAULT value is deprecated. Please use --explicit_defaults_for_timestamp server option (see documentation for more details).
my-mysqlapp  | 2022-08-04T06:42:42.893807Z 0 [ERROR] Can't find error-message file '/usr/bin/mysql/share/mysql/errmsg.sys'. Check error-message file location and 'lc-messages-dir' configuration directive.
my-mysqlapp  | 2022-08-04T06:42:42.900040Z 0 [Warning] InnoDB: Using innodb_file_format is deprecated and the parameter may be removed in future releases. See http://dev.mysql.com/doc/refman/5.7/en/innodb-file-format.html
my-engine    | start engine
my-apprval   | tomcat-start by django
my-apprval   | Tomcat started.
my-mysqlapp  | 2022-08-04T06:42:43.250709Z 0 [Warning] InnoDB: New log files created, LSN=45790
my-mysqlapp  | 2022-08-04T06:42:43.327185Z 0 [Warning] InnoDB: Creating foreign key constraint system tables.
my-mysql     | 2022-08-04T14:42:43.372498+08:00 0 [Warning] InnoDB: New log files created, LSN=45790
my-mysqlapp  | 2022-08-04T06:42:43.386231Z 0 [Warning] No existing UUID has been found, so we assume that this is the first time that this server has been started. Generating a new UUID: a48f603a-13c0-11ed-8abe-000c292bad89.
my-mysqlapp  | 2022-08-04T06:42:43.389715Z 0 [Warning] Gtid table is not ready to be used. Table 'mysql.gtid_executed' cannot be opened.
my-mysql     | 2022-08-04T14:42:43.450298+08:00 0 [Warning] InnoDB: Creating foreign key constraint system tables.
my-mysql     | 2022-08-04T14:42:43.512436+08:00 0 [Warning] No existing UUID has been found, so we assume that this is the first time that this server has been started. Generating a new UUID: a4a2a3d5-13c0-11ed-8b8b-000c292bad89.
my-mysql     | 2022-08-04T14:42:43.513440+08:00 0 [Warning] Gtid table is not ready to be used. Table 'mysql.gtid_executed' cannot be opened.
my-bkappsrv  | 2022-08-04 14:42:43,595 aioCloud proftpd[7]: using TCP receive buffer size of 87380 bytes
my-bkappsrv  | 2022-08-04 14:42:43,595 aioCloud proftpd[7]: using TCP send buffer size of 16384 bytes
my-mysql     | 2022-08-04T14:42:43.599776+08:00 0 [Warning] 
my-mysql     | 2022-08-04T14:42:43.599878+08:00 0 [Warning] 
my-mysql     | 2022-08-04T14:42:43.600680+08:00 0 [Warning] CA certificate ca.pem is self signed.
my-mysqlapp  | 2022-08-04T06:42:43.618890Z 0 [Warning] 
my-mysqlapp  | 2022-08-04T06:42:43.619032Z 0 [Warning] 
my-mysqlapp  | 2022-08-04T06:42:43.620598Z 0 [Warning] CA certificate ca.pem is self signed.
my-mysql     | 2022-08-04T14:42:43.636674+08:00 1 [Warning] root@localhost is created with an empty password ! Please consider switching off the --initialize-insecure option.
my-console   | 9908
 ```

#### 更多生产上的挑战
- 资源限制
- 日志整合
- 集群管理
- (devops)CI/CD 持续集成/持续部署
- 容器 => 容器云(kubernets)