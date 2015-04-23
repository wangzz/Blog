
## frame相关属性学习笔记

主要介绍frame/bounds/center/anchorPoint/position等UIView或CALayer相关属性之间的关系。

#### 一、view属性

###### 1、 frame

相对父坐标系。

计算公式：

```objective-c
- (CGRect)frame
{
    float x = center.x - layer.anchorPoint.x * bounds.size.width;
    float y = center.y - layer.anchorPoint.y * bounds.size.height;
    float width = bounds.size.width;
    float height = bounds.size.height;
    return CGRectMake(x, y, width, height);
}

- (void)setFrame:(CGRect) rect
{
    center.x = rect.origin.x + layer.anchorPoint.x * rect.size.width;
    center.y = rect.origin.y + layer.anchorPoint.y * rect.size.height;
    bounds.size.width = rect.size.width;
    bounds.size.height = rect.size.height;
}
```

由此可见：

更改center、anchorPoint、bounds的值都会是frame的值发生对应改变；

更改frame的值会是bounds的值发生对应改变；


###### 2、 bounds

相对本地坐标系。

###### 3、 origin

更改bounds的origin会改变当前view的子view的位置，具体原因见下图所示：

![bounds](http://img.my.csdn.net/uploads/201407/03/1404381811_6859.png)

参考链接：http://www.xiaoyaoli.com/?p=1182

###### 4、 size

更改frame的时候会改变bounds的size，同样更改bounds的size的时候也会改变frame的size；

frame.size和bounds.size不一定能完全一致，比如在view旋转的时候，详情见下图：

![coordinates](http://img.my.csdn.net/uploads/201407/03/1404381833_2518.jpg)

参考链接：http://stackoverflow.com/questions/5361369/uiview-frame-bounds-and-center

###### 5、 center

相对父坐标系。

更改frame的时候会改变center，同样更改center的时候也会改变frame的origin。


#### 二、layer属性

###### 1、 frame
 
对应view的frame

###### 2、 bounds

对应view的bounds

###### 3、 anchorPoint

默认值是(0.5，0.5);

是旋转、缩放等操作的中心点；

更改anchorPoint会改变frame的origin，但反过来更改frame不会改变anchorPoint的值。

###### 4、 position

和center永远保持一致
