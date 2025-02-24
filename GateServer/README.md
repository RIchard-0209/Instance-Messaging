# IM Server

## 依赖文件

### mysqlConnector++
在lib库中添加	`mysql-connector-c++-9.2.0-winx64\lib64\debug\vs14` 下面的两个`.lib`文件


### 登录系统
点击登录需要发送http请求到GateServer，经GateServer验证密码后，再调用grpc请求给StatusServer获取服务器ip信息和token信息反馈给客户端
[回复http登录请求](https://cdn.llfc.club/1713230325540.jpg)

# TODO

1、程序会异常崩溃，可能存在内存泄漏或者多线程竞争资源等问题。返回值为-1