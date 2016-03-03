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

* 通过符号断点的方式解决 method swizzling 了一个 dealloc 方法导致内存泄露问题
* 处理定位点不准的问题，通过埋点跟踪确定定位时间戳的取值筛选条件
* 灵活的活动功能
* 离线地图下载的优化过程

五、Runtime

1、定义

是将 C 语言转化为面向对象语言的扩展。

2、 消息传递机制

* 判断处理消息的对象是否为空
* 到接收消息的对象方法列表里找，找不到的话到父类方法列表里找
* 还找不到就调用 resolveInstanceMethod 看运行时有没有动态添加方法
* 仍然不行就调用 forwardInvocation 看其他对象能否处理
* 如果还不行就抛出异常

3、可以通过类别和联合存储的方式为一个类添加方法和属性

4、可以动态创建类，并为其添加属性、方法

六、Run Loop

1、事件源

* 定时器源

* 输入源

2、Mode

NSDefaultRunLoopMode // 空闲状态
UITrackingRunLoopMode // 滑动列表时
NSRunLoopCommonModes // 全部状态

3、线程和 Run Loop

每个线程都对应有一个 Run Loop，主线程是已经启动了，其它线程的需要我们手动启动；
当 Run Loop 里没有任何需要监测的源事件时，会退出










