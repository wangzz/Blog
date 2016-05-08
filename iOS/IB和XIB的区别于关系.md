## NIB和XIB的区别于关系


#### NIB(NeXT Interface Builder)

* 是二进制文件，只能被Xcode读取

* 使用 Xcode 组件 Interface Builder 编辑。

#### XIB(XML Interface Builder)

* 2007年 随着 Leopard 系统的发布，在 Xcode 3.0 中引入。

* 是可读的 XML 文本格式。

* 可以加入版本管理，有利于解决冲突。

* 使用 Xcode 组件 Interface Builder 编辑。

* Xcode 会在编译时将 XIB 文件通过 ibtool 工具编译成 NIB 文件，并拷贝至 app bundle 中作为资源文件在运行时使用。
