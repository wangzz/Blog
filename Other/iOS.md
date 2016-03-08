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

5、method swizzling

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

七、GCD

1、循环提交一个任务

```
func dispatch_apply(_ iterations: UInt,  执行的次数
                  _ queue: dispatch_queue_t!,  
                  _ block: ((UInt) -> Void)!)  
```

2、延迟执行一个任务

```
func dispatch_after(_ when: dispatch_time_t,  
                  _ queue: dispatch_queue_t!,  
                  _ block: dispatch_block_t!) 
```

3、阻塞当前queue

```
func dispatch_barrier_async(_ queue: dispatch_queue_t, _ block: dispatch_block_t):
```

最典型的使用场景是读写问题（Getter/Setter方法）：

```
- (void)setObject:(id)anObject forKey:(id<NSCopying>)aKey
{
    dispatch_barrier_async(self.concurrentQueue, ^{
        [self.mutableDictionary setObject:anObject forKey:aKey];
    });
}

- (id)objectForKey:(id)aKey
{
    __block id object = nil;
    dispatch_sync(self.concurrentQueue, ^{
        object = [self.mutableDictionary objectForKey:aKey];
    });
    return  object;
}
```

对于自己创建的并发的 queue，在它之后提交的任务需要在它之前提交的任务都执行完了，且自己的 block 任务也执行完了以后，才能执行；
非自己创建的并发 queue（串行或者 global 并行 queue） 调用该法的时候，等价于  dispatch_async。

该方法调用后会立即返回，还有一个 sync 的方法：

```
dispatch_barrier_sync(dispatch_queue_t queue, dispatch_block_t block);
```

该方法调用后不会立即返回，而是等待 queue 中在其之前提交的任务都执行了以后，其本身绑定的任务也执行了，才会继续执行其后提交的任务。也就是说，其后提交的任务在其执行完毕之前都还没被提交到 queue 中。

4、信号量

对应有三个方法：

```
dispatch_semaphore_t  dispatch_semaphore_create(long value);
long dispatch_semaphore_signal(dispatch_semaphore_t dsema) // 信号量加一
long dispatch_semaphore_wait(dispatch_semaphore_t dsema, dispatch_time_t timeout)； // 信号连减一
```
需要在非主线程中用上述方法；
只有信号量大于0的时候，才能继续执行，否则会挂起线程。

八、@synthesize和@dynamic的异同
两个关键字都是针对 property 声明的属性的：

1、@synthesize
告诉编译器帮忙生成 setter/getter 方法
可以指定对应的实例变量

2、@dynamic
用于当前类没有实现 property 的 setter/getter 方法时，告诉编译器不要自动生成 setter/getter方法，也不要发出警告，对应的 setter/getter 方法会在运行时动态生成或者已经在父类实现。

目前 @synthesize 属性均已没什么用处了，最新的 Xcode 会默认生成  setter/getter方法。

九、多线程技术

1、NSThread
优点：较轻量
缺点：需要自己管理线程的生命周期，线程同步，线程之间的依赖关系；

2、NSOperation/NSOperationQueue（对GCD的封装）
优点：
不需关心线程的事，可以把注意力集中在任务本身；
NSOperation 可以很方便的设置依赖，NSOperationQueue 可以很方便的管理并发；
任务状态可以通过 KVO/KVC 监听

以下情况会选择NSOperation：
任务之间有依赖关系
限制最大可执行的任务数量
任务可能被取消

缺点：

3、GCD
优点：
简单易用
针对多核处理器（iPhone4s A5以上）优化，任务都在并行执行的线程池中执行。
可以很方便的进行任务嵌套

缺点：
不支持 KVO/KVC
原生不支持取消（也可以通过不端检查 bool 标志，如果为 YES 就提前返回实现）

4、performSelector

十、NSOperation

1、 启动线程

创建 NSOperation 的子类（NSInvocationOperation、NSBlockOperation或者自定义子类）的实例后，NSOperation 对应任务的执行线程为：

* 主线程
 
①没有重写 start 方法，通过调用 start 方法启动任务，会在 `当前` 线程执行；

②将 NSOperation 对象添加到通过 [NSOperationQueue mainQueue] 方式创建的 NSOperationQueue 中，会在 main 线程执行；

* 其它线程

①重写 start 方法时，可以指定任务的执行线程(AFURLConnectionOperation)；

②将 NSOperation 对象添加到通过 [[NSOperationQueue alloc] init] 方式创建的 NSOperationQueue 中，会在其它线程执行；

2、依赖关系

NSOperation 之间可以设置依赖关系

3、NSOperation继承

除了直接使用 NSInvocationOperation 和 NSBlockOperation 外，自己子类化 NSOperation 的时候通常只需要重写 main 方法即可，但记得加 autorelease pool。

并发的 NSOperation 子类需要重写 start 方法。

4、Operation管理

NSOperationQueue 可以对最大并发数目进行约束；
NSOperationQueue 可以挂起 operation queue，但正在执行的任务会继续执行；
NSOperationQueue 可以取消 queue 中的所有 operation，但正在执行的任务会继续执行；

十一、其它

1、__weak 和 __block 区别
https://developer.apple.com/library/ios/releasenotes/ObjectiveC/RN-TransitioningToARC/Introduction/Introduction.html
* 所属范畴不同
前者属于ARC内存管理，后者属于GCD

* 作用不用
前者用于声明弱引用对象，后者用于声明在 GCD block 内外均可修改的局部变量

* 说明
__block 声明的变量在 MRC 下出现在 block 内时不会 retain 对象，但在 MRC 下会

2、Hex string to UIColor

```
// Assumes input like "#00FF00" (#RRGGBB).
+ (UIColor *)colorFromHexString:(NSString *)hexString {
    unsigned rgbValue = 0;
    NSScanner *scanner = [NSScanner scannerWithString:hexString];
    [scanner setScanLocation:1]; // bypass '#' character
    [scanner scanHexInt:&rgbValue];
    return [UIColor colorWithRed:((rgbValue & 0xFF0000) >> 16)/255.0 green:((rgbValue & 0xFF00) >> 8)/255.0 blue:(rgbValue & 0xFF)/255.0 alpha:1.0];
}
```

3、autolayout 原理
AutoLayout的原理是在layoutSubviews中应用Constraints来设置View的frame。换句话说，Constraints只是一个在layoutSubviews时执行的任务队列。

4、探索KVO实现原理



