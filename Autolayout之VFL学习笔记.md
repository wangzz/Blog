## Autolayout之VFL学习笔记

###### 简介

VFL(Visual Format Language)是Autolayout界面布局时使用的一种可视化标记语言。从名称中可以看出，其最大的特点是可视化，用户很容易看懂它所表示的含义。

本文重在介绍VFL语法。

###### 使用方式

NSLayoutConstraint类提供了一个VFL相关类方法：

```objective-c
+ (NSArray *)constraintsWithVisualFormat:(NSString *)format options:(NSLayoutFormatOptions)opts
metrics:(NSDictionary *)metrics
views:(NSDictionary *)views;
```

该方法返回包含由VFL确定的NSLayoutConstraint对象的数组。

###### 语法


