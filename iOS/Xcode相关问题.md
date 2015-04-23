## Xcode相关问题

记录和Xcode相关的术语含义、问题及解决办法等。

#### Base SDK and Deployment Target设置项区别

* Deployment Target

该设置项标识软件支持的最低系统版本。Deployment Target和比它早的系统的特性都可以无条件使用。

* Base SDK

标识应用能支持的最高系统版本。默认情况下Xcode会将该选项设置为当前最新的系统版本。

二者之间的关系如图所示：

![Deployment Target and Base SDK](https://developer.apple.com/library/mac/documentation/DeveloperTools/Conceptual/cross_development/art/using_sdks_2x.png)

PS:

* iOS或OS X 的SDK中的libraries仅仅用于链接，它们本身并不包含任何可执行文件。SDK只有和编译目标机器配合才能正常工作。

* 当使用Simulator SDKs编译的时候，生成的二进制文件只使用Base SDK指定的系统版本，即和Deployment Target指定的系统版本没有直接关系。


#### Xcode升级后插件失效解决办法

*  查看Xcode对应版本的UUID：

```
tail -f /var/log/system.log
```

*  更新插件配置文件

得到UUID后执行命令：

```
find ~/Library/Application\ Support/Developer/Shared/Xcode/Plug-ins -name Info.plist -maxdepth 3 | xargs -I{} defaults write {} DVTPlugInCompatibilityUUIDs -array-add XCode_UUID
```

* 参考链接：

[XCode升级后插件失效的原理与修复办法](http://joeshang.github.io/2015/04/10/fix-xcode-upgrade-plugin-invalid/)

#### Target/Project/workspace区别

* Xcode Target

Target是project或者workspace中通过一系列文件去定义一个product的编译方式的最小单位。它将product的组成部分：源代码和编译说明文件组织在一起，以供编译器使用。一个project能包含若干个target，但是一个target只有一个product（编译结果）。

编译说明文件有两种形式：build settings 和 build phases（构建阶段）。Target的build settings是从 project中继承的，但是可以在target对应的build settings中重写那些继承项；但是build phases则是每个Target独有的。

每次只能通过Xcode scheme指定唯一一个活跃的target。

target之间可以相互关联，在同一个workspace中Xcode能自动处理target之间的依赖关系，这被称为隐式依赖；也可以指定target之间的显示依赖关系。

* Xcode Project

Xcode project包含了编译product所需的全部代码文件、资源文件、设置项。

Project可以包含若干个target。

Project会为所有Target指定一个默认的build settings选项。

* Xcode workspace

workspace可以包含若干个project以及	其它的文档。

* Xcode Scheme

Xcode scheme 定义了指定target在Build/Run/Test/Profile/Analyze/Archive等模式下的配置项，如图所示：

![scheme](https://github.com/wangzz/Blog/blob/master/image/Xcode%E7%9B%B8%E5%85%B3%E9%97%AE%E9%A2%98/scheme.png)

需要注意的是：

一个scheme只能对应一个Target；

scheme可以有很多个，但是只能有一个处于活跃状态；

scheme可以属于project，也可以属于workspace。


