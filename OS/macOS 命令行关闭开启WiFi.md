## macOS 命令行关闭开启WiFi

* 确定 WiFi 设备名称
输入命令：

```
networksetup -listallhardwareports
```

输出中包含：

```
Hardware Port: Wi-Fi
Device: en0
Ethernet Address:dd:dd:dd:dd:dd:dd

```
得到 WiFi 的设备名称是 `en0`

* 创建命令文件

在目录 `/usr/local/bin` 下新建两个文件 `wifi0` 和 `wifi1`，分别写上 WiFi 开启和关闭的命令：

```
networksetup -setairportpower en0 on  // 开启
networksetup -setairportpower en0 off // 关闭
```

* 更改命令文件权限

输入以下命令更改文件权限：

```
sudo chmod  777 wifi0
sudo chmod  777 wifi1
```
至此，就可以愉快的在命令行开启和关闭 WiFi 了！
