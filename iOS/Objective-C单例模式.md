单例的写法有很多种

## 一、是否线程安全

可以分为下面两大类：

### 1、非线程安全写法

```
+ (MyClass *)sharedInstance {
    static MyClass *sharedInstance;
    if (sharedInstance == nil) {
        sharedInstance = [[MyClass alloc] init];
    }
    return sharedInstance;
}
```

### 2、线程安全写法

第一种：

```
static Foo *gSharedFoo;
    
+ (void)initialize {
    if(self == [Foo class])
        gSharedFoo = [[self alloc] init];
}
    
+ (id)sharedFoo {
    return gSharedFoo;
}
```

第二种：

```
+ (MyClass *)sharedInstance {
    static MyClass *sharedInstance;
    @synchronized(self) {
        if (sharedInstance == nil) {
            sharedInstance = [[MyClass alloc] init];
        }
    }
    return sharedInstance;
}
```

第三种：

```
+ (MYClass *)sharedInstance
{
    static dispatch_once_t pred = 0;
    static MYClass _sharedObject = nil;
    dispatch_once(&pred, ^{
        _sharedObject = [[self alloc] init]; // or some other init method
    });
    return _sharedObject;
}
```

## 二、严格单例写法

### 1、苹果官方例子(MRC)

```
static MyGizmoClass *sharedGizmoManager = nil;
 
+ (MyGizmoClass*)sharedManager
{
    if (sharedGizmoManager == nil) {
        sharedGizmoManager = [[super allocWithZone:NULL] init];
    }
    return sharedGizmoManager;
}
 
+ (id)allocWithZone:(NSZone *)zone
{
    return [[self sharedManager] retain];
}
 
- (id)copyWithZone:(NSZone *)zone
{
    return self;
}
 
- (id)retain
{
    return self;
}
 
- (NSUInteger)retainCount
{
    return NSUIntegerMax;  //denotes an object that cannot be released
}
 
- (void)release
{
    //do nothing
}
 
- (id)autorelease
{
    return self;
}
```

### 2、另一种简单严格写法

```
+ (MYSingleton *)sharedSingleton {
     static dispatch_once_t pred;
     static MYSingleton *instance = nil;
     dispatch_once(&pred, ^{instance = [[self alloc] initSingleton];});
     return instance;
}

- (id)init {
    // Forbid calls to –init or +new
    NSAssert(NO, @”Cannot create instance of Singleton”);
    // You can return nil or [self initSingleton] here, 
    // depending on how you prefer to fail.
    return nil;
}

// Real (private) init method
- (id)initSingleton {
    self = [super init];
    if ((self = [super init])) {
    // Init code }
    return self;
}
```

## 三、参考文章

* [Friday Q&A 2009-10-02: Care and Feeding of Singletons](https://mikeash.com/pyblog/friday-qa-2009-10-02-care-and-feeding-of-singletons.html)
* [Cocoa Fundamentals Guide](https://developer.apple.com/legacy/library/documentation/Cocoa/Conceptual/CocoaFundamentals/CocoaObjects/CocoaObjects.html#//apple_ref/doc/uid/TP40002974-CH4-SW32)
* [Singletons in Cocoa, Are They Evil?](http://gracelancy.com/blog/2013/06/04/singletons-in-cocoa-are-they-evil/)