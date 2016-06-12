## @weakif和@strongif详解

#### 一、简介

RAC 里出现一种牛逼轰轰的 block 防止循环引用的方法，使用方式如下：

```
id foo = [[NSObject alloc] init];
id bar = [[NSObject alloc] init];

@weakify(foo, bar);

// this block will not keep 'foo' or 'bar' alive
BOOL (^matchesFooOrBar)(id) = ^ BOOL (id obj){
    // but now, upon entry, 'foo' and 'bar' will stay alive until the block has
    // finished executing
    @strongify(foo, bar);

    return [foo isEqual:obj] || [bar isEqual:obj];
};
```

神奇的 @weakif 和 @strongif 宏定义是如何实现的？首先通过 Xcode --> Product --> PerformAction --> Preprocess 将使用这个两个宏的文件预编译以后可以看到它们的最终展开结果如下：

```
// @weakif
@autoreleasepool {} __attribute__((objc_ownership(weak))) __typeof__(self) self_weak_ = (self);

// @strongif
@autoreleasepool {} __attribute__((objc_ownership(strong))) __typeof__(self) self = self_weak_;
```

其中 __attribute__((objc_ownership(weak))) 和 __attribute__((objc_ownership(strong))) 分别是 __weak 和 __strong 的实现部分，也就是说最终这两个宏相当于：

```
// @weakif
@autoreleasepool {} __weak __typeof__(self) self_weak_ = (self);

// @strongif
@autoreleasepool {} __strong __typeof__(self) self = self_weak_;
```

二、宏定义详解

下面以 @weakif 为例来探究该宏定义的实现原理。

* 原始定义

```
#define weakify(...) \
    rac_keywordify \
    metamacro_foreach_cxt(rac_weakify_,, __weak, __VA_ARGS__)
```

其中 rac_keywordify 定义如下：

```
#if DEBUG
#define rac_keywordify autoreleasepool {}
#else
#define rac_keywordify try {} @catch (...) {}
#endif
```

目前 DEBUG 下宏定义变成了：

```
#define weakify(...) \
    autoreleasepool {} \
    metamacro_foreach_cxt(rac_weakify_,, __weak, __VA_ARGS__)
```

* 第二层定义

metamacro_foreach_cxt 定义如下：

```
#define metamacro_foreach_cxt(MACRO, SEP, CONTEXT, ...) \
        metamacro_concat(metamacro_foreach_cxt, metamacro_argcount(__VA_ARGS__))(MACRO, SEP, CONTEXT, __VA_ARGS__)
```

这是一个递归宏定义。由于 metamacro_concat 表示连接两个参数，而且 metamacro_argcount(__VA_ARGS__) 结果为 1，因此第二层定义就变成了：

```
#define metamacro_foreach_cxt1(MACRO, SEP, CONTEXT, __VA_ARGS__)
```

其中：

MACRO 为 rac_weakify_；SEP 为空；CONTEXT 为 __weak；.../__VA_ARGS__ 为 self。

目前宏定义分解后就成了这个样子：

```
#define weakify(...) \
    autoreleasepool {} \
    metamacro_foreach_cxt1(rac_weakify_,, __weak, self)
```

* 第三层定义

metamacro_foreach_cxt1 的定义如下：

```
#define metamacro_foreach_cxt1(MACRO, SEP, CONTEXT, _0) MACRO(0, CONTEXT, _0)
```
其中， _0 表示 self。因此，该宏成了：

```
rac_weakify_(0, __weak, self)
```

即 DEBUG 下宏定义变成了：

```
#define weakify(...) \
    autoreleasepool {} \
    rac_weakify_(0, __weak, self)
```

* 最终定义

rac_weakify_ 定义如下：

```
#define rac_weakify_(INDEX, CONTEXT, VAR) \
    CONTEXT __typeof__(VAR) metamacro_concat(VAR, _weak_) = (VAR);
```

其中 metamacro_concat 用于连接参数，因此最终宏定义就成了：

```
#define weakify(...) \
    autoreleasepool {} \
    __weak __typeof__(self) self_weak_ = (self);
```

三、参考文档

* [Reactive Cocoa中@weakify和@strongify实现分析](http://blog.csdn.net/taishanduba/article/details/47363147)