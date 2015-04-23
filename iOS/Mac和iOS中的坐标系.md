## Mac和iOS中的坐标系

话说Mac,iOS中的各种坐标系总会让初学者摸不着头脑，一会儿这样一会儿那样。不过有一点是不变的，z轴的正方向总是指向观察者，也就是垂直屏幕平面向上。

* 1.NSView坐标系
在Mac中NSView的坐标系默认是右手坐标系（View其实是二维坐标系，但是为了方便我们可以假设其是三维坐标系，只是所有界面的变化都是在xy平面上），原点在左下角. NSView提供了一个可以用于覆盖的方法

```objective-c
- (BOOL)isFlipped;
```

此默认返回NO，当返回YES的时候，则坐标系变成左手坐标系，坐标原点变成左上角。 

在Mac的AppKit中有很多界面组件本身就使用了Flipped Coordinate System(覆盖了上面的方法并返回YES)，如NSButton，NSTableview，NSSplitView 更详细的看这里 其中Cocoa Use of Flipped Coordinates 这一节 https://developer.apple.com/library/mac/#documentation/Cocoa/Conceptual/CocoaDrawingGuide/Transforms/Transforms.html

* 2.UIView坐标系
而在iOS的UIView中，则没有所谓的Flipped Coordinate的概念，统一使用左手坐标系，也就是坐标原点在左上角.

* 3.Quartz坐标系
Quartz（Core Graphics）坐标系使用的右手坐标系,原点在左下角,所以所有使用Core Graphics画图的坐标系都是右手坐标系，当使用CG的相关函数画图到UIView上的时候，需要注意CTM的Flip变换，要不然会出现界面上图形倒过来的现象。由于UIKit的提供的高层方法会自动处理CTM（比如UIImage的drawInRect方法），所以无需自己在CG的上下文中做处理。 参见Quartz 2D Coordinate Systems

* 4.CALayer坐标系
这个有些变态了，其坐标系和平台有关，在Mac中CALayer使用的是右手坐标系，其原点在左下角；iOS中使用的左手坐标系，其原点在左上角。 参见 Layer Coordinate System

参考链接：http://geeklu.com/2012/06/3d-coordinate-system/
