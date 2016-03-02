一、性能调优

1、通用

* 优化首次启动时间
* laze loading 数据和界面
* 尽可能让 view 不透明
* 灵活使用 imageNamed 和 initWithContentsOfFile
* 缓存数据
* 过于复杂的 view 不要通过 xib 实现
* 后台线程加载费时资源
* 尽量不要实现 drawRect 方法，会造成 CPU 离屏绘制以及增大内存占用


2、TableView

* cell 重用
* 缓存行高



二、XML和Json

* 同等数据Json更小
* Json 解析速度更快
* 系统自带了 Json 解析库

三、解决过什么难得问题

1、通过符号断点的方式解决 method swizzling 了一个 dealloc 方法导致内存泄露问题


