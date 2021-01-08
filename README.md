# MyFtpServer

一个简单ftp服务器实现，仅支持linux系统

### 特点：

- 基于libevent网络框架并参考Memcached线程池实现
- 完成用户的匿名登录，目录获取以及切换和文件的上传和下载
- 线程池使用轮询法选择任务，使用linux管道和event事件唤醒线程
- 使用单例模式，工厂模式

### 编译：

```
cmake .
make
```

### 运行：

```
./main
```

------

# MyFtpServer

A simple FTP server implementation, only supports linux system

### Feature:

- Based on the libevent network framework and refer to the Memcached thread pool implementation
- Complete the user’s anonymous login, directory acquisition and switching, and file upload and download

- Thread pool uses polling method to select tasks, uses linux pipes and event events to wake up threads
- Use singleton mode, factory mode

### Compile：

```
cmake .
make
```

### Run:

```
./main
```

