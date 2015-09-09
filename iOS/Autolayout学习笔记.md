## Autolayout学习笔记

#### 一、VFL

###### 1、简介

VFL(Visual Format Language)是Autolayout界面布局时使用的一种可视化标记语言。从名称中可以看出，其最大的特点是可视化，用户很容易看懂它所表示的含义。

本文重在介绍VFL语法。

###### 2、使用方式

NSLayoutConstraint类提供了一个VFL相关类方法：

```objective-c
+ (NSArray *)constraintsWithVisualFormat:(NSString *)format options:(NSLayoutFormatOptions)opts
metrics:(NSDictionary *)metrics
views:(NSDictionary *)views;
```

该方法返回包含由VFL确定的NSLayoutConstraint对象的数组。

###### 3、语法

* 标准间隔

```
[button]-[textField]
```
对应效果：
![standardSpace](https://github.com/wangzz/Blog/blob/master/image/Autolayout%E4%B9%8BVFL%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/standardSpace.png)

* 宽度约束

```
[button(>=50)]
```

表示button的宽度大于等于50，对应效果：

![widthConstraint](https://github.com/wangzz/Blog/blob/master/image/Autolayout%E4%B9%8BVFL%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/widthConstraint.png)

同理可为为高度设置约束。

* 相对Superview约束

```
|-50-[purpleBox]-50-|
```
表示purpleBox距离Superview的左右边距为50，效果如图：

![Connection to Superview](https://github.com/wangzz/Blog/blob/master/image/Autolayout%E4%B9%8BVFL%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/connectionToSuperview.png)

* 竖直方向约束

```
V:[topField]-10-[bottomField]
```

表示topField和bottomField在竖直方向间距为10，效果如图：

![Vertical Layout](https://github.com/wangzz/Blog/blob/master/image/Autolayout%E4%B9%8BVFL%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/verticalLayout.png)

需要指出的是，如果没有指明`V:`，均表示约束是水平方向，即默认约束都是水平方向的。

* 紧邻布局

```
[maroonView][blueView]
```

表示maroonView和blueView紧挨，即间距为0，效果如图：

![Flush Views](https://github.com/wangzz/Blog/blob/master/image/Autolayout%E4%B9%8BVFL%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/flushViews.png)

* 约束优先级

```
[button(100@20)]
```

表示将button的宽度设为100，同时将该约束的优先级设置为20，如果针对button宽度有比20优先级高的约束，则该约束无效。如图所示：

![Priority](https://github.com/wangzz/Blog/blob/master/image/Autolayout%E4%B9%8BVFL%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/priority.png)

* 等宽约束

```
[button1(==button2)]
```

表示建立一个使得button1和button2宽度相同的约束，效果如图：

![Equal Widths](https://github.com/wangzz/Blog/blob/master/image/Autolayout%E4%B9%8BVFL%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/equalWidths.png)

* 多条件约束

```
[flexibleButton(>=70,<=100)]
```

表示flexibleButton的宽度约束为大于等于70，小于等于100，具体值要根据整体约束情况确定，效果如图：

![Multiple Predicates](https://github.com/wangzz/Blog/blob/master/image/Autolayout%E4%B9%8BVFL%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0/multiplePredicates.png)

#### 二、几个重布局方法区别

* setNeedsUpdateConstraints

告诉系统我的某个属性改变了，可能影响到界面布局，系统会在未来的某个时刻调用 updateConstraints 方法重新布局。

* updateConstraints

重新布局界面。

* needsUpdateConstraints

布局系统使用这个方法的返回值来确定是否需要调用 updateConstraints 方法。

* updateConstraintsIfNeeded

系统会根据 needsUpdateConstraints 方法的返回值来确定是否需要调用 updateConstraints 来重新布局。

#### 参考链接

* [Auto Layout Guide](https://developer.apple.com/library/ios/documentation/UserExperience/Conceptual/AutolayoutPG/VisualFormatLanguage/VisualFormatLanguage.html#//apple_ref/doc/uid/TP40010853-CH3-SW1)
