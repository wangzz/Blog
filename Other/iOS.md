一、性能调优

1、优化方向

* 流畅度
* 内存占用
* 耗电量

2、优化方法

* 优化首次启动时间
* laze loading 数据和界面
* 尽可能让 view 不透明
* 灵活使用 imageNamed 和 initWithContentsOfFile
* 缓存数据
* 过于复杂的 view 不要通过 xib 实现
* 后台线程加载费时资源
* 尽量不要实现 drawRect 方法，会造成 CPU 离屏绘制以及增大内存占用

3、TableView

* cell 重用
* 缓存行高

二、XML和JSON

* 同等数据 JSON 更小
* JSON 解析速度更快
* 系统自带了 JSON 解析库

三、Toll-Free-Bridge

1、MRC

手动内存管理的时候直接强制转换即可，无需涉及到对象权限的转移。

2、ARC

编译器只能处理 OC 对象的内存管理，处理不了 CF 对象，因此需要使用 Toll-Free-Bridge 的关键字进行对象权限转移。

__bridge, __bridge_retained, __bridge_transfer

四、解决过什么难得问题

1、通过符号断点的方式解决 method swizzling 了一个 dealloc 方法导致内存泄露问题

