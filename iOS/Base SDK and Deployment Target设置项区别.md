## Base SDK and Deployment Target设置项区别

* Deployment Target

该设置项标识软件支持的最低系统版本。Deployment Target和比它早的系统的特性都可以无条件使用。

* Base SDK

标识应用能支持的最高系统版本。默认情况下Xcode会将该选项设置为当前最新的系统版本。

二者之间的关系如图所示：

![Deployment Target and Base SDK](https://developer.apple.com/library/mac/documentation/DeveloperTools/Conceptual/cross_development/art/using_sdks_2x.png)

PS:

* iOS或OS X 的SDK中的libraries仅仅用于链接，它们本身并不包含任何可执行文件。SDK只有和编译目标机器配合才能正常工作。

* 当使用Simulator SDKs编译的时候，生成的二进制文件只使用Base SDK指定的系统版本，即和Deployment Target指定的系统版本没有直接关系。
