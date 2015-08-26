### 对Objective-C语言动态性的理解

###### 1、数据类型

对象的类型（包括id类型和具体类型）都是不可靠的，其真实身份只有在运行时通过以下方式：

```
id obj=self;
if ([obj respondsToSelector:@selector(function1:)) {
}
if ([obj isKindOfClass:[NSArray class]] ) {
}
if ([obj conformsToProtocol:@protocol(myProtocol)]) {
}
if ([[obj class] isSubclassOfClass:[NSArray class]]) {
}
```

才能判断。

###### 2、方法调用

严格来说OC里不存在方法调用，只有向一个对象发送消息。二者的区别是：

* 方法调用

一旦确定了调用哪个方法，就能且只能调用该方法

* 发送消息时

在Runtime的消息转发机制中，首先，最终响应该消息的对象不一定是当前接收消息的对象；其次，该消息对应的最终响应方法调用可以任意指定，不一定是消息对应的方法调用；最后，该消息对应的方法是否存在只有通过运行时的消息转发机制在运行时才能判断。

因此，消息机制就给了我们偷梁换柱的能力，大大增加了方法调用的灵活性。

###### 3、类的创建

静态语言中类的定义必须在编译前完成，OC里可以实现运行时创建新类，并为其增加属性、方法。


