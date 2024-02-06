---
layout: post
title:  OceanBase社区版-单实例安装
date:   2024-02-05 13:21:44
categories: OceanBase
tags: OceanBase
---

* content
{:toc}

| 版本 | 修改人 | 修改日期 | 修改内容 | 备注 |
| :--: | :----: | :------: | :------: | :--: |
| 1.0 | 黄超 | 2023/10/03 | 初版 | |





## 名词约定

- OB
  OceanBase 数据库，简称OB
- OBD 
  OceanBase Deployer，OceanBase 集群安装部署工具，简称为 OBD





## 一、环境配置

| 配置项       |                             要求                             |
| ------------ | :----------------------------------------------------------: |
| 系统         | Anolis OS 8.X / Red Hat Enterprise Linux Server 7.X 、8.X / CentOS Linux 7.X 、8.X/ SUSE / OpenSUSE 15.X 及以上 / Debian 9.X 及以上 / Ubuntu 20.X 及以上 |
| 操作系统内核 |                       大于等于 3.10.0                        |
| CPU          | 个人用户最低要求 2 核，推荐 8 核及以上 / 生产环境推荐 32 核及以上 |
| 内存         | 个人用户简单试用最低要求 8G 闲置，长期使用要求不低于 32G，推荐 64G 及以上 / 生产环境推荐256G 及以上 |
| 磁盘类型     |                         推荐使用 SSD                         |
| 磁盘存储空间 |   内存大小的 4 倍及以上，需要有额外的硬盘用于挂载数据目录    |
| 文件系统     |           EXT4 戓 XFS，当数据超过 16T 时，使用 XFS           |
| 网卡         |                        千兆互联及以上                        |

**注意** OceanBase 需要有单独的数据盘，因此至少需要两块硬盘





## 二、软件资源准备

aarch64 版本

OBD：http://mirrors.aliyun.com/oceanbase/community/stable/el/7/aarch64/ob-deploy-2.3.1-2.el7.aarch64.rpm

OB：http://mirrors.aliyun.com/oceanbase/community/stable/el/7/aarch64/oceanbase-ce-3.1.5-100010012023060910.el7.aarch64.rpm

x86_64版本

OBD：http://mirrors.aliyun.com/oceanbase/community/stable/el/7/x86_64/ob-deploy-2.3.1-2.el7.x86_64.rpm

OB：http://mirrors.aliyun.com/oceanbase/community/stable/el/7/x86_64/oceanbase-ce-3.1.5-100020022023091114.el7.x86_64.rpm





## 三、部署前配置
### 3.1 CentOS 操作系统，需要运行以下命令，手动关闭透明大页
`` echo never > /sys/kernel/mm/transparent_hugepage/enabled ``


### 3.2 修改 sysctl.conf
在 ``/etc/sysctl.conf``添加以下内容

``` ini
# for oceanbase
## 修改内核异步 I/O 限制
fs.aio-max-nr=1048576

## 网络优化
net.core.somaxconn = 2048
net.core.netdev_max_backlog = 10000 
net.core.rmem_default = 16777216 
net.core.wmem_default = 16777216 
net.core.rmem_max = 16777216 
net.core.wmem_max = 16777216

net.ipv4.ip_local_port_range = 3500 65535 
net.ipv4.ip_forward = 0 
net.ipv4.conf.default.rp_filter = 1 
net.ipv4.conf.default.accept_source_route = 0 
net.ipv4.tcp_syncookies = 0 
net.ipv4.tcp_rmem = 4096 87380 16777216 
net.ipv4.tcp_wmem = 4096 65536 16777216 
net.ipv4.tcp_max_syn_backlog = 16384 
net.ipv4.tcp_fin_timeout = 15 
net.ipv4.tcp_max_syn_backlog = 16384 
net.ipv4.tcp_tw_reuse = 1 
net.ipv4.tcp_tw_recycle = 1 
net.ipv4.tcp_slow_start_after_idle=0

vm.swappiness = 0
vm.min_free_kbytes = 2097152
# 修改进程可以拥有的虚拟内存区域数量
vm.max_map_count = 655360
```

使配置生效
``sysctl -p``




### 3.3 关闭 SELINUX
修改 ``/etc/selinux/config``
``` ini
# This file controls the state of SELinux on the system.
# SELINUX= can take one of these three values:
#     enforcing - SELinux security policy is enforced.
#     permissive - SELinux prints warnings instead of enforcing.
#     disabled - No SELinux policy is loaded.
SELINUX=disabled
# SELINUXTYPE= can take one of three values:
#     targeted - Targeted processes are protected,
#     minimum - Modification of targeted policy. Only selected processes are protected.
#     mls - Multi Level Security protection.
SELINUXTYPE=targeted
```



### 3.4 关闭防火墙或OB的rpc及数据访问端口

``` shell
systemctl disable firewalld 
systemctl stop firewalld
systemctl status firewalld
```



### 3.5 创建普通用户

> 社区版也可以不操作，但是后面用户相关配置会变动

按照以下步骤创建用户并设置权限，此处以创建 oceanbase 用户为例

``` shell
# 创建用户
mkdir -p /home/oceanbase
useradd -U oceanbase -d /home/oceanbase -s /bin/bash
chown -R oceanbase:oceanbase /home/oceanbase

# 设置密码（可略）
passwd oceanbase

# 添加 sudo 权限，修改 /etc/sudoers 添加如下内容：
## Same thing without a password
# %wheel        ALL=(ALL)       NOPASSWD: ALL
oceanbase       ALL=(ALL)       NOPASSWD: ALL
```


### 3.7 修改 limits.conf
在 ``/etc/security/limits.conf``添加以下内容
``` shell
oceanbase hard nofile 655360
oceanbase soft nofile 655360
oceanbase hard stack 20480
oceanbase soft stack 20480
oceanbase soft nproc 655360
oceanbase hard nproc 655360
oceanbase soft core unlimited
oceanbase hard core unlimited
```
重新进入会话后生效



### 3.6 挂载data、log 用硬盘

**注意** 磁盘分区格式化操作需要根据机器实际情况操作，以下操作仅供参考

``` shell
# 对 sdb 进行分区
fdisk /dev/ 

# Welcome to fdisk (util-linux 2.23.2).
# Changes will remain in memory only, until you decide to write them.
# Be careful before using the write command.
# 
# Device does not contain a recognized partition table
# Building a new DOS disklabel with disk identifier 0x1df3941f.

# 操作菜单：键入 n 新建分区
Command (m for help): n
# 分区类型
Partition type:
  p   primary (0 primary, 0 extended, 4 free)
  e   extended
# 默认回车，主分区
Select (default p): 
Using default response p
# 分区号，新硬盘直接默认
Partition number (1-4, default 1): 
# 起始扇区，默认即可
First sector (2048-2147483647, default 2048): 
Using default value 2048
# 结束扇区，根据需要设置，至少20G，数据分区大小应该尽量大，如果直接回车则直接分配整块硬盘
Last sector, +sectors or +size{K,M,G} (2048-2147483647, default 2147483647): +200G
Partition 1 of type Linux and of size 200 GiB is set

# w 指令保存退出
Command (m for help): w
The partition table has been altered!
Calling ioctl() to re-read partition table.
```


我们内部部署暂时不单独挂载 log 盘


修改集群相关挂载目录所属用户并挂载目录

``` shell
# 格式化分区
mkfs.ext4 /dev/sdb1
mkfs.ext4 /dev/sdc1

# 创建挂载目录
mkdir -p /data
mkdir -p /redo

mount /dev/sdb1 /data

# 设置所属用户
chown -R oceanbase:oceanbase /data
chown -R oceanbase:oceanbase /redo
```



## 四、部署步骤（在新建oceanbase用户下操作）

切换到 oceanbase用户
su - oceanbase



### 4.1 安装obd
``rpm -ivh ob-deploy*.rpm``

如果是 Ubuntu 或 Debian 系操作系统，使用以下指令安装
``sudo alien --scripts -i ob-deploy-*.rpm``



### 4.2 将 OB 安装包加入到本地仓库

执行如下命令禁用远程仓库
``obd  mirror disable remote``

上传下载的 ob 到本地仓库
``obd mirror clone oceanbase-ce-*.rpm``

``obd mirror list local``

在输出的列表中查看到部署所需安装包即表示上传成功。




### 4.3 配置

``` shell
# 创建配置目录
mkdir -p /home/oceanbase/.oceanbase-all-in-one/conf/

# 拷贝默认配置(也可按下面内容自行创建)
cp  /usr/obd/example/mini-local-example.yaml /home/oceanbase/.oceanbase-all-in-one/conf/
```

修改 ``/home/oceanbase/.oceanbase-all-in-one/conf/mini-local-example.yaml``
``` yml
oceanbase-ce:
  version: 3.1.5
  servers:
    # Please don't use hostname, only IP can be supported
    - 127.0.0.1
  global:
    #  The working directory for OceanBase Database. OceanBase Database is started under this directory. This is a required field.
    home_path: /home/oceanbase/observer
    # The directory for data storage. The default value is $home_path/store.
    data_dir: /data
    # The directory for clog, ilog, and slog. The default value is the same as the data_dir value.
    redo_dir: /redo
    # Please set devname as the network adaptor's name whose ip is  in the setting of severs.
    # if set severs as "127.0.0.1", please set devname as "lo"
    # if current ip is 192.168.1.10, and the ip's network adaptor's name is "eth0", please use "eth0"
    devname: lo
    mysql_port: 23306 # External port for OceanBase Database. The default value is 2881. DO NOT change this value after the cluster is started.
    rpc_port: 2882 # Internal port for OceanBase Database. The default value is 2882. DO NOT change this value after the cluster is started.
    zone: zone1
    cluster_id: 1
    # please set memory limit to a suitable value which is matching resource.
    # 最小可用模式下固定为 6G。
    # 最大占用模式下 memory_limit = max(6G, 可用内存*0.9)，当磁盘可用空间不足的情况下会被重新调整，最小值为 6G。
    memory_limit: 6G # The maximum running memory for an observer
    # 最小可用模式下固定为 1G。
    # 最大占用模式下该配置项根据 memory_limit 配置的大小调整。对应关系如下表所示。
    # memory_limit	[6G, 8G)	[8G, 16G)	[16G, 32G)	[32G,48G)	[48G, 64G]	
    # system_memory	2G	            3G	        5G	        7G	        10G
    system_memory: 2G # The reserved system memory. system_memory is reserved for general tenants. The default value is 30G.
    # datafile_size: 2G # Size of the data file.
    # datafile_next: 2G # the auto extend step. Please enter an capacity, such as 2G
    # datafile_maxsize: 20G # the auto extend max size. Please enter an capacity, such as 20G
    # log_disk_size: 13G # The size of disk space used by the clog files.
    cpu_count: 4
    production_mode: false
    enable_syslog_wf: false # Print system logs whose levels are higher than WARNING to a separate log file. The default value is true.
    enable_syslog_recycle: true # Enable auto system log recycling or not. The default value is false.
    max_syslog_file_count: 4 # The maximum number of reserved log files before enabling auto recycling. The default value is 0.
    root_password: tr838408_dlp@2019 # root user password, can be empty
```

配置条目说明参考：[配置文件说明-OceanBase 安装部署工具-OceanBase文档中心-分布式数据库使用文档](https://www.oceanbase.com/docs/community-obd-cn-1000000000254555)

### 4.4 部署&启动 OB

``obd cluster deploy single_node -c ~/.oceanbase-all-in-one/conf/mini-local-example.yaml --clean``



### 4.5 连接测试

使用 mysql 官方客户端 或 第三方客户端均可连接验证



### 4.6 OB 运维 指令参考

以部署名为 single_node 为例

``` shell
# 查看集群列表
obd cluster list

# 查看集群状态
obd cluster display single_node

# 停止运行中的集群
obd cluster stop single_node

# 启动已部署的集群，成功时打印集群状态
obd cluster start single_node

# 重启一个运行中集群
obd cluster restart single_node

# 修改已有节点配置
obd cluster edit-config single_node

# 重载一个运行中的集群(常用于配置更新)
obd cluster reload single_node

# 销毁已部署的集群
obd cluster destroy single_node

# 展示指定集群的状态
obd cluster display single_node

# 重启一个运行中集群（当您使用 edit-config 修改一个运行的集群的配置信息后，可以通过 redeploy 命令应用修改）
obd cluster redeploy single_node

# 根据配置部署集群
obd cluster deploy single_node

# 配置并部署启动集群
obd cluster autodeploy
```

更多指令参考：[集群命令组-OceanBase 安装部署工具-OceanBase文档中心-分布式数据库使用文档](https://www.oceanbase.com/docs/community-obd-cn-1000000000197051)