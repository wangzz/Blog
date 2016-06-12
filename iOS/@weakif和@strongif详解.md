## @weakif和@strongif详解

RAC 里出现一种牛逼轰轰的 block 防止循环引用的方法，使用方式如下：

```
* @code

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

 * @endcode
```

神奇的 @weakif 和 @strongif 是如何实现的？

一、宏定义详解

* 一层定义

@weakif 和 @strongif 都是宏，其定义如下：

```
#define weakify(...) \
    rac_keywordify \
    metamacro_foreach_cxt(rac_weakify_,, __weak, __VA_ARGS__)

#define strongify(...) \
    rac_keywordify \
    _Pragma("clang diagnostic push") \
    _Pragma("clang diagnostic ignored \"-Wshadow\"") \
    metamacro_foreach(rac_strongify_,, __VA_ARGS__) \
    _Pragma("clang diagnostic pop")
```

* 二层定义

由此看出，@weakif 和 @strongif 的定义都可以分解成两部分：

```
@part1

 rac_keywordify

@endpart1

@part2

// @weakif
metamacro_foreach_cxt(rac_weakify_,, __weak, __VA_ARGS__)

//@strongif
metamacro_foreach(rac_strongify_,, __VA_ARGS__)

@endpart2

```

而 part1 定义为：

```
#if DEBUG
#define rac_keywordify autoreleasepool {}
#else
#define rac_keywordify try {} @catch (...) {}
#endif
```

part2 定义为：

```
// @weakif
#define metamacro_foreach_cxt(MACRO, SEP, CONTEXT, ...) \
        metamacro_concat(metamacro_foreach_cxt, metamacro_argcount(__VA_ARGS__))(MACRO, SEP, CONTEXT, __VA_ARGS__)
        
//@strongif
#define metamacro_foreach(MACRO, SEP, ...) \
        metamacro_foreach_cxt(metamacro_foreach_iter, SEP, MACRO, __VA_ARGS__)
```

因此，DEBUG 条件下 @weakif 和 @strongif 的就变成了：

```
#define weakify(...) \
    autoreleasepool {} \
    metamacro_concat(metamacro_foreach_cxt, metamacro_argcount(__VA_ARGS__))(MACRO, SEP, CONTEXT, __VA_ARGS__)

#define strongify(...) \
    autoreleasepool {} \
    metamacro_foreach_cxt(metamacro_foreach_iter, SEP, MACRO, __VA_ARGS__)
```

* 三层定义

1、metamacro_concat 定义如下：

```
#define metamacro_concat(A, B) \
        metamacro_concat_(A, B)
```

metamacro_concat_ 定义如下：

```
#define metamacro_concat_(A, B) A ## B
```

实际上就是把 A 和 B 连接起来。

2、metamacro_foreach_cxt 定义如下：

```
#define metamacro_foreach_cxt(MACRO, SEP, CONTEXT, ...) \
        metamacro_concat(metamacro_foreach_cxt, metamacro_argcount(__VA_ARGS__))(MACRO, SEP, CONTEXT, __VA_ARGS__)
```

最终也是指向 metamacro_concat。

3、最终 @weakif 和 @strongif 的就变成了：

```
#define weakify(...) \
    autoreleasepool {} \
    metamacro_concat(metamacro_foreach_cxt, metamacro_argcount(__VA_ARGS__))(MACRO, SEP, CONTEXT, __VA_ARGS__)

#define strongify(...) \
    autoreleasepool {} \
    metamacro_concat(metamacro_foreach_cxt, metamacro_argcount(__VA_ARGS__))(MACRO, SEP, CONTEXT, __VA_ARGS__)
```












