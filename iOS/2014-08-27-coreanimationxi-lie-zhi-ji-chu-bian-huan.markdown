---
layout: post
title: "CoreAnimation系列之基础变换"
date: 2014-08-27 12:21:50 +0800
comments: true
categories: iOS
tags: [CoreAnimation, iOS, CATransform3D]
keywords: CoreAnimation, iOS, CATransform3D, Translate, Scale, Rotate, 平移, 缩放, 旋转
description: CoreAnimation系列之基础变换

---

从[CSDN](http://blog.csdn.net/wzzvictory)时代开始，就有用一系列文章聊聊CoreAnimation的打算，这算是本系列中的第三篇了。一直以来都是哪天心情好的时候来一篇，真怀疑等把整个系列写完的时候CoreAnimation是不是都要被Apple换掉了。

本文打算介绍自己对基础变换的认识。

##一、基础变换与数学

####1.两种坐标系

不管是平面几何还是立体几何，笛卡尔坐标系都是我们学习和研究几何的最基础工具。的笛卡尔坐标系主要分两种：左手坐标系和右手坐标系。

对于三维坐标系，[百度百科](http://baike.baidu.com/view/2939423.htm)上给出了右手坐标系的判断方法：在空间直角坐标系中，让右手拇指指向x轴的正方向，食指指向y轴的正方向，如果中指能指向z轴的正方向，则称这个坐标系为右手直角坐标系。同理左手直角三维坐标系。

<!-- more -->

下图直观的表示了上述判断方法（图片来自[这里](http://outofmemory.cn/wr/?u=http%3A%2F%2Fwonderffee.github.io%2Fblog%2F2013%2F10%2F17%2Fa-simple-method-to-determine-positive-rotation-in-in-three-dimensional-space%2F)）：

![left-right hand coordinate](/images/article6/coordinate-system.jpg)

由此判断，从中学到大学的课堂上我们接触的立体几何都是右手系。

####2.基础变换的数学公式

一个点在立体空间内的变换可以通过数学公式表示，前面讲那么多左手和右手坐标系相关的内容是因为`不同坐标系下计算公式不同`。

iOS中CoreAnimation的CALayer默认使用的是`左手坐标系`（使用哪种坐标系可以通过CALayer的`geometryFlipped`属性更改，该值默认为NO，设为YES时表示使用右手坐标系），因此本文后面所说的所有坐标系都是之左手坐标系。

变换对于动画来说应该是最基础最核心的内容了，CoreAnimation中基础变换包括平移（Translate）、缩放（Scale）、旋转（Rotate）三种。假如三维空间中有一个点(x0, y0, z0)，该点经过一定条件的基础变换，变换后的坐标为(x, y, z)，则针对平移、缩放、旋转三种基础变换，对应的坐标变换关系如下：

######2.1 平移

平移对应的变化量为(δx, δy, δz)。

```
x = x0 + δx;
y = y0 + δy;
z = z0 + δz;
```

######2.2 缩放

缩放对应的缩放倍数为(δx, δy, δz)。

```
x = x0 * δx;
y = y0 * δy;
z = z0 * δz;
```

######2.3 旋转

旋转的方式有很多，比如简单点的绕X轴、Y轴、Z轴旋转，复杂点的还有绕任意三维向量旋转。为了简单起见，旋转以绕Z轴旋转了角度α（注意这里及后文所有涉及角度的地方都是弧度制）为例，对应的变化关系为：

```
x = y0*sinα + x0*cosα;
y = y0*conα - x0*sinα;
z = z0;
```

其它的大家感兴趣可以自己推倒下。


##二、变换矩阵

在CoreAnimation中用CATransform3D来表示三维齐次坐标变换矩阵，在齐次坐标中n维空间的坐标需要用n+1个元素的坐标元组来表示（详情还请自行Google），因此CATransform3D定义如下：

```
struct CATransform3D
{
  CGFloat m11, m12, m13, m14;
  CGFloat m21, m22, m23, m24;
  CGFloat m31, m32, m33, m34;
  CGFloat m41, m42, m43, m44;
};
```
为什么实现变换要有变换矩阵呢？

以上文中旋转的计算公式为例，可以使用如下矩阵运算表示：

![left-right hand coordinate](/images/article6/matrix1.png)

其中的矩阵：

![left-right hand coordinate](/images/article6/matrix2.png)

就被称为点(x0, y0, z0)绕Z轴旋转角度α的变换矩阵。

由于放射变换可以通过矩阵变换来实现，而且看起来更加直观，因此变换公式通常都用对应的变换矩阵表示。

在CoreAnimation中平移、缩放、旋转对应的变换矩阵为：

####1. 平移

![left-right hand coordinate](/images/article6/matrix3.png)

其中δx、δy、δz表示三个坐标上对应的平移量。

####2. 缩放

![left-right hand coordinate](/images/article6/matrix4.png)

其中δx、δy、δz表示三个坐标上对应的缩放倍数。

####3. 旋转

![left-right hand coordinate](/images/article6/matrix5.png)

该矩阵为任意点(x, y, z)绕任意向量旋转旋转角度α的旋转向量。

##三、验证

前面总结了CoreAnimation中三种基础变换对应的变换矩阵，这样以来我们就能自己对任意的矩阵做变换了。平移、缩放、旋转对应的变换矩阵计算方法如下：

####1. 平移

```objective-c
- (CATransform3D)translateWithMatrix:(CATransform3D)t x:(CGFloat)x y:(CGFloat)y z:(CGFloat)z
{
    CATransform3D matrixTransform = CATransform3DIdentity;
    matrixTransform.m41 = x;
    matrixTransform.m42 = y;
    matrixTransform.m43 = z;
    
    return CATransform3DConcat(matrixTransform, t);
}
```

该方法根据平移变换矩阵的计算方式，得到平移参数(x, y, z)对应的变换矩阵，然后和原始矩阵相乘，得到最终的变换矩阵。

####2. 缩放

```objective-c
- (CATransform3D)scaleWithMatrix:(CATransform3D)t x:(CGFloat)x y:(CGFloat)y z:(CGFloat)z
{
    CATransform3D matrixTransform = CATransform3DIdentity;
    matrixTransform.m11 = x;
    matrixTransform.m22 = y;
    matrixTransform.m33 = z;
    
    return CATransform3DConcat(matrixTransform, t);
}
```

该方法根据缩放变换矩阵的计算方式，得到缩放参数(x, y, z)对应的变换矩阵，然后和原始矩阵相乘，得到最终的变换矩阵。

####3. 旋转

```objective-c
- (CATransform3D)rotateWithMatrix:(CATransform3D)t angle:(CGFloat)angle x:(CGFloat)x y:(CGFloat)y z:(CGFloat)z
{
    CGFloat unitValue = sqrtf(powf(x, 2)+powf(y, 2)+powf(z, 2));
    CGFloat x0 = x/unitValue;
    CGFloat y0 = y/unitValue;
    CGFloat z0 = z/unitValue;
    
    CATransform3D matrixTransform = CATransform3DIdentity;
    matrixTransform.m11 = powf(x0, 2)*(1-cosf(angle))+cosf(angle);
    matrixTransform.m12 = x0*y0*(1-cosf(angle))+z0*sinf(angle);
    matrixTransform.m13 = x0*z0*(1-cosf(angle))-y0*sinf(angle);
    
    matrixTransform.m21 = x0*y0*(1-cosf(angle))-z0*sinf(angle);
    matrixTransform.m22 = powf(y0, 2)*(1-cosf(angle))+cosf(angle);
    matrixTransform.m23 = y0*z0*(1-cosf(angle))+x0*sinf(angle);
    
    matrixTransform.m31 = x0*z0*(1-cosf(angle))+y0*sinf(angle);
    matrixTransform.m32 = y0*z0*(1-cosf(angle))-x0*sinf(angle);
    matrixTransform.m33 = powf(z0, 2)*(1-cosf(angle))+cosf(angle);
    
    return CATransform3DConcat(matrixTransform, t);
}
```

该方法根据旋转变换矩阵的计算方式，得到旋转参数(angle, x, y, z)对应的变换矩阵，然后和原始矩阵相乘，得到最终的变换矩阵。


####4. demo

以旋转变换demo为例：

```objective-c
- (void)logTransform:(CATransform3D)t
{
    NSLog(@"***************************");
    NSLog(@"%f,%f,%f,%f",t.m11,t.m12,t.m13,t.m14);
    NSLog(@"%f,%f,%f,%f",t.m21,t.m22,t.m23,t.m24);
    NSLog(@"%f,%f,%f,%f",t.m31,t.m32,t.m33,t.m34);
    NSLog(@"%f,%f,%f,%f",t.m41,t.m42,t.m43,t.m44);
}

- (IBAction)onRotateButtonAction:(id)sender
{
    //Rotate
    
    //随意的原始矩阵
    CATransform3D matrixOrigin = CATransform3DMakeRotation(1, 4, 3, 6);
    
    CGFloat x = 2, y = 3, z = 4; //旋转向量（2,3,4）
    CGFloat angle = 30.0f * M_PI / 180.0f; //旋转角度30°，计算对应的弧度
    
    //通过系统函数计算变换矩阵
    CATransform3D matrixSystem = CATransform3DRotate(matrixOrigin, angle, x, y, z); 
    systemLayer.transform = matrixSystem;
    [self logTransform:matrixSystem];
    
    //自定义方法计算3D旋转矩阵
    CATransform3D matrixCalculate = [self rotateWithMatrix:matrixOrigin angle:angle x:x y:y z:z]; 
    customLayer.transform = matrixCalculate;
    [self logTransform:matrixCalculate];
}
```

demo使用随意生成的参数`matrixOrigin`模拟一个CALayer的初始`transform`属性值，然后使用同一组变换参数，分别通过系统函数和自定义方法对原始`transform`做变换，然后对比变换结果。最终的计算得到的变换矩阵可以通过log的方式打印出来，也可以在界面上做直观的展示。

平移和缩放变换矩阵的验证方式和旋转类似。

经过对比发现两种计算方式得到的最终变换矩阵是完全相同的，这进一步验证了CoreAnimation中变换矩阵的计算方式。

完整的[demo](https://github.com/wangzz/Demo/tree/master/CoreAnimationDemo)放到了github上，欢迎大家下载。

##四、说明

* 变换矩阵可以组合

可以同时对CALayer进行多种变换，比如同时缩放和旋转，直接通过矩阵相乘得到组合变换的变换矩阵。CoreAnimation提供了变换矩阵组合的方法：

```objective-c
CATransform3D CATransform3DConcat (CATransform3D a, CATransform3D b)
```
需要注意的是，通常情况下矩阵乘法不支持交换律，因此两个矩阵a、b的顺序不能交换。

* 最好不要手动修改变换矩阵的值

CoreAnimation的3D变换对应的4维变换矩阵，单独修改其中的任何一个值都可能带来不可控的变换结果，因此不建议单独手动修改变换矩阵，而是通过基础变换或者基础变换组合的方式修改。

* 理解有误的地方还望大家指出。

##五、参考文档

* [INTRODUCTION TO 3D DRAWING IN CORE ANIMATION](http://www.thinkandbuild.it/introduction-to-3d-drawing-in-core-animation-part-2/)
* [Rotation matrix](http://en.wikipedia.org/wiki/Rotation_matrix)
* [右手坐标系](http://baike.baidu.com/view/2939423.htm)

