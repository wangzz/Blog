CoreGraphics学习笔记

一、简介
1、CoreGraphics 坐标系
CoreGraphics 坐标系和我们平时用UIKit的坐标系是不样的，CoreGraphics 的左下角为(0,0)点，而UIKit的左上角为(0,0)点。
但通常我们无需关心此问题，因为我们都是如下方式使用 CoreGraphics ：
```
- (void)drawRect:(CGRect)rect {
    CGContextRef context = UIGraphicsGetCurrentContext();
}
```
其中的 UIGraphicsGetCurrentContext(); 方法是UIKit的方法，坐标系自然还是UIKit的坐标系。

2、几个名词区别
Quartz 、 Quartz 2D 和 QuartzCore 区别

Quartz 是 CoreGraphics 的核心，二者相互替代也没什么问题；Quartz 2D 是 CoreGraphics 的一部分； QuartzCore 实际就是 Core Animation，它扩展了 CoreGraphics 的部分功能，但二者没有互属关系。

二、API使用

1、绘制上下文

```
UIKIT_EXTERN CGContextRef __nullable UIGraphicsGetCurrentContext(void)
```
实际是 UIKit 的方法，用于获取当前绘制上下文，仅当使用 CoreGraphics 绘制时才需要手动调用，使用UIKit绘制（ex. UIBezierPath）时 framework会自动获取绘制上下文。

```
CG_EXTERN void CGContextSaveGState(CGContextRef __nullable c)
```
该方法保存当前的图形状态

```
CG_EXTERN void CGContextRestoreGState(CGContextRef __nullable c)
```
该方法弹出刚刚保存的 context 状态并恢复到上一个 context 状态，两个方法需要成对调用。

2、开始/结束绘制

```
UIKIT_EXTERN CGContextRef __nullable UIGraphicsGetCurrentContext(void)
```
获取当前绘制 context。

```
UIKIT_EXTERN void     UIGraphicsBeginImageContext(CGSize size)
UIKIT_EXTERN void     UIGraphicsBeginImageContextWithOptions(CGSize size, BOOL opaque, CGFloat scale)
UIKIT_EXTERN void     UIGraphicsEndImageContext(void)
```
前两个方法创建新的绘制上下文，并将其设为当前绘制上下文。最后一个方法结束当前的绘制工作，必须和前两个配对调用。

```
UIKIT_EXTERN BOOL UIGraphicsBeginPDFContextToFile(NSString *path, CGRect bounds, NSDictionary * __nullable documentInfo) 
UIKIT_EXTERN void UIGraphicsBeginPDFContextToData(NSMutableData *data, CGRect bounds, NSDictionary * __nullable documentInfo)
UIKIT_EXTERN void UIGraphicsEndPDFContext(void)
```
前两个方法创建新的PDF绘制 context 。最后一个方法结束当前绘制工作，必须和前两个配对调用。

```
CG_EXTERN void CGContextBeginPath(CGContextRef __nullable c)
```
在当前绘制 context 中创建新的 path，准备开始画线。

3、设置绘制条件

```
CG_EXTERN void CGContextSetLineWidth(CGContextRef __nullable c, CGFloat width)
CG_EXTERN void CGContextSetStrokeColorWithColor(CGContextRef __nullable c,
    CGColorRef __nullable color)
```
设置颜色、宽度等属性。

4、颜色

有RGB/CMYK等颜色空间，以RGB为例：

```
CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB(); 
```

获取颜色有以下方法：

```
CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
const CGFloat components[] = {0.90, 0.80, 0.80, 1.0};
CGColorRef color = CGColorCreate(colorSpace, components);

// 或

CGColorRef color = CGColorCreateGenericRGB(0.90, 0.80, 0.80, 1.0);
``` 

5、CTM(Current Transformation Matrix)

实现平移、缩放、旋转等基本变换效果的变换矩阵可通过 CoreGraphics 中直接修改 CTM 的相关方法实现：

```
CG_EXTERN void CGContextTranslateCTM(CGContextRef __nullable c,
    CGFloat tx, CGFloat ty)
CG_EXTERN void CGContextScaleCTM(CGContextRef __nullable c,
    CGFloat sx, CGFloat sy)
CG_EXTERN void CGContextRotateCTM(CGContextRef __nullable c, CGFloat angle)
CG_EXTERN void CGContextConcatCTM(CGContextRef __nullable c,
    CGAffineTransform transform)
```

同样的变换效果也可以通过 CGAffineTransform 相关方法配合 CGContextConcatCTM 实现：

```
CGAffineTransform transform = CGAffineTransformIdentity;
transform = CGAffineTransformTranslate(transform, 100, 100);
transform = CGAffineTransformRotate(transform, M_PI_4);
transform = CGAffineTransformScale(transform, .5, .5);
CGContextConcatCTM(context, transform);
```

4、绘制直线/曲线

有以下两种类型的绘制方式：

* 直接在 context 里作画

```
CG_EXTERN void CGContextMoveToPoint(CGContextRef __nullable c,
    CGFloat x, CGFloat y)
```
开始一个新的 subpath，并将起点设置在(x,y)

```
CG_EXTERN void CGContextAddLineToPoint(CGContextRef __nullable c,
    CGFloat x, CGFloat y)
CG_EXTERN void CGContextAddLines(CGContextRef __nullable c,
    const CGPoint * __nullable points, size_t count)
```
从当前点画一条到(x,y)点的直线。


```
CG_EXTERN void CGContextAddRect(CGContextRef __nullable c, CGRect rect)
CG_EXTERN void CGContextAddRects(CGContextRef __nullable c,
    const CGRect * __nullable rects, size_t count)
```
画矩形区域。

```
CG_EXTERN void CGContextAddEllipseInRect(CGContextRef __nullable c, CGRect rect)
```
画椭圆。

```
CG_EXTERN void CGContextAddCurveToPoint(CGContextRef __nullable c, CGFloat cp1x,
    CGFloat cp1y, CGFloat cp2x, CGFloat cp2y, CGFloat x, CGFloat y)
CG_EXTERN void CGContextAddQuadCurveToPoint(CGContextRef __nullable c,
    CGFloat cpx, CGFloat cpy, CGFloat x, CGFloat y)
```
画贝塞尔曲线和二次贝塞尔曲线。

```
CG_EXTERN void CGContextAddArc(CGContextRef __nullable c, CGFloat x, CGFloat y,
    CGFloat radius, CGFloat startAngle, CGFloat endAngle, int clockwise)
CG_EXTERN void CGContextAddArcToPoint(CGContextRef __nullable c,
    CGFloat x1, CGFloat y1, CGFloat x2, CGFloat y2, CGFloat radius)
```
画圆/弧。


```
CG_EXTERN void CGContextSetLineDash(CGContextRef __nullable c, CGFloat phase,
    const CGFloat * __nullable lengths, size_t count)
```
画虚线

```
CG_EXTERN void CGContextClosePath(CGContextRef __nullable c)
```
将最近的一个 subpath 的起点和终点连接到一起，形成封闭空间。和 CGContextBeginPath 方法没有配对关系。

* 制作 path 后再将其绘制到 context 

其基本方法在上面都能找到对应的，其 API 包含在 CGPath.h 文件中，这里不一一列出，只举个例子：

```
- (void)drawRect:(CGRect)rect{
    CGContextRef currentContext = UIGraphicsGetCurrentContext();
    
    // 创建并绘制 path
    CGMutablePathRef path = CGPathCreateMutable();
    CGPathMoveToPoint(path,NULL, self.frame.size.width, self.frame.origin.y);
    CGPathAddEllipseInRect(path, &CGAffineTransformIdentity, CGRectMake(0, 320, 320, 160));
    CGPathAddLineToPoint(path,NULL, self.frame.origin.x, self.frame.size.height);
    
    // 将 path 添加到绘制 context
    CGContextAddPath(currentContext, path);

    // 绘制 path
    CGContextDrawPath(currentContext, kCGPathStroke);
    CGPathRelease(path);
}

```

5、描绘/填充绘制区域

```
CG_EXTERN void CGContextStrokePath(CGContextRef __nullable c)
```
将 path 用指定的颜色、宽度等属性描绘出来。如果不调用该方法我们是看不到绘制出来的线的。

```
CG_EXTERN void CGContextFillPath(CGContextRef __nullable c)
```
使用 winding-number 规则将 path 所围区域用指定的颜色填充。如果path没有闭合，填充会默认闭合。

```
CG_EXTERN void CGContextEOFillPath(CGContextRef __nullable c)
```
功能同 CGContextFillPath 方法，只是填充规则变成 even-odd。

以上描绘/填充模式也可以使用下面方法通过设置 CGPathDrawingMode 的方式实现上述方法同样的功能：

```
CG_EXTERN void CGContextDrawPath(CGContextRef __nullable c,
    CGPathDrawingMode mode)
```

其中：

```
typedef CF_ENUM (int32_t, CGPathDrawingMode) {
  kCGPathFill,				// 等价于 CGContextFillPath 方法
  kCGPathEOFill,				// 等价于 CGContextEOFillPath 方法
  kCGPathStroke,				// 等价于 CGContextStrokePath 方法
  kCGPathFillStroke,		// 等价于 CGContextFillPath 和 CGContextStrokePath 方法的合成效果
  kCGPathEOFillStroke		// 等价于 kCGPathEOFill 和 CGContextStrokePath 方法的合成效果
};
```

剩余其它描绘/填充方法都类似。

6、裁剪

```
CG_EXTERN void CGContextClip(CGContextRef __nullable c)  // the winding-number fill rule
```

该方法使用一个 path（是否闭合都可以）去裁剪另一个 path，或者说该方法以取交集的方式来得到两个 path 的共同区域。使用方式是首先画一个 path（不能对该 path 调用 fill/stroke 方法）；然后调用 CGContextClip 方法；其后再画另一个 path；最后调用 fill/stroke 方法将裁剪得到的 path 画出来。比如：

```
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetStrokeColorWithColor(context, [UIColor redColor].CGColor);
    CGContextSetLineWidth(context, 1);
   
    CGContextBeginPath(context);
    
    CGContextMoveToPoint(context, 5, 5);
    CGContextAddLineToPoint(context, 5, 30);
    CGContextAddLineToPoint(context, 30, 30);
    CGContextAddLineToPoint(context, 30, 5);
//    CGContextClosePath(context);
    
    CGContextClip(context);
//    CGContextClipToRect(context, CGRectMake(5, 5, 25, 25));
    
    CGContextMoveToPoint(context, 10, 10);
    CGContextAddLineToPoint(context, 10, 100);
    CGContextAddLineToPoint(context, 100, 100);
    CGContextAddLineToPoint(context, 100, 60);
    CGContextClosePath(context);
    
    CGContextStrokePath(context);
``` 

与该方法功能类似的还有以下方法：

```
CG_EXTERN void CGContextClipToRect(CGContextRef __nullable c, CGRect rect)
CG_EXTERN void CGContextClipToRects(CGContextRef __nullable c, const CGRect *  rects, size_t count)
CG_EXTERN void CGContextEOClip(CGContextRef __nullable c)   // even-odd fill rule
CG_EXTERN void CGContextClipToMask(CGContextRef __nullable c, CGRect rect, CGImageRef __nullable mask)
```

7、context
