## NSRunloop学习笔记

#### Run Loop Modes

预定义好的Run loop modes共有以下几种：

* NSDefaultRunLoopMode

从名字可以看出，大多数时候我们只需要使用该mode就行（Core Foundation中对应kCFRunLoopDefaultMode）。但有些情况就不可以，比如在后面这些特定mode的情况下。

* NSConnectionReplyMode

用于NSConnection收发消息，通常情况下无需使用。

* NSModalPanelRunLoopMode

用于处理modal panels事件，也不常用。

* NSEventTrackingRunLoopMode

用于处理鼠标/手势拖动以及其它用户交互事件，比如UIScrollView在滚动时就运行在该mode下。

* NSRunLoopCommonModes

实际上这不是指某个具体的mode，通常是若干具体mode的集合（Core Foundation中对应kCFRunLoopCommonModes）。Cocoa中默认情况下该mode保含了default, modal和event tracking几种modes；但在Core Foundation中默认只包含default mode。

该mode的另一个特殊之处是可以使用Core Foundation中的方法：

```objective-c
void CFRunLoopAddCommonMode ( CFRunLoopRef rl, CFStringRef mode );
```
为common mode增加新的mode。





